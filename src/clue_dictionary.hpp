#include <string>
#include <list>
#include <unordered_map>

#pragma once

using namespace std;

struct Clue {
    bool is_used;
    string solution;
    string hint;
};

class ClueDictionary {
    private:
        unordered_map<int, list<Clue*>> clue_map;
    public:
        static ClueDictionary from_csv(string file_path, int solution_col, int hint_col);
        list<Clue*> get_clues_by_length(int length);
        void shuffle_clues(int seed);
};