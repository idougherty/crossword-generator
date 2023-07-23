#include "variable.hpp"
#include <cstdio>

Variable::Variable() {}

Variable::Variable(int row, int col, int length, bool is_across) {
    this->length = length;
    this->value = string(length, OPEN_TILE);
    this->row = row;
    this->col = col;
    this->is_across = is_across;
    this->is_filled = false;
}

bool Variable::prune_values(char c, int idx) {
    list<string> pruned_batch = list<string>();
    list<string>::iterator iter;

    if(is_filled)
        return false;

    value[idx] = c;

    iter = possible_values.begin();
    while (iter != possible_values.end()) {
        string val = *iter;

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

// should re-add some range to start of possible vals
void Variable::unprune_values(char c, int idx) {
    if(is_filled)
        return;

    possible_values.splice(possible_values.end(), pruned_values.top());
    pruned_values.pop();

    // list<string>::iterator iter;

    // // re-add all matches, can be more efficient
    // iter = pruned_values.begin();
    // while (iter != pruned_values.end()) {
    //     bool match = true;
    //     string word = *iter;
    //     for(int i = 0; i < length; i++) {
    //         if(value[i] != OPEN_TILE && value[i] != (*iter)[i]) {
    //             match = false;
    //             break;
    //         }
    //     }
    //     if(!match) break;
    //     possible_values.push_front(*iter);
    //     iter = pruned_values.erase(iter);
    // }
}

void Variable::print() {
    printf("Variable (%d,%d): length=%lu is_across=%d value=%s \n", col, row, value.size(), is_across, value.data());
}