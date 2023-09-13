#include <chrono>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <emscripten/bind.h>
#include "board.hpp"
#include "solver.hpp"
#include "clue_dictionary.hpp"
#include "board_generator.hpp"

#define CSV_FILE "data/unique-nyt-clues.csv"
#define WORD_COL 1
#define CLUE_COL 2

vector<char> generate_board_with_seed(const int size, const int seed) {
    struct Board* b;

    b = create_board(size);

    srand(seed);
    generate_blocking_tiles(b);

    vector<char> data;
    for(int i = 0; i < size*size; i++)
        data.push_back(b->data[i]);

    free_board(b);

    return data;
}

vector<Solution> solve_given_board(const int size, vector<char> data) {
    ClueDictionary dict;
    vector<Solution> solutions;
    Board* b;
    chrono::high_resolution_clock::time_point t1, t2, t3;

    b = create_board(size);
    b->data = data.data();

    t1 = chrono::high_resolution_clock::now();
    dict = ClueDictionary::from_csv(CSV_FILE, WORD_COL, CLUE_COL);
    dict.shuffle_clues(time(NULL));
    t2 = chrono::high_resolution_clock::now();
    solutions = solve_board(b, dict);
    t3 = chrono::high_resolution_clock::now();

    printf("CLUES:\n");
    for(auto solution : solutions) {
        printf("(%d-%s) %s: %s\n", solution.word_num, solution.is_across ? "across" : "down", solution.value.data(), solution.hint.data());
    }
    printf("Parsed csv in: %lld ms.\n", chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
    printf("Solved board in: %lld ms.\n", chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count());

    return solutions;
}

EMSCRIPTEN_BINDINGS(crosswordgen_module) {
    emscripten::function("generate_board_with_seed", &generate_board_with_seed);
    emscripten::function("solve_given_board", &solve_given_board);

    emscripten::value_object<Solution>("Solution")
        .field("word_num", &Solution::word_num)
        .field("is_across", &Solution::is_across)
        .field("word", &Solution::value)
        .field("hint", &Solution::hint);
        
    emscripten::register_vector<Solution>("vector<Solution>");
    emscripten::register_vector<char>("vector<char>");
}
