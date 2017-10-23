#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>

#include "Puzzle8State.h"
#include "Puzzle8StateManager.h"
#include "Puzzle8Solver.h"
#include "Puzzle8PQ.h"
#include "CPUTimer.h"

using namespace std;

void DemoStateManager () {
	cout<<"State Manager Demo: "<<std::endl;

	// Linearized representations of states to be used in the demo (only the blank tile changes location).
	vector<string> states = {
			"012345678",
			"102345678",
			"120345678",
			"123045678",
			"012345678"	// Same as the first one.
	};

	Puzzle8StateManager sm;	// Initialize the state manager.

	for (int i = 0; i < states.size(); i++) {
		Puzzle8State s(states[i]);	// Create a state from its string representation.

		if (sm.IsGenerated(s)) { // You can check if a state has already been generated before ...
			// ... and get its ids by using the function GetStateId.
			cout<<"State "<<s.GetLinearizedForm()<<" has already been generated with id "<<sm.GetStateId(s)<<endl;
		}
		else {

			// GenerateState only generates a state if it has not been generated before.
			// It returns the id of the state (if it already exists, returns previous id, otherwise, generates a new id).
			cout<<"State "<<s.GetLinearizedForm()<<" is generated with id "<<sm.GenerateState(s)<<endl;
		}
	}
	cout<<"----------"<<endl;
}

void DemoPriorityQueue () {
	cout<<"Priority Queue Demo: "<<std::endl;

	// State ids and their f-values to be used in the demo.
	vector<int> ids  	   = {   1,   2,   3,   4,   1};
	vector<double> f_vals = {   7,   3,   1, 2.5,   4};

	Puzzle8PQ pq;	// Initialize the priority queue.

	for (int i = 0; i < ids.size(); i++) {
		pq.push(PQElement(ids[i], f_vals[i])); // Create a PQElement and add it to the queue.
		cout<<"Added state "<<ids[i]<<" to the priority queue with f-value "<<f_vals[i]<<endl;
	}

	while (!pq.empty()) {	// While the priority queue is not empty
		PQElement next = pq.top(); // The element with the minimum f-val.
		cout<<"Next state to expand is "<<next.id<<" with f-value "<<next.f<<endl;
		pq.pop();	// Remove the top element from the priority queue.
	}

	cout<<"Notice that state 1 was added to and removed from the priority queue twice. Be careful not to expand it twice!"<<endl;
	cout<<"----------"<<endl;
}

void Demo () {
	DemoStateManager();
	DemoPriorityQueue();
}

// Calls the WeightedAStar function, times it, and prints statistics.
void Solve8Puzzle(string puzzle, double w) {
	int cost = 0;
	int expansions = 0;
	CPUTimer t;

	t.StartTimer();
	WeightedAStar(puzzle, w, cost, expansions);
	t.EndTimer();

	cout<<"\nCost: "<<cost<<endl;
	cout<<"Expansions: "<<expansions<<endl;
	cout<<"Time: "<<t.GetElapsedTime()*1000<<" ms."<<endl;
}

// Runs experiments required for the theoretical part, and prints a table.
void CreateTable(int start_id, int num_instances = 50, ostream & out = cout) {
	vector<double> weights = {0, 0.25, 0.5, 0.75, 1, 1.5, 2, 3, 5, 10};

	// Read in the relevant instances from the 'instances' file.
	vector<string> instances;
	ifstream in;
	in.open("instances");
	int instance_id;
	string instance;
	while (in >> instance_id) {
		in >> instance;
		if (instance_id < start_id)	// Loop around.
			instance_id += 1000;
		if (start_id <= instance_id && instance_id < start_id + num_instances)
			instances.push_back(instance);
	}
	assert(instances.size() == num_instances);

	// Create the header of the table.
	out<<right<<setprecision(2)<<fixed;

	out<<std::setw(6)<<"w";
	out<<std::setw(10)<<"Cost";
	out<<std::setw(12)<<"Time(ms)";
	out<<std::setw(12)<<"Expansions";
	out<<endl;

	// Create the table row by row.
	for (int i = 0; i < weights.size(); i++) {
		double w = weights[i];

		int total_cost = 0;
		double total_time = 0;
		int total_expansions = 0;

		// Perform experiments for the given weight.
		for (int j = 0; j < instances.size(); j++) {
			int cost = 0;
			int expansions = 0;
			CPUTimer t;

			t.StartTimer();
			WeightedAStar(instances[j], w, cost, expansions);
			t.EndTimer();

			total_cost += cost;
			total_time += t.GetElapsedTime();
			total_expansions += expansions;
		}

		// Report averaged results.
		out<<std::setw(6)<<w;
		out<<std::setw(10)<<total_cost/(double)num_instances;
		out<<std::setw(12)<<1000*total_time/num_instances;
		out<<std::setw(12)<<total_expansions/(double)num_instances<<endl;
	}
}

int main(int argc, char *argv[]) {

	// No arguments: demo.
	if (argc == 1)
		Demo();

	// Puzzle and weight: Weighted A*.
	else if (argc == 3) {
		double w;
		sscanf(argv[2],"%lf",&w);
		Solve8Puzzle(argv[1], w);
	}

	// Last 3 digits of student id as argument: Run experiments and generate table.
	else if (argc == 2) {
		int id;
		sscanf(argv[1], "%d", &id);
		CreateTable(id%1000);
	}

	return 0;
}
