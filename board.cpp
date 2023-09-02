#include <cstdio>
#include <cstdlib>
#include "board.hpp"
#include "defs.h"
#include <string.h>
#include <string>

Board* create_board(int size) {
    int i, j;
    Board* b;
    
    b = new Board; 
    b->size = size;
    b->data = new char[size * size];
    
    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            b->data[i * size + j] = OPEN_TILE;

    return b;
}

void free_board(struct Board* b) {
    free(b);
}

void print_board(struct Board* b) {
    int i, j;

    printf("┏");
    for(i = 0; i < b->size; i++)
        printf("━━");
    printf("┓\n");

    for(i = 0; i < b->size; i++) {
        printf("┃");
        for(j = 0; j < b->size; j++) {
            if(GETVAL(b, i, j) == CLOSED_TILE)
                printf("██");
            else
                printf("%c ", GETVAL(b, i, j));
        }
        printf("┃\n");
    }

    printf("┗");
    for(i = 0; i < b->size; i++)
        printf("━━");
    printf("┛\n");
}

int get_length(struct Board* b, int row, int col, bool is_across, int dir) {
    int length = 0;

    while(col >= 0 && row >= 0 && col < b->size && row < b->size && GETVAL(b, row, col) != CLOSED_TILE) {
        length++;
        is_across ? col += dir : row += dir;
    }

    return length;
}