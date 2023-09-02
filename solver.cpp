#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <fstream>
#include "board.hpp"
#include "solution_dictionary.hpp"
#include "solver.hpp"
#include "variable.hpp"

Variable* get_variable(vector<Variable*> vars, Board* b, int row, int col, bool is_across);
void fill_board(vector<Variable*> vars, struct Board* b, bool is_across = true);

vector<Variable*> create_variables(struct Board* b, SolutionDictionary dict) {
    int row, col, length;
    Variable* var, *across, *down;
    vector<Variable*> vars = vector<Variable*>();

    // initialize variables
    for(row = 0; row < b->size; row++) {
        for(col = 0; col < b->size; col++) {
            if(GETVAL(b, row, col) == CLOSED_TILE)
                continue;
            if(row == 0 || GETVAL(b, row - 1, col) == CLOSED_TILE) {
                length = get_length(b, row, col, false, 1);
                var = new Variable(row, col, length, false);
                var->possible_values = dict.get_solutions_by_length(length);
                var->constraints = vector<pair<Variable*, int>>(length);
                vars.push_back(var);
            }
            if(col == 0 || GETVAL(b, row, col - 1) == CLOSED_TILE) {
                length = get_length(b, row, col, true, 1);
                var = new Variable(row, col, length, true);
                var->possible_values = dict.get_solutions_by_length(length);
                var->constraints = vector<pair<Variable*, int>>(length);
                vars.push_back(var);
            }
        }
    }

    // set constraints
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

Variable* choose_variable(vector<Variable*> vars) {
    vector<Variable*>::iterator iter;
    Variable* best = NULL;

    for(iter = vars.begin(); iter != vars.end(); iter++) {
        if((*iter)->is_filled)
            continue;
        
        if(best == NULL || (*iter)->possible_values.size() < best->possible_values.size())
            best = *iter;
    }

    return best;
}

Solution choose_value(Variable* var) {
    int i, j, score, best_score;
    list<Solution>::iterator iter, best;

    iter = var->possible_values.begin();
    best = iter;
    while(iter != var->possible_values.begin()) {
        iter = next(iter);
        if(iter == var->possible_values.end())
            break;
        
        score = 1;

        for(j = 0; j < var->length; j++) {
            auto [cross, pos] = var->constraints[i];
            if(cross->is_filled)
                continue;

            cross->prune_values((*iter).word.at(j), pos);
            score *= cross->possible_values.size();
            cross->unprune_values((*iter).word.at(j), pos);
        }

        if(score > best_score) {
            best_score = score;
            best = iter;
        }
    }

    var->possible_values.erase(iter);

    return *iter;
}

int attempts = 0;

bool recursive_solve(vector<Variable*> vars, Board* b) {
    int i;
    bool backtrack;
    Solution chosen_val;
    string prev_value;
    list<Solution> attempted_values = list<Solution>();
    Variable* var;

    var = choose_variable(vars);

    // no variables left to solve for, solution found!
    if(var == NULL)
        return true;

    while(var->possible_values.size() > 0) {
        backtrack = false;

        attempts++;
        if(attempts % 10000 == 0) {
            fill_board(vars, b);
            print_board(b);
        }

        chosen_val = choose_value(var);
        attempted_values.push_front(chosen_val);

        // we know we have collapsed this variable to a single value
        prev_value = var->value;
        var->value = chosen_val.word;
        var->solution = chosen_val;
        var->is_filled = true;
        var->pruned_values.push(var->possible_values);
        var->possible_values = list<Solution>{};

        for(i = 0; i < var->value.length(); i++) {
            auto [cross, pos] = var->constraints[i];
            if(cross->prune_values(chosen_val.word[i], pos)) {
                backtrack = true;
                break;
            }
        }

        if(!backtrack && recursive_solve(vars, b))
            return true;

        var->value = prev_value;
        var->is_filled = false;
        var->possible_values.splice(var->possible_values.end(), var->pruned_values.top());
        var->pruned_values.pop();
        for(i = (i >= var->length) ? var->length-1 : i; i >= 0; i--) {
            auto [cross, pos] = var->constraints[i];
            cross->value[pos] = prev_value[i];
            cross->unprune_values(chosen_val.word[i], pos);
        }
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

        printf("CLUES:\n");
        for(auto var : vars) {
            printf("%s: %s\n", var->solution.word.data(), var->solution.clue.data());
        }
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