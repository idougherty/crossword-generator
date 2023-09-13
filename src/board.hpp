#include "variable.hpp"

#pragma once

struct Board {
    int size;
    char* data;
};

Board* create_board(int size);
void free_board(struct Board* b);
void print_board(struct Board* b);
void fill_board(vector<Solution> solutions, struct Board* b, bool is_across = true);
void fill_board(vector<Variable*> solutions, struct Board* b, bool is_across = true);
int get_length(Board* b, int row, int col, bool is_across, int dir = 1);