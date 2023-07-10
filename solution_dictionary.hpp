#include <string>
#include <list>
#include <unordered_map>

#pragma once

using namespace std;

class SolutionDictionary {
    private:
        unordered_map<int, list<string>> word_list;
    public:
        static SolutionDictionary from_csv(string file_path);
        list<string> get_words_by_length(int length);
};