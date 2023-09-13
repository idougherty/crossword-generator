#include <cstdint>
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
#include "clue_dictionary.hpp"
#include "solver.hpp"

Variable* get_variable(vector<Variable*> vars, Board* b, int row, int col, bool is_across);

vector<Variable*> create_variables(struct Board* b, ClueDictionary dict) {
    int row, col, length, word_num = 0;
    bool valid_across, valid_down;
    Variable* var, *across, *down;
    vector<Variable*> vars = vector<Variable*>();

    // initialize variables
    for(row = 0; row < b->size; row++) {
        for(col = 0; col < b->size; col++) {
            valid_across = col == 0 || GETVAL(b, row, col - 1) == CLOSED_TILE;
            valid_down = row == 0 || GETVAL(b, row - 1, col) == CLOSED_TILE;

            if(GETVAL(b, row, col) == CLOSED_TILE || (!valid_across && !valid_down))
                continue;
            
            word_num++;

            if(valid_down) {
                length = get_length(b, row, col, false, 1);
                var = new Variable(row, col, length, false);
                var->possible_values = list<Clue*>( dict.get_clues_by_length(length) );
                var->constraints = vector<pair<Variable*, int>>(length);
                var->word_num = word_num;
                vars.push_back(var);
            }

            if(valid_across) {
                length = get_length(b, row, col, true, 1);
                var = new Variable(row, col, length, true);
                var->possible_values = list<Clue*>( dict.get_clues_by_length(length) );
                var->constraints = vector<pair<Variable*, int>>(length);
                var->word_num = word_num;
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
    Variable* best = nullptr;

    for(iter = vars.begin(); iter != vars.end(); iter++) {
        if((*iter)->is_filled)
            continue;
        
        if(best == nullptr || (*iter)->possible_values.size() < best->possible_values.size())
            best = *iter;
    }

    return best;
}

Clue* choose_value(Variable* var) {
    int i, attempts = 0;
    uint64_t score, best_score = 0;
    list<Clue*>::iterator iter, best = var->possible_values.end();
    Clue* ret;

    iter = var->possible_values.begin();
    while(iter != var->possible_values.end() && attempts < LOOKAHEAD) {

        if((*iter)->is_used) {
            iter = next(iter);
            continue;
        }
        
        score = 1;

        for(i = 0; i < var->length; i++) {
            auto [cross, pos] = var->constraints[i];
            if(cross->is_filled)
                continue;

            cross->prune_values((*iter)->solution.at(i), pos);
            score *= cross->possible_values.size();
            cross->unprune_values((*iter)->solution.at(i), pos);
        }

        if(score >= best_score) {
            best_score = score;
            best = iter;
        }

        iter = next(iter);
        attempts++;
    }

    if(best == var->possible_values.end())
        return nullptr;

    ret = *best;
    var->possible_values.erase(best);

    return ret;
}

bool blocks_unique_vals(vector<Variable*> vars, Variable* curr_var) {
    // check if choosing this value will cause another variable to have all used possible vals 
    for(auto var : vars)
        if(var != curr_var && !var->is_filled && var->possible_values.size() <= vars.size() && !var->has_unused_values())
            return true;
    return false;
}

bool recursive_solve(vector<Variable*> vars, Board* b) {
    int i;
    bool backtrack;
    Clue* chosen_val;
    string prev_domain;
    list<Clue*> attempted_values = list<Clue*>();
    Variable* var;

    var = choose_variable(vars);

    // no variables left to solve for, solution found!
    if(var == nullptr)
        return true;

    while(var->possible_values.size() > 0) {
        backtrack = false;

        chosen_val = choose_value(var);

        // no valid values
        if(chosen_val == nullptr)
            break;

        attempted_values.push_front(chosen_val);

        // this variable has been collapsed to a single value
        chosen_val->is_used = true;
        prev_domain = var->domain;
        var->set_value(chosen_val);

        for(i = 0; i < var->domain.length(); i++) {
            auto [cross, pos] = var->constraints[i];

            cross->domain[pos] = chosen_val->solution[i];
            if(cross->prune_values(chosen_val->solution[i], pos)) {
                backtrack = true;
                break;
            }
        }

        if(!backtrack && blocks_unique_vals(vars, var))
            backtrack = true;
        
        if(!backtrack && recursive_solve(vars, b))
            return true;

        chosen_val->is_used = false;
        var->unset_value(prev_domain);

        for(i = (i >= var->length) ? var->length-1 : i; i >= 0; i--) {
            auto [cross, pos] = var->constraints[i];

            if(cross->is_filled)
                continue;

            cross->domain[pos] = prev_domain[i];
            cross->unprune_values(chosen_val->solution[i], pos);
        }
    }

    var->possible_values.splice(var->possible_values.end(), attempted_values);
    return false;
}

vector<Solution> solve_board(struct Board* b, ClueDictionary dict) {
    vector<Solution> solutions; 
    vector<Variable*> vars = create_variables(b, dict);

    bool success = recursive_solve(vars, b);

    if(!success)
        return vector<Solution> {};

    for(auto var : vars) {
        solutions.push_back(
            Solution {
                var->row,
                var->col,
                var->is_across,
                var->word_num,
                var->chosen_value->solution,
                var->chosen_value->hint
            }
        );
    }

    return solutions;
}