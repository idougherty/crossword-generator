#pragma once

struct Board {
    int size;
    char* data;
};

Board* create_board(int size);
void free_board(struct Board* b);
void print_board(struct Board* b);
int get_length(Board* b, int row, int col, bool is_across, int dir = 1);