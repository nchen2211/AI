#include "Puzzle8Solver.h"
#include "Puzzle8StateManager.h"
#include "Puzzle8State.h"
#include "Puzzle8PQ.h"
#include <cmath>
#include <algorithm>

// using namespace std;

std::vector<std::vector<char>> mGoal(3, std::vector<char>(3));
std::string mGoalStr = "012345678";

void SetGoal() {
	mGoal[0][0] = '0';
	mGoal[0][1] = '1';
	mGoal[0][2] = '2';
	mGoal[1][0] = '3';
	mGoal[1][1] = '4';
	mGoal[1][2] = '5';
	mGoal[2][0] = '6';
	mGoal[2][1] = '7';
	mGoal[2][2] = '8';
}

std::vector<std::vector<char>> GetGoal() {
	return mGoal;
}

int GetHeuristic(Puzzle8State& state) {
	std::vector<std::vector<char>> tiles = state.GetTiles();
	int manhattan = 0;

	// calculating Manhattan distance
	for(int r = 0; r < 3; r++)
	{
		for(int c = 0; c < 3; c++)
		{
			if(tiles[r][c] != '0')
			{	
				for(int rG = 0; rG < 3; rG++)
				{
					for(int cG = 0; cG < 3; cG++)
					{
						if(tiles[r][c] == mGoal[rG][cG])
						{
							manhattan += abs(r - rG) + abs(c-cG);
						}
					}
				}
			}
		}
	}
	return manhattan;	
}

std::string TilesToString (std::vector<std::vector<char>> tiles) {
	std::string s = "";
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++) {
			s += tiles[r][c];
		}
	return s;
}

std::vector<Puzzle8State> GenerateChildren(Puzzle8State& parentState) {

	std::vector<std::vector<char>> parentTiles = parentState.GetTiles();
	std::vector<Puzzle8State> expandedNodes;

	// get blank space index
	std::pair<int,int> blankIndex = parentState.GetBlankIndex();
	std::vector<std::string> childrenVector;

	// children offset
	int dr[] = {-1,1,0,0};//{1,-1,0,0}; //37
	int dc[] = {0,0,1,-1};//{0,0,1,-1};

	// get children	
	for (int i = 0; i < 4; i++) {

		std::vector<std::vector<char>> childrenTemp = parentTiles;
		int rowChildIndex = blankIndex.first + dr[i]; // get child row
		int colChildIndex = blankIndex.second + dc[i]; // get child col

		if (rowChildIndex >=0 && rowChildIndex <=2 &&  colChildIndex >= 0 && colChildIndex <= 2) {
			if (rowChildIndex != blankIndex.first || colChildIndex != blankIndex.second) {
				std::swap(childrenTemp[blankIndex.first][blankIndex.second], childrenTemp[rowChildIndex][colChildIndex]);	
				childrenVector.push_back(TilesToString(childrenTemp));
			}
		}
	}

	while (! childrenVector.empty()) {
		Puzzle8State state(childrenVector.back());
		state.SetgValue(parentState.GetgValue() + 1);
		expandedNodes.push_back(state);
		childrenVector.pop_back();
	}

	return expandedNodes;
}



void WeightedAStar(std::string puzzle, double w, int & cost, int & expansions) {
	cost = 0;
	expansions = 0;

	SetGoal(); // setting the goal
	Puzzle8StateManager sm;	// Initialize the state manager.
	Puzzle8PQ openList;	// Initialize the priority queue for open list.
	Puzzle8StateManager closed;	// closed list

	// Initialize a vector of state for all generated states. Its index corresponds to state id
	std::vector<Puzzle8State> generatedStates; 
	if (generatedStates.size() == 0) { // process initial state
		Puzzle8State initialState(puzzle);
		// assign ID to this state
		sm.GenerateState(initialState);

		if (puzzle == mGoalStr) {
			closed.GenerateState(initialState);
			std::cout << "the initial state is the goal\n";
			cost = 0;
			expansions = 0;
		} else {
			initialState.SethValue(GetHeuristic(initialState)); // assign h value as its manhattan
			initialState.SetgValue(0); // assign g value
			initialState.SetfValue(w); // set f value

			generatedStates.push_back(initialState); // set initial state to generatedState vector

			// add to open list
			openList.push(PQElement(sm.GetStateId(initialState), initialState.GetfValue()));
		}
	} 

	 // for processing all states generated but the initial state
	while (! openList.empty()) {
		int currentId = openList.top().id;
		openList.pop();

		// if it is not in closed list, add to closed list
		if (! closed.IsGenerated(generatedStates[currentId])) {
			closed.GenerateState(generatedStates[currentId]);
		}
	

		// if it's the goal
		if (generatedStates[currentId].GetLinearizedForm() == mGoalStr) {
			cost = generatedStates[currentId].GetgValue();
			expansions = closed.GetSize();
			return;
		}

		// get expanded nodes
		std::vector<Puzzle8State> expandedNodes = GenerateChildren(generatedStates[currentId]);

		for (Puzzle8State node : expandedNodes) {
			if (! closed.IsGenerated(node)) {
				// if it has been generated
				if (sm.IsGenerated(node)) {

					// if this node g value is less than the its older version
					if (generatedStates[sm.GetStateId(node)].GetgValue() > node.GetgValue()) {
						// update the g value
						generatedStates[sm.GetStateId(node)].SetgValue(node.GetgValue());
						// calculate h and f value 
						node.SethValue(GetHeuristic(node));
						node.SetfValue(w);
						// push it to the open list
						openList.push(PQElement(sm.GetStateId(node),node.GetfValue()));
					}

				} else { // if this node hasn't been generated
					node.SethValue(GetHeuristic(node));
					node.SetfValue(w);
					openList.push(PQElement(sm.GenerateState(node), node.GetfValue()));
					generatedStates.push_back(node);
				}
			}
		}
	}
}