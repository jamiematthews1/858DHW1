#include <vector>

class bitSelect{
public:
    bitSelect(bitRank*);
    int select1(int);
    int select0(int);
    int select1_range(int, int);
    int select0_range(int, int);
    int overhead();
    int binSearch(int, int, int);

private:
    bitRank *br;

};
