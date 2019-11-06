#include <vector>

class bitRank {
public:
    bitRank(vector<bool>*);
    int rank1(int);
    int rank0(int);
    int overhead();
    int get_size();

private:
    int size_arr;
    short *super;
    short *block;
    short *type;
    short **table;
    int size_super;
    int size_block;
};
