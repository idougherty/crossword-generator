#include "variable.hpp"
#include <cstddef>
#include <cstdio>

Variable::Variable() {}

Variable::Variable(int row, int col, int length, bool is_across) {
    this->length = length;
    this->domain = string(length, OPEN_TILE);
    this->row = row;
    this->col = col;
    this->is_across = is_across;
    this->is_filled = false;
    this->word_num = -1;
}

bool Variable::has_unused_values() {
    for (auto clue : possible_values)
        if(!clue->is_used)
            return true;
    return false;
}

void Variable::set_value(Clue* val) {
    is_filled = true;

    chosen_value = val;
    domain = val->solution;
    pruned_values.push(possible_values);
    possible_values = list<Clue*>{};
}

void Variable::unset_value(string prev_domain) {
    is_filled = false;

    chosen_value = nullptr;
    domain = prev_domain;
    possible_values.splice(possible_values.end(), pruned_values.top());
    pruned_values.pop();
}

bool Variable::prune_values(char c, int idx) {
    list<Clue*> pruned_batch = list<Clue*>();
    list<Clue*>::iterator iter;

    if(is_filled)
        return false;

    iter = possible_values.begin();
    while (iter != possible_values.end()) {
        string val = (*iter)->solution;

        if (val.at(idx) == c) {
            ++iter;
        } else {
            pruned_batch.push_front(*iter);
            iter = possible_values.erase(iter);
        }
    }

    pruned_values.push(pruned_batch);

    return possible_values.size() == 0;
}

void Variable::unprune_values(char c, int idx) {
    if(pruned_values.empty()) {
        printf("Cannot unprune values, no values left to unprune.\n");
        throw exception();
    }

    possible_values.splice(possible_values.end(), pruned_values.top());
    pruned_values.pop();
}

void Variable::print() {
    printf("Variable (%d,%d): length=%d is_across=%d value=%s \n", col, row, length, is_across, domain.data());
}