#ifndef PUZZLE_8_STATE_MANAGER_H
#define PUZZLE_8_STATE_MANAGER_H

#include <unordered_map>
#include "Puzzle8State.h"

// Implements a hash-table for the Puzzle8State class.

class Puzzle8StateManager {
public:
	Puzzle8StateManager () {next_id = 0;}
	~Puzzle8StateManager () {}

	// If puzzle p is already generated, returns its id. Otherwise, assigns the next available integer to it and returns that id.
	int GenerateState(Puzzle8State & p) {
		int key = p.GetKey();
		if (map.find(key) != map.end()) {
			return map[key];
		}
		else {
			map[key] = next_id;
			next_id++;
			return next_id - 1;
		}
	}

	// Check if puzzle p has already been generated (that is, assigned an id).
	bool IsGenerated(Puzzle8State & p) {
		return map.find(p.GetKey()) != map.end();
	}

	// Get the id of puzzle p. If p has not been generated before, returns -1.
	int GetStateId(Puzzle8State & p) {
		int key = p.GetKey();
		if (map.find(key) != map.end()) {
			return map[key];
		}
		return -1;
	}

	// Removes all generated states and resets their ids.
	void Reset() {
		map.clear();
		next_id = 0;
	}

	int GetSize() {
		return map.size();
	}

private:
	std::unordered_map<int,int> map;	// Hash table.
	int next_id;	// Keep track of the next id to assign.
};


#endif
