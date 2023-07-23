#include <list>
#include <stack>
#include <string>
#include <vector>
#include "defs.h"

using namespace std;

class Variable {
    public:
        list<string> possible_values;
        stack<list<string>> pruned_values;
        vector<pair<Variable*, int>> constraints;    // int is the index of the other variable that overlaps
        string value;
        int row;
        int col;
        int length;
        bool is_across;
        bool is_filled;

        Variable();
        Variable(int row, int col, int length, bool is_across);
        bool prune_values(char c, int idx);         // returns true if backtracking is necessary
        void unprune_values(char c, int idx);
        void print();
};