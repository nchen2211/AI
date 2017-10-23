#ifndef PUZZLE_8_SOLVER_H
#define PUZZLE_8_SOLVER_H

#include <string>
#include <iostream>
#include <vector>


int GetHeuristic(std::string& puzzle); 
void WeightedAStar(std::string puzzle, double w, int & cost, int & expansions);
void SetGoal();
std::vector<std::vector<char>> GetGoal();

#endif
