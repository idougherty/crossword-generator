#include <list>
#include <stack>
#include <string>
#include <vector>
#include "defs.h"
#include "clue_dictionary.hpp"

using namespace std;

struct Solution {
    int row, col;
    bool is_across;
    int word_num;
    string value;
    string hint;
};

class Variable {
    public:
        list<Clue*> possible_values;
        stack<list<Clue*>> pruned_values;
        Clue* chosen_value;
        vector<pair<Variable*, int>> constraints;    // int is the index of the other variable that overlaps
        string domain;
        int row;
        int col;
        int length;
        int word_num;
        bool is_across;
        bool is_filled;

        Variable();
        Variable(int row, int col, int length, bool is_across);
        bool has_unused_values();
        void set_value(Clue* val);
        void unset_value(string domain);
        bool prune_values(char c, int idx);         // returns true if backtracking is necessary
        void unprune_values(char c, int idx);
        void print();
};