#include <iostream>
using namespace std;
#include <vector>
#include <math.h>
#include "../rank/bitRank.h"
#include "bitSelect.h"

bitSelect::bitSelect(bitRank *br1) {
    br = br1;
}

int bitSelect::binSearch(int goal, int digit, int low) {
    int high = (*br).get_size() - 1;
    if (low != 0) { //doing binsearch on part of array
        // update goal to reflect how many 1's are in the lower part of array
        if (digit) {
            goal = goal + (*br).rank1(low-1);
        } else {
            goal = goal + (*br).rank0(low-1);
        }
    }
    int size = high/2;
    int curr = size;
    bool flag;

    while (low < high) {
        size = size/2;
        if(size == 0) {
            size = 1;
        }

        if (digit == 1) {
            flag = ((*br).rank1(curr) < goal);
        }

        if (digit == 0) {
            flag = ((*br).rank0(curr) < goal);
        }

        if(flag) {
            low = curr+1;
            curr = curr + size;
        }
        else {
            high = curr;
            curr = curr - size;
        }
    } //end while

    return low;
}

int bitSelect::select1_range(int goal, int low) {
    return binSearch(goal, 1, low) - low;
}

int bitSelect::select0_range(int goal, int low) {
    return binSearch(goal, 0, low) - low;
}


int bitSelect::select1(int goal) {
    return binSearch(goal, 1, 0);
}

int bitSelect::select0(int goal) {
    return binSearch(goal, 0, 0);
}

int bitSelect::overhead() {
    return sizeof(br)*8 + sizeof(*br)*8 + (*br).get_size();
}
