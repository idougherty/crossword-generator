#include <string>
#include <list>
#include <unordered_map>

#pragma once

using namespace std;

struct Solution {
    string word;
    string clue;
};

class SolutionDictionary {
    private:
        unordered_map<int, list<Solution>> word_list;
    public:
        static SolutionDictionary from_csv(string file_path, int word_col, int clue_col);
        list<Solution> get_solutions_by_length(int length);
};