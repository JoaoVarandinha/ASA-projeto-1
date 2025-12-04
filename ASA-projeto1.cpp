#include <iostream>
#include <vector>

#define TERMINAL_POTENTIAL 1

using namespace std;

//Affinity table between two biochemical classes (variable)
int affinity[5][5] {
    //P N A B T
     {1,3,1,3,1}, //P
     {5,1,0,1,1}, //N
     {0,1,0,4,1}, //A
     {1,3,2,3,1}, //B
     {1,1,1,1,1}  //T
};

//Convert class to index
int biochemicalClass(char c) {
    switch (c) {
        case 'P': return 0;
        case 'N': return 1;
        case 'A': return 2;
        case 'B': return 3;
        default: return 4;
    }
}

//Calculates the total released when removing the amino acid at position i
unsigned long long calculateEnergy(int posLeft, int posMiddle, int posRight, 
                                    const vector<unsigned long long>& stabilityPotentials, 
                                    const vector<int>& biochemicalClasses) {
    
    //Calculate the potencial(P) of each one
    unsigned long long P_left = ((posLeft < 0) ? TERMINAL_POTENTIAL : stabilityPotentials[posLeft]);
    unsigned long long P_middle = stabilityPotentials[posMiddle];
    unsigned long long P_right = ((posRight >= (int)stabilityPotentials.size()) ? TERMINAL_POTENTIAL : stabilityPotentials[posRight]);

    //Calculate the biochemical class (BC) of each one
    int BC_left = (posLeft < 0) ? biochemicalClass('T') : biochemicalClasses[posLeft];
    int BC_middle = biochemicalClasses[posMiddle];
    int BC_right = ((posRight >= (int)biochemicalClasses.size()) ? biochemicalClass('T') : biochemicalClasses[posRight]);

    //Calculate total
    unsigned long long E1 = P_left*affinity[BC_left][BC_middle]*P_middle;
    unsigned long long E2 = P_middle*affinity[BC_middle][BC_right]*P_right;

    return E1 + E2;
}



void calculateTotalEnergy(int n, int i, int j, 
                            vector<vector<unsigned long long>>& maxEnergy,
                            vector<vector<int>>& lastRemoved,
                            const vector<unsigned long long>& stabilityPotentials, 
                            const vector<int>& biochemicalClasses) {

    unsigned long long best = 0;
    int best_idx = 0;

    for (int idx = i; idx <= j; idx++) {
        unsigned long long leftEnergy = ((idx > i) ? maxEnergy[i][idx - 1] : 0);
        unsigned long long rightEnergy = ((idx < j) ? maxEnergy[idx + 1][j] : 0);
        unsigned long long middleEnergy = calculateEnergy(i-1, idx, j+1, stabilityPotentials, biochemicalClasses);

        unsigned long long total = leftEnergy + middleEnergy + rightEnergy;
        
        if (best < total || (best == total && best_idx < idx)) {
            best = total;
            best_idx = idx;
        }
    }

    maxEnergy[i][j] = best;
    lastRemoved[i][j] = best_idx;
    return;
}



void fillMaxEnergyTable(int n, vector<vector<unsigned long long>>& maxEnergy, vector<vector<int>>& lastRemoved, 
                            const vector<unsigned long long>& stabilityPotentials, const vector<int>& biochemicalClasses){

    //Fill maxEnergy table for intervals of increasing size (iterative)
    for(int intervalSize = 1; intervalSize <= n; intervalSize++) {
        for(int i = 0; i <= n-intervalSize; i++) {
            int j = i + intervalSize - 1;

            //Initialize with first value
            calculateTotalEnergy(n, i, j, maxEnergy, lastRemoved, stabilityPotentials, biochemicalClasses);
        }
    }
}




vector<int> getRemovalOrder(int n, const vector<vector<int>>& lastRemoved) {
    vector<int> order;
    order.reserve(n);

    /*Stack to process intervals iteratively 
      (interval[i, j], shouldAdd)
      shouldAdd = false -> first time we "see" the interval, so we need to process it
      shouldAdd = true -> interval has been processed, we can add the last removed to the order
    */
    vector<pair<pair<int, int>, bool>> stack;
    
    //Inicialize with the whole interval
    stack.push_back({{0, n-1}, false});
    
    while(!stack.empty()) {
        pair<int, int> interval = stack.back().first;
        bool shouldAdd = stack.back().second;

        //Remove the last element
        stack.pop_back();

        int i = interval.first;
        int j = interval.second;

        //Invalid inteval
        if(i > j) continue;

        int lastRemovedIdx = lastRemoved[i][j];

        if(shouldAdd) {
            //Add to the order
            order.push_back(lastRemovedIdx + 1);
        } else {
            //First time we see the interval [i, j]
            //Process the subintervals first and then add the last removed (1st left, 2nd rignt, 3rd current)
            
            //Push the current interval to add it later
            stack.push_back(make_pair(make_pair(i, j), true));

            //Push right subinterval
            stack.push_back(make_pair(make_pair(lastRemovedIdx + 1, j), false));

            //Push left subinterval
            stack.push_back(make_pair(make_pair(i, lastRemovedIdx - 1), false));
        }
    }

    return order;
}



int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);


    //number of amino acids in the sequence
    int n;
    cin >> n;

    //vector with all stability potentials of the n amino acids
    vector<unsigned long long> stabilityPotentials(n);
    for(int i = 0; i < n; i++) {
        cin >> stabilityPotentials[i];
    }

    //string of n biochemical class characters(without spaces)
    string classes_str;
    cin >> classes_str;
    //vector with the biochemical classes of all n amino acids (converted from char to index)
    vector<int> biochemicalClasses(n);
    for(int i = 0; i < n; i++) {
        biochemicalClasses[i] = biochemicalClass(classes_str[i]);
    }


    

    //maxEnergy[l][r] = maximum total when removing all amino acids in interval [l,r]
    vector<vector<unsigned long long>> maxEnergy(n, vector<unsigned long long>(n));
    //lastRemoved[l][r] = last amino acid removed in interval [l,r]
    vector<vector<int>> lastRemoved(n, vector<int>(n, -1));

    fillMaxEnergyTable(n, maxEnergy, lastRemoved, stabilityPotentials, biochemicalClasses);

    vector<int> removalOrder = getRemovalOrder(n, lastRemoved);

    //Output
    cout << maxEnergy[0][n-1] << "\n";

    for (int idx = 0; idx < (int)removalOrder.size(); idx++) {
        cout << removalOrder[idx];
        if (idx < (int)removalOrder.size() - 1) cout << " ";
    }
    cout << "\n";

    return 0;
}
