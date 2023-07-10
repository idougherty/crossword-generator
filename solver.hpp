#include <list>
#include <string>
#include <vector>
#include "board.hpp"
#include "solution_dictionary.hpp"
#include "defs.h"

using namespace std;

void solve_board(struct Board* b, SolutionDictionary dict);

class Variable {
    public:
        list<string> possible_values;
        list<string> pruned_values;
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
        void unprune_values(char c, int idx);         // returns true if backtracking is necessary
        void print();
};