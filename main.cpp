#include <cstdio>
#include <iostream>
#include "board.hpp"
#include "board_generator.hpp"
#include "solver.hpp"
#include "solution_dictionary.hpp"
#include "defs.h"

int main(int argc, char** args) {
    int size = 7;
    string solution_file;
    SolutionDictionary dict;
    struct Board* b;

    if(argc < 2) {
        printf("Please provide a file with clues and solutions.\n");
        return -1;
    }

    dict = SolutionDictionary::from_csv(args[1], 1, 2);

    printf("Generating a board of size %d...\n", size);
    b = create_board(size);

    srand(time(NULL));
    generate_blocking_tiles(b);

    printf("\n%dx%d Puzzle Board: \n", b->size, b->size);
    print_board(b);

    solve_board(b, dict);

    free_board(b);

    return 0;
}

/*
TODO:
- add solution clues
- do not reuse words in the same puzzle
- figure out how solution dictionary reordering will work
- package up the following functionalities as stateless wasm modules
    - generate crossword & solution given a some grid size
        - should retry until valid answer is found in a reasonable amount of time
    - generate solution given a crossword grid
        - should have failure states
*/