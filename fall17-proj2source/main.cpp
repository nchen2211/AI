#include "DecisionTree.h"
#include <iostream>

int main(int argc, char* argv[]) {
	DecisionTree tree;
	tree.BuildTree(argv[1]);
	// tree.ParseFile(argv[1]);


	// tree.CalculateGain();
	// tree.BuildingTree();
}
