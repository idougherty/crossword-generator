#include "solution_dictionary.hpp"
#include <cstddef>
#include <exception>
#include <fstream>
#include <sstream>
#include <vector>

vector<string> split(string s, char delimiter) {
    size_t pos_start = 0, pos_end;
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + 1;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

vector<string> split_row(string line, char delimiter) {
    const char single_quote = '\"';
    const string escaped_quote = "\"\"";

    std::string column;
    stringstream stream(line);
    vector<string> values;
    bool hasQuote = false;
    string part = "";
    int start, end;

    // TODO: make this not so ugly
    while (getline(stream, column, ',')) {
        if (hasQuote)
            part += ',';

        start = 0;
        if (!hasQuote && !column.empty() && column.front() == single_quote) {
            hasQuote = true;
            start = 1;
            part = "";
        }
        
        if (hasQuote) {
            while ((end = column.find(escaped_quote, start)) != string::npos) {
                part += column.substr(start, end - start) + single_quote;
                start = end + escaped_quote.length();
            }
        } else {
            values.push_back(column);
            continue;
        }

        if (hasQuote && (end = column.find(single_quote, start)) != string::npos) {
            hasQuote = false;
            part += column.substr(start, (column.length() - start) - 1);
            values.push_back(part);
        } else {
            part += column.substr(start);
        }
    }
    
    return values;
}

SolutionDictionary SolutionDictionary::from_csv(string file_path, int word_col, int clue_col) {
    auto dict = SolutionDictionary();
    auto file =  ifstream(file_path);
    string line;
    vector<string> split_line;
    Solution* solution;
    int len;

    if(!file.good())
        throw new exception();

    getline (file, line);
    
    while (getline (file, line)) {
        split_line = split_row(line, ',');

        if(split_line.size() < word_col || split_line.size() < clue_col) {
            printf("Could not parse line: %s\n\n", line.data());
            continue;
        }

        solution = new Solution {
            split_line[word_col],
            split_line[clue_col]
        };
        
        len = solution->word.length();

        if(dict.word_list.find(len) == dict.word_list.end()) {
            dict.word_list.insert({len, list<Solution*> { solution }});
        } else {
            dict.word_list.at(len).push_back(solution);
        }
    }

    file.close();

    return dict;
}

list<Solution*> SolutionDictionary::get_solutions_by_length(int length) {
    if(word_list.find(length) == word_list.end())
        return list<Solution*>();
    return word_list.at(length);
}