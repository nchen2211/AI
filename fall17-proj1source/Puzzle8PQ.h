#ifndef PUZZLE_8_PQ_H
#define PUZZLE_8_PQ_H

#include <queue>

// We define an element in the priority queue as a pair of state-id (to uniquely identify which state
// the element corresponds to) and an f-value (to be used to order the elements in the priority queue).
struct PQElement {
	PQElement(int id_, double f_) {
		id = id_;
		f = f_;
	}

	int id;
	double f;
};

// Comparator is implemented as a greater than operator, so that the priority queue acts as a min heap.
struct PQElementCompare {
    bool operator()(const PQElement& lhs, const PQElement& rhs) {
        return lhs.f > rhs.f;
    }
};

// Puzzle8PQ is defined as a priority queue that uses PQElement as element and PQElementCompare as comparator.
// See DemoPQ in main.cpp
typedef std::priority_queue<PQElement, std::vector<PQElement>, PQElementCompare > Puzzle8PQ;

#endif
