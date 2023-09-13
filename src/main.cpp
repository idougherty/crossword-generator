#include <cstdio>
#include <iostream>
#include <vector>
#include "board.hpp"
#include "board_generator.hpp"
#include "solver.hpp"
#include "clue_dictionary.hpp"
#include "defs.h"

int main(int argc, char** args) {
    int size, seed;
    string solution_file;
    ClueDictionary dict;
    vector<Solution> solutions;
    struct Board* b;

    if(argc < 2) {
        printf("Please provide a crossword size.\n");
        return -1;
    }

    size = stoi(args[1]);

    if(argc < 3) {
        printf("Please provide a file with clues and solutions.\n");
        return -1;
    }

    seed = argc < 4 ? time(NULL) : stoi(args[3]);

    printf("Generating a board of size %d using seed %d...\n", size, seed);
    b = create_board(size);

    srand(seed);
    generate_blocking_tiles(b);

    printf("\n%dx%d Puzzle Board: \n", b->size, b->size);
    print_board(b);

    dict = ClueDictionary::from_csv(args[2], 1, 2);
    dict.shuffle_clues(seed);

    solutions = solve_board(b, dict);

    if(solutions.empty()) {
        printf("Could not solve this board. :(\n");
    } else {
        printf("SOLUTION:\n");
        fill_board(solutions, b);
        print_board(b);

        printf("CLUES:\n");
        for(auto solution : solutions) {
            printf("(%d-%s) %s: %s\n", solution.word_num, solution.is_across ? "across" : "down", solution.value.data(), solution.hint.data());
        }
    }

    free_board(b);

    return 0;
}