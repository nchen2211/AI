#ifndef STATE_8_PUZZLE_H
#define STATE_8_PUZZLE_H

#include <string>
#include <cassert>
#include <iostream>
#include <cstring>
#include <vector>

// Represents an 8-puzzle state as a 3x3 array of chars. Each char can take values in range '0'-'9' (chars, not integers).
// '0' represents the blank tile.
// Provides GetKey to uniquely represent the puzzle as an integer (for hashing).
// You can extend this class to generate successors and get heuristic distance to '012345678', which is always the goal state.

class Puzzle8State {
public:
	Puzzle8State(std::string s = "012345678") : mTiles(3,std::vector<char>(3)){
		closedFlag = false; // it's not in closed list yet

		assert(s.length() == 9);
		for (int r = 0; r < 3; r++)
			for (int c = 0; c < 3; c++) {
				mTiles[r][c] = s[r*3 + c];

				if (mTiles[r][c] == '0') { // getting blank space index
					mBlankIndex.first = r;
					mBlankIndex.second = c;
				}
			}
	}

	// Key generated as an integer for the hash function in Puzzle8StateManager.
	int GetKey() {
		int key = 0;
		for (int r = 0; r < 3; r++)
			for (int c = 0; c < 3; c++) {
				key = key*10 + int(mTiles[r][c] - '0');
			}
		return key;
	}

	// Return the linearized form as a string. (You don't need to use this.)
	std::string GetLinearizedForm () {
		std::string s = "";
		for (int r = 0; r < 3; r++)
			for (int c = 0; c < 3; c++) {
				s += mTiles[r][c];
			}
		return s;
	}

	// Print the puzzle in a 3x3 layout. (You don't need to use this.)
	void Print(std::ostream & out = std::cout) {
		for (int r = 0; r < 3; r++) {
			for (int c = 0; c < 3; c++) {
				out<<mTiles[r][c]<<" ";
			}
			out<<std::endl;
		}
		out<<GetKey()<<std::endl;
	}

	void SetfValue(double w) {
		f = double(g + (w * h));
	}

	void SethValue(int hVal) {
		h = hVal;
	}

	void SetgValue(int gVal) {
		g = gVal;
	}

	double GetfValue() {
		return f;
	}

	int GetgValue() {
		return g;
	}

	int GethValue() {
		return h;
	}

	void SetClosedFlag() {
		closedFlag = true;
	}

	bool IsClosed() {
		return closedFlag;
	}

	std::vector<std::vector<char>> GetTiles() const {
		return mTiles;
	}

	std::pair<int,int> GetBlankIndex() const {
		return mBlankIndex;
	}

private:

	// tiles[r][c] is the tile (or blank) at row r (0-2) and column c (0-2)
	// 0th row is the top row, and 0th column is the leftmost column.
	// char tiles[3][3];
	std::vector<std::vector<char>> mTiles;
	std::pair<int,int> mBlankIndex;
	double f;
	int g;
	int h;
	bool closedFlag;
	std::string goalString = "012345678";
};

#endif