#include "clue_dictionary.hpp"
#include <cstddef>
#include <exception>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <random>

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
    string::size_type start, end;

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

ClueDictionary ClueDictionary::from_csv(string file_path, int solution_col, int hint_col) {
    auto dict = ClueDictionary();
    auto file =  ifstream(file_path);
    string line;
    vector<string> split_line;
    Clue* clue;
    int len;

    if(!file.good())
        throw exception();

    getline (file, line);
    
    while (getline (file, line)) {
        split_line = split_row(line, ',');

        if(split_line.size() < solution_col || split_line.size() < hint_col) {
            printf("Could not parse line: %s\n\n", line.data());
            continue;
        }

        clue = new Clue {
            false,
            split_line[solution_col],
            split_line[hint_col]
        };
        
        len = clue->solution.length();

        if(len == 0)
            continue;

        if(dict.clue_map.find(len) == dict.clue_map.end()) {
            dict.clue_map.insert({len, list<Clue*> { clue }});
        } else {
            dict.clue_map.at(len).push_back(clue);
        }
    }

    file.close();

    return dict;
}

void ClueDictionary::shuffle_clues(int seed) {
    vector<Clue*> tmp;

    for(auto entry : clue_map) {
        tmp = vector<Clue*>(entry.second.begin(), entry.second.end());
        shuffle(tmp.begin(), tmp.end(), default_random_engine(seed));
        copy(tmp.begin(), tmp.end(), clue_map.find(entry.first)->second.begin());
    }
}

list<Clue*> ClueDictionary::get_clues_by_length(int length) {
    if(clue_map.find(length) == clue_map.end())
        return list<Clue*>();
    return clue_map.at(length);
}