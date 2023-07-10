#include <cstdio>
#include <cstdlib>
#include <exception>
#include <vector>
#include "board.hpp"
#include "board_generator.hpp"
#include "defs.h"

using namespace std;

bool can_block_cell(Board* b, int idx);
bool is_in_corner(Board* b, int idx);
bool surrounds_cell(Board* b, int idx);
bool violates_min_length(Board* b, int idx);
int walk_idx(Board*b, int idx);

// Takes an empty board and generates in closed squares
void generate_blocking_tiles(Board* b) {

    // b->data[0] = CLOSED_TILE;
    // b->data[4] = CLOSED_TILE;
    // b->data[20] = CLOSED_TILE;
    // b->data[24] = CLOSED_TILE;

    // return;

    int i, j, idx, cell, iterations;
    
    // vector of indices of open spots
    // TODO: should this be a pointer?
    auto *open_cells = new vector<int>();

    for(i = 0; i < b->size; i++)
        for(j = 0; j < b->size; j++)
            open_cells->push_back((GETIDX(b, i, j)));

    while (!open_cells->empty()) {
        idx = rand() % open_cells->size();
        cell = open_cells->at(idx);

        if(can_block_cell(b, cell)) {
            b->data[cell] = CLOSED_TILE;
            open_cells->erase(open_cells->begin()+idx);
        }
        
        if(open_cells->size() / (b->size * b->size * 1.0) < 1 - CLOSED_TILE_RATIO)
            return;

        iterations++;
        if(iterations > 1000)
            break;
    }
}

bool can_block_cell(Board* b, int idx) {
    if(surrounds_cell(b, idx))
        return false;
    if(b->size > 7 && is_in_corner(b, idx))
        return false;
    if(violates_min_length(b, idx))
        return false;
    return true;
}

bool is_in_corner(Board* b, int idx) {
    return idx == 0 || 
           idx == b->size - 1 || 
           idx == b->size * (b->size - 1) ||
           idx == b->size * b->size - 1;
}

int num_neighbors(Board* b, int row, int col) {
    int count = 0;

    if(row - 1 >= 0      && GETVAL(b, row - 1, col) == CLOSED_TILE)
        count++;
    if(row + 1 < b->size && GETVAL(b, row + 1, col) == CLOSED_TILE)
        count++;
    if(col - 1 >= 0      && GETVAL(b, row, col - 1) == CLOSED_TILE)
        count++;
    if(col + 1 < b->size && GETVAL(b, row, col + 1) == CLOSED_TILE)
        count++;

    return count;
}

bool surrounds_cell(Board* b, int idx) {
    int row, col;

    row = GETROW(b, idx);
    col = GETCOL(b, idx);
    
    if(num_neighbors(b, row - 1, col) == 2)
        return true;
    if(num_neighbors(b, row + 1, col) == 2)
        return true;
    if(num_neighbors(b, row, col - 1) == 2)
        return true;
    if(num_neighbors(b, row, col + 1) == 2)
        return true;

    return false;
}

bool violates_min_length(Board* b, int idx) {
    int length, row, col;

    row = GETROW(b, idx);
    col = GETCOL(b, idx);

    // check to the left
    length = get_length(b, row, col - 1, true, -1);
    if(length < MIN_LENGTH && length > 0)
        return true;

    // check to the right
    length = get_length(b, row, col + 1, true, 1);
    if(length < MIN_LENGTH && length > 0)
        return true;

    // check upwards
    length = get_length(b, row - 1, col, false, -1);
    if(length < MIN_LENGTH && length > 0)
        return true;

    // check downwards
    length = get_length(b, row + 1, col, false, 1);
    return length < MIN_LENGTH && length > 0;
}