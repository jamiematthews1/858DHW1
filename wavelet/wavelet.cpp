#include <iostream>
using namespace std;
#include <vector>
#include <math.h>
#include "../rank/bitRank.h"
#include "../select/bitSelect.h"
#include <fstream>
#include <bitset>
#include <unordered_map>
#include <string>

unordered_map<char, string> map_bits;
unordered_map<char, short> map_nums;
int *cul_hist;
string alphabet;
vector<vector<bool> > wt;

// Method to load a WT from file
void load_wt(string input_file) {
    ifstream input(input_file);
    string line;
    char *token;
    int num_lines;

    getline(input, line);
    num_lines = stoi(line);
    wt.resize(num_lines);

    getline(input, line);
    int num_chars = stoi(line);
    for (int i=0; i<num_lines; i++) {
        wt[i].resize(num_chars);
    }
    wt.resize(num_lines);

    //read WT
    for (int i=0; i<num_lines; i++) {
        int count = 0;
        getline(input, line);
        char cstr[line.size()+1];
        strcpy(cstr, line.c_str());
        token = strtok(cstr, " ");

        while (token != NULL) {
            wt[i][count] = (token[0] == '1');
            count++;
            token = strtok(NULL, " ");
        }
    }


    // read cul_hist data
    getline(input, line);
    char cstr1[line.size()+1];
    strcpy(cstr1, line.c_str());
    token = strtok(cstr1, " ");
    int count = 0;
    cul_hist = new int[num_chars];
    while (token != NULL) {
        cul_hist[count] = atoi(token);
        count++;
        token = strtok(NULL, " ");
    }

    // read mapping data
    getline(input, line);
    alphabet = line;
    char cstr[line.size()+1];
    strcpy(cstr, line.c_str());
    token = strtok(cstr, " ");
    count = 0;

    while (token != NULL) {
        map_nums[token[0]] = count;
        map_bits[token[0]] = bitset<8>(count).to_string().substr(8-num_lines,num_lines);
        count++;
        token = strtok(NULL, " ");
    }

    input.close();
}

string int_to_string(int num, int length) {
    return(bitset<8>(num).to_string().substr(8-length,length));
}

string prefix(int length_encoding, int num, int wanted_length) {
    return(bitset<8>(num).to_string().substr(8-length_encoding,wanted_length));
}

int rank_helper(int index, char character) {
    bitRank br(&wt[0]);
    bool bit;
    int high = wt[0].size()-1;
    int low = 0;
    int num_bits = wt.size();

    for (int i=0; i<num_bits; i++) {
        br = bitRank(&wt[i]);
        bit = map_bits[character].at(i) == '1';
        if (bit) {
            if (low == 0) {
                low = br.rank0(high);
                index = br.rank1(index) - 1;
            } else {
                index = br.rank1(index+low) - br.rank1(low-1) -1;
                low = low + br.rank0(high) - br.rank0(low-1);
            }
        } else {
            if (low == 0) {
                index = br.rank0(index) - 1;
                high = high - br.rank1(high);
            } else {
                index = br.rank0(index+low) -  br.rank0(low-1) -1;
                high = high - br.rank1(high) + br.rank1(low-1);
            }
        }
    }

    return(index+1);
}

void build(string input, string output) {
    string line;
    vector<char> sorted;
    int num_unique;
    vector<char>::iterator it;
    int num_bits;
    int *hist;
    int num_chars;
    unordered_map<string, int> spos;
    int pos;

    ifstream infile(input);
    istream_iterator<char> start(infile), end;
    vector<char> chars(start, end);
    infile.close();
    num_chars = chars.size();
    cout << "Number of characters in string; " << num_chars << std::endl;

    sorted = chars;
    sort(sorted.begin(), sorted.end());
    it = unique(sorted.begin(), sorted.end());
    num_unique = distance(sorted.begin(),it);
    num_bits = ceil(log2(num_unique));
    sorted.resize(num_unique);

    cout << "Number of unique character in alphabet: " << num_unique << "\n";

    //create hashmap of alphabet characters to bit vectors (strings)
    for(int i=0; i<num_unique; i++) {
        map_bits[sorted[i]] = bitset<8>(i).to_string().substr(8-num_bits,num_bits);
    }

    for(int i=0; i<num_unique; i++) {
        map_nums[sorted[i]] = i;
    }


    //create space for hist and wt
    hist = new int[num_unique];
    for(int i=0; i<num_unique; i++) {
        hist[i] = 0;
    }

    cul_hist = new int[num_unique];
    wt.resize(num_bits);
    for (int i=0; i<num_bits; i++) {
        wt[i].resize(num_chars);
    }

    //create wavetree
    for (int i=0; i<num_chars; i++) {
        hist[map_nums[chars[i]]]++;
        wt[0][i] = (map_bits[chars[i]].at(0) == '1');
    }

    //create cul_hist
    cul_hist[0] = 0;
    for (int i=1; i<num_unique; i++) {
        cul_hist[i] = cul_hist[i-1] + hist[i-1];
    }

    // create layers 1 to num_bits-1 of WT
    for (int l=num_bits-1; l>0; l--) {
        for (int i=0; i < pow(2,l); i++) {
            hist[i] = hist[2*i] + hist[2*i + 1];
        }
        spos[int_to_string(0,l)] = 0;
        for (int i=1; i < pow(2,l); i++) {
            spos[int_to_string(i,l)] = spos[int_to_string(i-1,l)] + hist[i-1];
        }
        for (int i=0; i < num_chars; i++) {
            pos = spos[prefix(num_bits, map_nums[chars[i]], l)]++;
            wt[l][pos] = (map_bits[chars[i]].at(l) == '1');
        }
    }

    ofstream output_file(output);
    output_file << num_bits << "\n" << num_chars << "\n";
    for (int i=0; i<num_bits; i++) {
        for (int j=0; j<num_chars; j++) {
            output_file << wt[i][j] << " ";
        }
        output_file << "\n";
    }

    for (int i=0; i<num_unique; i++) {
        output_file << cul_hist[map_nums[sorted[i]]] << " ";
    }
    output_file << "\n";

    for (int i=0; i<num_unique; i++) {
        output_file << sorted[i] << " ";
    }
    output_file << "\n";

}

char query(int index) {
    bitRank br(&(wt[0]));
    bool bit;
    int l = wt.size();
    int high = wt[0].size()-1;
    int low = 0;
    int answer = 0;
    for (int i=0; i<l; i++) {
        br = bitRank(&(wt[i]));
        bit = wt[i][index+low];
        answer = answer + bit*pow(2, l-1-i);
        if (bit) {
            if (low == 0) {
                low = br.rank0(high);
                index = br.rank1(index) - 1;
            } else {
                index = br.rank1(index+low) - br.rank1(low-1) -1;
                low = low + br.rank0(high) - br.rank0(low-1);
            }
        } else {
            if (low == 0) {
                index = br.rank0(index) - 1;
                high = high - br.rank1(high);
            } else {
                index = br.rank0(index+low) -  br.rank0(low-1) -1;
                high = high - br.rank1(high) + br.rank1(low-1);
            }
        }
    }
    return alphabet[answer*2];
}

void access(string tree, string indices) {
    load_wt(tree);

    ifstream input(indices);
    string line;
    while (getline(input, line)) {
        cout << query(stoi(line)) << "\n";
    }

}

void get_rank(string tree, string indices) {
    load_wt(tree);

    ifstream input(indices);
    int index;
    char character;
    while (input >> character) {
        input >> index;
        cout << rank_helper(index, character) << "\n";
    }
}

int select_helper(int index, char character) {
    bitRank br(&wt[0]);
    bitSelect bs(&br);
    bool bit;
    int num_bits = wt.size();
    int char_as_int = map_nums[character];
    int hist_index;

    for (int i=num_bits-1; i>=0; i--) {
        br = bitRank(&wt[i]);
        bs = bitSelect(&br);
        bit = map_bits[character].at(i) == '1';

        hist_index = (int) floor(char_as_int/pow(2, num_bits-i)) * pow(2,num_bits-i);
        if (bit) {
            index = bs.select1_range(index, cul_hist[hist_index]) +1;
        } else {
            index = bs.select0_range(index, cul_hist[hist_index]) +1;
        }
    }
    return index-1;
}

void select(string tree, string indices) {
    load_wt(tree);

    ifstream input(indices);
    int index;
    char character;

    while (input >> character) {
        input >> index;
        cout << select_helper(index, character) << "\n";
    }
}

int main(int argc, char** argv)
{

    if (argc >= 4) {
        if (((string)"build").compare(argv[1]) == 0) {
            build(argv[2], argv[3]);
        } else if (((string) argv[1]).compare("access") == 0) {
            access(argv[2], argv[3]);
        } else if (((string) argv[1]).compare("rank") == 0) {
            get_rank(argv[2], argv[3]);
        } else if (((string) argv[1]).compare("select") == 0) {
            select(argv[2], argv[3]);
        } else {
            cout << "Unknown command. Please use eihter build, access, rank, or select.";
        }
    }

}
