#include "solution_dictionary.hpp"
#include <cstddef>
#include <exception>
#include <fstream>

SolutionDictionary SolutionDictionary::from_csv(string file_path) {
    auto dict = SolutionDictionary();
    auto file =  ifstream(file_path);
    string word;
    int len;

    if(!file.good())
        throw new exception();

    while (getline (file, word)) {
        len = word.length();

        if(dict.word_list.find(len) == dict.word_list.end()) {
            dict.word_list.insert({len, list<string> { word }});
        } else {
            if(dict.word_list.at(len).size() < 5000)
                dict.word_list.at(len).push_back(word);
        }
    }

    file.close();

    return dict;
}

list<string> SolutionDictionary::get_words_by_length(int length) {
    if(word_list.find(length) == word_list.end())
        return list<string>();
    return word_list.at(length);
}