#include "DecisionTree.h"
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
	DecisionTree tree(-1);
	tree.StartTree(argv[1]);

	std::pair<double,double> accuracy = tree.GetAccuracyValues();
	std::cout << "------------------------------------\n";
	std::cout << "         Unbounded Tree             \n";
	std::cout << "------------------------------------\n";
	std::cout << " Train data accuracy = " << accuracy.first * 100 << std::endl;
	std::cout << " Test data accuracy = " << accuracy.second * 100 << std::endl;


	std::cout << "\n\n------------------------------------\n";
	std::cout << "         Bounded Tree             \n";
	std::cout << "------------------------------------\n";

	std::cout << std::setw(5) << "depth";
    std::cout << std::setw(10) << "train%";
    std::cout << std::setw(10) <<"valid%\n";

    double maxAccuracy = 0.0;
    int maxDepth = 0;
    for (int i = 1; i <= 15; ++i) {
    	DecisionTree dTree(i);
    	dTree.StartTree(argv[1]);
    	accuracy = dTree.GetAccuracyValues();

    	if (maxAccuracy < accuracy.second) {
    		maxAccuracy = accuracy.second;
    		maxDepth = i;
    	}

    	std::cout << std::setw(5) << i << std::setw(10) << accuracy.first * 100 << std::setw(10) << accuracy.second * 100 << std::endl;
    }

    DecisionTree mostAccurateTree(maxDepth);
    mostAccurateTree.StartTree(argv[1]);
    accuracy = mostAccurateTree.GetAccuracyValues();
    std::cout << "\nMost accurate depth is " << maxDepth << std::endl;
    std::cout << "Accuracy = " << accuracy.second  * 100 << std::endl;
    
}
