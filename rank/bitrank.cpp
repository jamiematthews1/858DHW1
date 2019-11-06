#include <iostream>
using namespace std;
#include <vector>
#include <math.h>
#include "bitRank.h"

bitRank::bitRank(vector<bool> *bitvec) {
    int num_super;
    int num_block;

    bool curr;
    int super_counter = 0;
    int block_counter = 0;
    int type_counter = 0;
    int temp;

    size_arr = (*bitvec).size();

    //Create super blocks
    size_block = ceil((double) log2(size_arr)/2);
    size_super = ceil((double) log2(size_arr)/2)*ceil(log2(size_arr));

    num_block = ceil((double) size_arr/size_block);
    num_super = ceil((double) size_arr/size_super);

    super = new short [num_super];
    block = new short [num_block];
    type = new short [num_block];

    for(int i=0; i < size_arr; i++) {
        if (i % size_super == 0) {
            super[i/size_super] = super_counter;
            block_counter = 0; //reset block counter when we start a new superblock
        }
        if (i % size_block == 0) {
            block[i/size_block] = block_counter;

            if (i != 0) {
                type[i/size_block - 1] = type_counter;
                type_counter = 0;
            }
        }

        //update counts if curr element is a 1
        if ((*bitvec)[i] == 1) {
            super_counter++;
            block_counter++;
            type_counter = type_counter + pow(2,(size_block-i%size_block-1));
        }
    } // end for
    type[num_block - 1] = type_counter;


    table = new short*[(int)pow(2,size_block)];
    for (int i = 0; i < pow(2,size_block); ++i) {
        table[i] = new short[size_block];
    }

    for (int i = 0; i < pow(2,size_block); ++i) {   // for each row
        temp = i;
        if ((temp- pow(2,size_block-1)) >= 0) {
            temp = (int) i- pow(2,size_block-1);
            table[i][0] = 1;
        }
        else {
            table[i][0] = 0;
        }
        for (int j = 1; j < size_block; ++j) { // for each column
            if ((temp - pow(2,size_block-1-j)) >= 0) {
                temp = temp - pow(2,size_block-1-j);
                table[i][j] = table[i][j-1] + 1;
            }
            else {
                table[i][j] = table[i][j-1];
            }
        }
    }
} // end constructor

int bitRank::rank1(int index) {
    int sum = 0;
    short block_type;

    if (index / size_super > 0) {
        sum = sum + super[index/size_super];
    }
    if (index / size_block > 0) {
        sum = sum + block[index/size_block];
    }

    block_type = type[index/size_block];
    sum = sum + table[block_type][index%size_block];
    return sum;
}

int bitRank::rank0(int index) {
    return index-bitRank::rank1(index) + 1;
}

int bitRank::overhead() {
    int num_block = ceil((double) size_arr/size_block);
    int num_super = ceil((double) size_arr/size_super);
    return 8*(sizeof(int)*3 + sizeof(short)*num_block*2 +
    sizeof(short)*num_super + sizeof(short)*size_block*pow(2,size_block));
}

int bitRank::get_size() {
    return size_arr;
}
