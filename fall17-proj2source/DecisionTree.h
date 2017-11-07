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

struct TreeAttribute {
	std::string mDescription;
	std::vector<char> mAttributeLabels;
	bool mIsExpanded;
	int attrIndex;
};

struct Node {
	std::string mNodeAttribute; // attribute description
	char mNodeValue; // store 0, 1, or plurality value
	bool mIsLeaf; // flag of leaf node
	std::vector<Node*> mNodeChildren;
	std::vector<char> mNodeLabels;
	int mNodeDepth = 1;
	Node* mNodeParent;
};

class DecisionTree {

private:
	std::vector<TreeAttribute> mTreeAttributes;
	std::vector<Example>mTrainingSet;
	std::vector<Example>mValidationSet;
	std::vector<Example>mTestSet;
	std::unordered_map<std::string, int> mAttributeMap;
	int mMaxDepth;
	std::pair<double, double> mAccuracyValues;
	Node *mRoot;

public:
	DecisionTree(int depth);
	~DecisionTree();
	void StartTree(std::string filename);
	std::vector<Example> ParseFile(std::string filename);
	void AssignSets(std::vector<Example>& allDatasets, double percentage);
	Node* BuildTree(Node* currNode, std::vector<Example>& currExamples, std::vector<Example>& parentExamples, std::vector<TreeAttribute> currTreeAttributes);
	std::pair<std::string, std::vector<char> > GetAttributeToSplit(std::vector<Example>& exampleSet, std::vector<TreeAttribute> currTreeAttributes);
	std::vector<Example> PruneDataset(char label, int attrIndex, const std::vector<Example>& exampleSet);
	char GetClassificationFromTree(Node *treeNode, const Example& currentTestData);
	double EvaluateAccuracy(Node *treeNode, const std::vector<Example>& dataSet);
	std::pair<double, double> GetAccuracyValues() const;
};

#endif