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
        case 'T': return 4;
        default:;
    }
}

//Calculates the energy released when removing the amino acid at position i
unsigned long long calculateEnergy(int posLeft, int posMiddle, int posRight, const vector<int>& stabilityPotentials, 
                                                                    const vector<int>& biochemicalClasses) {
    
    //Calculate the potencial(P) of each one
    int P_left = (posLeft == -1) ? TERMINAL_POTENTIAL : stabilityPotentials[posLeft];
    int P_middle = stabilityPotentials[posMiddle];
    int P_right = (posRight == stabilityPotentials.size()) ? TERMINAL_POTENTIAL : stabilityPotentials[posRight];

    //Calculate the biochemical class (BC) of each one
    int BC_left = (posLeft == -1) ? biochemicalClass('T') : biochemicalClasses[posLeft];
    int BC_middle = biochemicalClasses[posMiddle];
    int BC_right = (posRight == biochemicalClasses.size()) ? biochemicalClass('T') : biochemicalClasses[posRight];

    //Calculate energy
    unsigned long long E1 = P_left*affinity[BC_left][BC_middle]*P_middle;
    unsigned long long E2 = P_middle*affinity[BC_middle][BC_right]*P_right;

    return E1 + E2;
}



unsigned long long calculateTotalEnergy(int n, int l, int r, int k, const vector<vector<unsigned long long>>& maxEnergy,  
                            const vector<int> stabilityPotentials, const vector<int> biochemicalClasses) {
    unsigned long long leftEnergy = (k > l) ? maxEnergy[l][k-1] : 0;
    unsigned long long rightEnergy = (k < l) ? maxEnergy[k+1][r] : 0;

    //Calculate energy when removing k last
    int posMiddle = k;
    int posLeft = (l == 0) ? -1 : l - 1;
    int posRight = (r == n - 1) ? n : r + 1;
    unsigned long long kEnergy = calculateEnergy(posLeft, posMiddle, posRight, stabilityPotentials, biochemicalClasses);

    return leftEnergy + kEnergy + rightEnergy;
}



void fillMaxEnergyTable(int n, vector<vector<unsigned long long>>& maxEnergy, vector<vector<int>>& lastRemoved, 
                            const vector<int> stabilityPotentials, const vector<int> biochemicalClasses){
    //Base cases (intervals of size 1)
    for (int i = 0; i < n; i++) {
        int posMiddle = i;
        int posLeft = i - 1;
        int posRight = i + 1;
        maxEnergy[i][i] = calculateEnergy(posLeft, posMiddle, posRight, stabilityPotentials, biochemicalClasses);
        lastRemoved[i][i] = posMiddle;
    }


    //Fill maxEnergy table for intervals of increasing size (iterative)
    for(int intervalSize = 2; intervalSize <= n; intervalSize++) {
        for(int l = 0; l <= n-intervalSize; l++) {
            int r = l + intervalSize - 1;

            //Initialize with first value
            int k = l;
            unsigned long long bestEnergy = calculateTotalEnergy(n, l, r, k, maxEnergy, stabilityPotentials, biochemicalClasses);
            int bestK = k;
        }
        //FIX ME -- for cm o k
    }
}



int main() {

    //number of amino acids in the sequence
    int n;
    cin >> n;

    //vector with all stability potentials of the n amino acids
    vector<int> stabilityPotentials(n);
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


    

    //maxEnergy[l][r] = maximum energy when removing all amino acids in interval [l,r]
    vector<vector<unsigned long long>> maxEnergy(n, vector<unsigned long long>(n, 0));
    //lastRemoved[l][r] = last amino acid removed in interval [l,r]
    vector<vector<int>> lastRemoved(n, vector<int>(n, -1));

    fillMaxEnergyTable(n, maxEnergy, lastRemoved, stabilityPotentials, biochemicalClasses);

    //FIX ME


}
