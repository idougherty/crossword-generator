#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iterator>
#include <utility>
#include <vector>
#include <iostream>
#include <fstream>
#include "board.hpp"
#include "solution_dictionary.hpp"
#include "solver.hpp"

Variable* get_variable(vector<Variable*> vars, Board* b, int row, int col, bool is_across);
void fill_board(vector<Variable*> vars, struct Board* b, bool is_across = true);

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
            pruned_values.push_front(*iter);
            iter = possible_values.erase(iter);
        }
    }

    return possible_values.size() == 0;
}

// should re-add some range to start of possible vals
void Variable::unprune_values(char c, int idx) {
    list<string>::iterator iter;
    
    // printf("unpruning @ (%d, %d)!, taking back all words that can fit in %s\n", col, row, value.c_str());

    // printf("before: cross->possible_vals:\n");
    // for(auto w : possible_values)
    //     printf("\t%s\n", w.c_str());

    // printf("before: cross->pruned_vals:\n");
    // for(auto w : pruned_values)
    //     printf("\t%s\n", w.c_str());

    // re-add all matches, can be more efficient
    iter = pruned_values.begin();
    while (iter != pruned_values.end()) {
        bool match = true;
        string word = *iter;
        for(int i = 0; i < length; i++) {
            if(value[i] != OPEN_TILE && value[i] != (*iter)[i]) {
                match = false;
                break;
            }
        }
        if(!match) break;
        possible_values.push_front(*iter);
        iter = pruned_values.erase(iter);
    }

    // printf("after: cross->possible_vals:\n");
    // for(auto w : possible_values)
    //     printf("\t%s\n", w.c_str());

    // printf("after: cross->pruned_vals:\n");
    // for(auto w : pruned_values)
    //     printf("\t%s\n", w.c_str());
}

vector<Variable*> create_variables(struct Board* b, SolutionDictionary dict) {
    int row, col, length;
    Variable* var, *across, *down;
    vector<Variable*> vars = vector<Variable*>();

    for(row = 0; row < b->size; row++) {
        for(col = 0; col < b->size; col++) {
            if(GETVAL(b, row, col) == CLOSED_TILE)
                continue;
            if(row == 0 || GETVAL(b, row - 1, col) == CLOSED_TILE) {
                length = get_length(b, row, col, false, 1);
                var = new Variable(row, col, length, false);
                var->possible_values = list<string>(dict.get_words_by_length(length));
                var->constraints = vector<pair<Variable*, int>>(length);
                vars.push_back(var);
            }
            if(col == 0 || GETVAL(b, row, col - 1) == CLOSED_TILE) {
                length = get_length(b, row, col, true, 1);
                var = new Variable(row, col, length, true);
                var->possible_values = list<string>(dict.get_words_by_length(length));
                var->constraints = vector<pair<Variable*, int>>(length);
                vars.push_back(var);
            }
        }
    }

    for(row = 0; row < b->size; row++) {
        for(col = 0; col < b->size; col++) {
            if(GETVAL(b, row, col) == CLOSED_TILE) 
                continue;
            across = get_variable(vars, b, row, col, true);
            down = get_variable(vars, b, row, col, false);

            across->constraints.at(col - across->col) = {down, row - down->row};
            down->constraints.at(row - down->row) = {across, col - across->col};
        }
    }

    return vars;
}

Variable* get_variable(vector<Variable*> vars, Board* b, int row, int col, bool is_across) {
    if(is_across) {
        while(col - 1 >= 0 && GETVAL(b, row, col - 1) != CLOSED_TILE)
            col--;
    } else {
        while(row - 1 >= 0 && GETVAL(b, row - 1, col) != CLOSED_TILE)
            row--;
    }

    for(auto var : vars)
        if(var->col == col && var->row == row && var->is_across == is_across)
            return var;

    throw exception();
}

void Variable::print() {
    printf("Variable (%d,%d): length=%lu is_across=%d value=%s \n", col, row, value.size(), is_across, value.data());
}

bool recursive_solve(vector<Variable*> vars, Board* b) {
    int i, num_solved = 0;
    bool backtrack;
    string chosen_val, prev_value;
    list<string> attempted_values = list<string>();
    vector<Variable*>::iterator iter;
    Variable* var;

    // printf("one level deeper\n");

    for(iter = vars.begin(); iter != vars.end(); iter++) {
        var = *iter;
        if(!var->is_filled)
            break;
        num_solved++;
    }

    // puzzle solved!
    if(iter == vars.end())
        return true;

    while(var->possible_values.size() > 0) {
        backtrack = false;

        chosen_val = var->possible_values.front();
        var->possible_values.erase(var->possible_values.begin());

        // we know we have collapsed this variable to a single value
        prev_value = var->value;
        var->value = chosen_val;
        var->is_filled = true;
        var->pruned_values = var->possible_values;
        var->possible_values = list<string>{};
        attempted_values.push_front(chosen_val);

        // printf("trying to place word '%s'\n", chosen_val.c_str());
        for(i = 0; i < var->value.length(); i++) {
            auto [cross, pos] = var->constraints[i];
            if(cross->prune_values(chosen_val[i], pos)) {
                // printf("word '%s' failed at %d\n", chosen_val.c_str(), i);

                backtrack = true;
                break;
            }
        }

        // if(!backtrack) {
        //     printf("placing word '%s'\n", chosen_val.c_str());
        //     fill_board(vars, b);
        //     print_board(b);
        // }

        if(!backtrack && recursive_solve(vars, b))
            return true;

        var->value = prev_value;
        var->is_filled = false;
        var->possible_values.splice(var->possible_values.end(), var->pruned_values);
        for(i = (i >= var->length) ? var->length-1 : i; i >= 0; i--) {
            auto [cross, pos] = var->constraints[i];
            cross->value[pos] = prev_value[i];
            cross->unprune_values(chosen_val[i], pos);
        }

        // printf("backtracking word '%s', previously '%s'\n", chosen_val.c_str(), prev_value.c_str());
        // fill_board(vars, b);
        // print_board(b);
    }

    var->possible_values.splice(var->possible_values.end(), attempted_values);
    return false;
}

void solve_board(struct Board* b, SolutionDictionary dict) {
    vector<Variable*> vars = create_variables(b, dict);

    bool success = recursive_solve(vars, b);
    
    if(success) {
        printf("SOLUTION:\n");
        fill_board(vars, b);
        print_board(b);
    } else {
        printf("Couldn't solve board. :(");
    }
}

void fill_board(vector<Variable*> vars, struct Board* b, bool is_across) {
    int row, col;
    for(auto var : vars) {
        if(var->is_across != is_across)
            continue;
        row = var->row;
        col = var->col;
        for(char c : var->value) {
            b->data[row * b->size + col] = c;
            if(is_across)
                col++;
            else
                row++;
        }
    }
}