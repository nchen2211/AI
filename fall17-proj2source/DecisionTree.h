#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

struct Example {
	char mClassification;
	std::vector<char> mAttrValues;
	int mExampleIndex;
};

struct Attribute {
	std::string mDescription; // attribute description
	std::unordered_map<char, std::vector<Example>> mValues; // map of attr values and examples that fall into its value's category
	int mTotalExample; // total examples that fall into this attr category. For calculating gain
	double mGain; // information gain of this attribute
	bool mIsExpanded; // set to true if it is selected to be a node
};

struct Node {
	
	std::string mNodeDescription; // attribute description or 0 or 1
	bool mIsLeaf; // flag of leaf node
	std::unordered_map<char, Node*> mChildrenMap; // attr value as edge, Node as vertex
	Node* parent;

	Node (std::string desc) {
		mNodeDescription = desc;
	}
};

class DecisionTree {

private:
	std::vector<Attribute> mAttributes;
	std::vector<Example>mTrainingSet;
	std::unordered_map<int, Example> mTrainingMap;
	std::unordered_map<int, Example> mValidationMap;
	std::unordered_map<int, Example> mTestMap;

public:
	DecisionTree();
	void ParseFile(std::string filename);
	void AssignSets(std::vector<Example>& allDatasets);
	void CalculateGain(std::vector<Example>& exampleSet);
	void BuildTree(std::string filename);
	Node* GenerateRoot();
	Node* GenerateNode(std::vector<Example>& exampleSet, Node* parentNode, bool isLeaf);
	void GenerateSuccessor(Node* parentNode, Attribute attr);
};

#endif