#include "DecisionTree.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <cfloat>

DecisionTree::DecisionTree(int depth) {
	mMaxDepth = depth;
	mAccuracyValues.first = 0.0;
	mAccuracyValues.second = 0.0;
}

DecisionTree::~DecisionTree() {
	delete mRoot;
}

void DecisionTree::StartTree(std::string filename) {
	std::vector<Example> allDatasets = ParseFile(filename);
	AssignSets(allDatasets, 0.8);
	Node* currNode = new Node();
	mRoot = BuildTree(currNode, mTrainingSet, mTrainingSet, mTreeAttributes);

	if (mMaxDepth == -1) { // unbounded tree
		double trainAccuracy = EvaluateAccuracy(mRoot, mTrainingSet);
		double testAccuracy = EvaluateAccuracy(mRoot, mTestSet);
		
		mAccuracyValues.first = trainAccuracy;
		mAccuracyValues.second = testAccuracy;		

	} else { // bounded tree
		double trainDataAccuracy = EvaluateAccuracy(mRoot, mTrainingSet);
		double validationDataAccuracy = EvaluateAccuracy(mRoot, mValidationSet);
		
		mAccuracyValues.first = trainDataAccuracy;
		mAccuracyValues.second = validationDataAccuracy;
	}
}

std::pair<double, double> DecisionTree::GetAccuracyValues() const {
	return mAccuracyValues;
}

std::vector<Example> DecisionTree::ParseFile(std::string filename){
	
	std::ifstream inFile;
	inFile.open(filename);
	std::string line;

	std::vector<Example> allDatasets;
	int exampleIndex = 0;
	if (inFile.is_open()) {
		while (std::getline(inFile, line)) {
			std::stringstream ss(line);
	
			// storing attributes
			if (mTreeAttributes.empty()){
				std::string attr;
				int attrIndex = 0;
				while (std::getline(ss, attr,',')) {

					if (attrIndex > 0) {
						TreeAttribute a;
						a.mDescription = attr;
						a.mIsExpanded = false;
						a.attrIndex = attrIndex - 1;
						mTreeAttributes.emplace_back(a);

						// store to map (this is for evaluation purposes)
						mAttributeMap[attr] = attrIndex - 1;
					}				
					++attrIndex;
				}
				// pop the first element since it's the classification
				// mTreeAttributes.erase(mTreeAttributes.begin());
			} else { // storing datasets and attributes labels
				std::string st;
				char tempClassification;
				std::vector<char> tempV;
				int attrIndex = 0;

				while (std::getline(ss, st,',')) {
					// if classification
					if (st == "0" || st == "1")  
						tempClassification = st[0];
					// if attribute labels
					else {
						tempV.emplace_back(st[0]); // temporarily store labels for each example

						// adding labels to each attribute
						if (mTreeAttributes[attrIndex].mAttributeLabels.empty()) {
							mTreeAttributes[attrIndex].mAttributeLabels.emplace_back(st[0]);
						}
						else {
							std::vector<char>::iterator it = find(mTreeAttributes[attrIndex].mAttributeLabels.begin(), mTreeAttributes[attrIndex].mAttributeLabels.end(),st[0]);
							if (it == mTreeAttributes[attrIndex].mAttributeLabels.end()) 
								mTreeAttributes[attrIndex].mAttributeLabels.emplace_back(st[0]);
						}
						++attrIndex;
					}
				}
		
				// create Example struct obj
				Example s;
				s.mClassification = tempClassification;
				s.mAttrValues = tempV;
				s.mExampleIndex = exampleIndex;

				//store this example to allDatasets
				allDatasets.emplace_back(s);
				++exampleIndex;
			}
		}
	}
	inFile.close();

	// for (int i = 0; i < mTreeAttributes.size(); ++i) {
	// 	std::cout << "attr: " << mTreeAttributes[i].mDescription << " values: " ;
	// 	for (int j =0; j < mTreeAttributes[i].mAttributeLabels.size(); ++j) {
	// 		std::cout << mTreeAttributes[i].mAttributeLabels[j];
	// 	}
	// 	std::cout << std::endl;
	// }

	// int x; std::cin>>x;
	// for (int i = 0; i < allDatasets.size(); ++i) {
	// 	std::cout << "E: " << allDatasets[i].mExampleIndex << "classification " << allDatasets[i].mClassification << " values: " ;
	// 	for (int j = 0; j < allDatasets[i].mAttrValues.size(); ++j) 
	// 		std::cout << allDatasets[i].mAttrValues[j] << ",";
	// 	std::cout << std::endl;
	// }

	return allDatasets;
}

void DecisionTree::AssignSets(std::vector<Example>& allDatasets, double percentage) {

	// get the size of n% data
	int size = percentage * allDatasets.size(); 
	
	// store n % data to training set
	for (int i=0; i<size-1; ++i) 
		mTrainingSet.emplace_back(allDatasets[i]);
	// shuffle datasets
	std::random_shuffle(mTrainingSet.begin(), mTrainingSet.end());

	if (mMaxDepth != -1) { // bounded tree
		
		int limit = 0.2 * allDatasets.size() + 1;

		// assign validation set
		int validStartIndex = size;
		for (int i=0; i<limit; ++i)  
			mValidationSet.emplace_back(allDatasets[size + i]);
		std::random_shuffle(mValidationSet.begin(), mValidationSet.end());

	} else { // unbounded tree
		// store 100 - n% of data to test set
		int limit = 0.2 * allDatasets.size();
		for (int i=0; i< limit; ++i)  
			mTestSet.emplace_back(allDatasets[size +i]);
		std::random_shuffle(mTestSet.begin(), mTestSet.end());
	}

	allDatasets.clear();
}

double GetSameValue(const std::pair<char, std::vector<Example> >& labelExample, char classification) {
	double value = 0.0;
	for (auto example : labelExample.second) {
		if (example.mClassification == classification) {
			++value;
		}
	}
	return value;
}

double CalculateEntropy(double pRatio, double nRatio) {

	double posEntropy = 0.0;
	double negEntropy = 0.0;
	if (pRatio == 0 || pRatio == 1)
		posEntropy = 0.0;
	else
		posEntropy = pRatio * (log(pRatio)/log(2));

	if (nRatio == 0 || nRatio == 1)
		negEntropy = 0.0;
	else
		negEntropy = nRatio * (log(nRatio)/log(2));


	return posEntropy + negEntropy;
}

// return a pair of attribute and the attribute labels
std::pair<std::string, std::vector<char> > DecisionTree::GetAttributeToSplit(std::vector<Example>& exampleSet, std::vector<TreeAttribute> currTreeAttributes) {

	// ################  get number of example of each attribute labels ##################
	unsigned int eIndex = 0; // index for each column
	std::vector<std::vector<std::pair<char, int> > > allAttrNumExamples; // store pair vector of examples of each attr label
	std::vector<std::vector<std::pair<char, std::vector<Example> > > > allAttrEx; // store pair vector of all example objects contain in each attribute label
	
	std::vector<TreeAttribute> newAttributes;
	int index = 0;
	for (auto attr : mTreeAttributes) {
		if (! currTreeAttributes[index].mIsExpanded) {//(! attr.mIsExpanded) {
			newAttributes.emplace_back(attr);
			std::vector<std::pair<char, int> > numLabelExamples;
			std::vector<std::pair<char, std::vector<Example> > > labelExamples;

			for (auto label : attr.mAttributeLabels) {// store the label(s) for this attribute
				numLabelExamples.emplace_back(std::make_pair(label, 0));
				std::vector<Example> e;
				labelExamples.emplace_back(std::make_pair(label, e));
			}
			
			int j = 0;
			for (auto& label : numLabelExamples) { // count # of example for each label attr
				// go through each example
				for (int i=0; i<exampleSet.size(); ++i) {	
					if (label.first == exampleSet[i].mAttrValues[eIndex] ) {
						++label.second;
						labelExamples[j].second.emplace_back(exampleSet[i]);
					}
				}
				++j;
			}

			allAttrNumExamples.emplace_back(numLabelExamples);
			allAttrEx.emplace_back(labelExamples);
		}
		++index;
		++eIndex;
	}
	// ###################################################################################
	

	// ########################## calculate gain for each attribute #####################
	std::vector<double> allAttrGain;
	
	for (auto attrExamples : allAttrEx) {
		
		double currAttrTotalExample = 0.0;
		double attrEntropy = 0.0;
		for (auto labelExamples : attrExamples) {
			currAttrTotalExample += static_cast<double>(labelExamples.second.size()); // total example for this attribute
		}

		double entropy = 0.0;
		double remainder = 0.0;
		double informationGain = 0.0;
		
		for (auto labelExamples : attrExamples) {
			
			double currLabelTotalExample = static_cast<double>(labelExamples.second.size());
		
			if (currLabelTotalExample > 0) {
				double countTrue = GetSameValue(labelExamples,'1');
				double countFalse = GetSameValue(labelExamples, '0');
				double pRatio = countTrue / currLabelTotalExample;
				double nRatio = countFalse / currLabelTotalExample;
				entropy = CalculateEntropy(pRatio, nRatio);
				

				double featureRatio = currLabelTotalExample / currAttrTotalExample;
				attrEntropy -= (featureRatio * entropy);
			}
		}
		allAttrGain.emplace_back(attrEntropy);
	}
	// ##################################################################################


	double minGain = DBL_MAX; int minIndex = 0;
	for (int i = 0; i < allAttrGain.size(); ++i) {
		if (allAttrGain[i] < minGain) {
			minGain = allAttrGain[i];
			minIndex = i;
		}
	}

	std::pair<std::string, std::vector<char> > attrToSplit;
	attrToSplit.first = newAttributes[minIndex].mDescription;
	attrToSplit.second = newAttributes[minIndex].mAttributeLabels;

	return attrToSplit;
}

// to check if all attributes have been expanded
bool IsAttributeEmpty(std::vector<TreeAttribute>& mTreeAttributes) {
	for (auto attr : mTreeAttributes) {
		if (! attr.mIsExpanded)
			return false;
	}
	return true;
}

// return true if all classifications are the same
bool IsSameClassification(std::vector<Example>& exampleSet) {	
	char classification = exampleSet[0].mClassification;
	for (int i=1; i<exampleSet.size(); ++i) {

		if (classification != exampleSet[i].mClassification) 
			return false;
	}
	return true;
}

// grouping examples based on splitted attribute label(s)
std::vector<Example> DecisionTree::PruneDataset(char label, int attrIndex, const std::vector<Example>& exampleSet) {
	std::vector<Example> prunedExamples;

	for (int i = 0; i < exampleSet.size(); ++i) {
		if (exampleSet[i].mAttrValues[attrIndex] == label) {
			prunedExamples.emplace_back(exampleSet[i]);
		}
	}
	return prunedExamples;
}

char GetPluralityValue(const std::vector<Example>& exampleSet) {
	int trueValue = 0;
	int falseValue = 0;

	for (auto example : exampleSet) {
		if (example.mClassification == '0')
			++falseValue;
		else if (example.mClassification == '1')
			++trueValue;
	}

	if (trueValue > falseValue)
		return '1';
	return '0';
}

// recursively build decision tree
Node* DecisionTree::BuildTree(Node* currNode, std::vector<Example>& currExamples, std::vector<Example>& parentExamples, std::vector<TreeAttribute> currTreeAttributes) {

	// if example runs out
	if (currExamples.size() == 0) {
		currNode->mIsLeaf = true;
		currNode->mNodeValue = GetPluralityValue(currExamples);
		currNode->mNodeAttribute = "";
		return currNode;
	}

	// if all attributes runs out
	if (IsAttributeEmpty(mTreeAttributes)) {
		currNode->mIsLeaf = true;
		currNode->mNodeValue = GetPluralityValue(currExamples);
		currNode->mNodeAttribute = "";
		return currNode;
	}

	// if the tree depth reaches max depth for bounded tree only
	if (currNode->mNodeDepth == mMaxDepth) {
		currNode->mIsLeaf = true;
		currNode->mNodeValue = GetPluralityValue(currExamples);
		currNode->mNodeAttribute = "";
		return currNode;
	}

	// if all example classifications are the same
	if (IsSameClassification(currExamples)){ 
		currNode->mIsLeaf = true;
		currNode->mNodeValue = currExamples[0].mClassification;
		return currNode;
	} else { // if they are different, split feature
		
		std::pair<std::string, std::vector<char> > attrToSplit = GetAttributeToSplit(currExamples, currTreeAttributes);
		currNode->mNodeAttribute = attrToSplit.first;
		
		int attrIndex = 0;
		// set flag to this attribute as expanded
		for (int i = 0; i < mTreeAttributes.size(); ++i) {
			if (currTreeAttributes[i].mDescription == currNode->mNodeAttribute) {
				currTreeAttributes[i].mIsExpanded = true;
				attrIndex = i;
				break;
			}
		}  

		// generate successor(s)
		for (auto attrLabel : attrToSplit.second) {

			Node* childNode = new Node();
			childNode->mIsLeaf = false;
			childNode->mNodeValue = attrLabel;
			childNode->mNodeDepth = currNode->mNodeDepth + 1;
			childNode->mNodeParent = currNode;

	
			// get child's example set
			std::vector<Example> childExample = PruneDataset(attrLabel, attrIndex, currExamples);
			// add this attr label to 
			currNode->mNodeLabels.emplace_back(attrLabel);
			currNode->mNodeChildren.emplace_back(BuildTree(childNode, childExample, currExamples, currTreeAttributes));
		}
	}
	return currNode;
}

char DecisionTree::GetClassificationFromTree(Node *treeNode, const Example& currentTestData) {
	char nodeResultValue = treeNode->mNodeValue;

	while (! treeNode->mIsLeaf && ! treeNode->mNodeChildren.empty()) {
		// get index of this node attribute
		int attrIndex = mAttributeMap[treeNode->mNodeAttribute];

		// get the label value of the current test data for this feature
		char testLabelValue = currentTestData.mAttrValues[attrIndex];

		int childNodeIndex = -1;
		for (int i=0; i< mTreeAttributes[attrIndex].mAttributeLabels.size(); ++i) {
			if (testLabelValue == mTreeAttributes[attrIndex].mAttributeLabels[i]) {
				childNodeIndex = i; break;
			}
		}
		
		treeNode = treeNode->mNodeChildren[childNodeIndex];
		nodeResultValue = treeNode->mNodeValue;
	}
	return nodeResultValue;
}

double DecisionTree::EvaluateAccuracy(Node *treeNode, const std::vector<Example>& dataSet) {

	int countMatch = 0;
	for (auto example : dataSet) {
		char classification = GetClassificationFromTree(treeNode, example);
		if (classification == example.mClassification) 
			++countMatch;	
	}
	return ((double) countMatch / dataSet.size());
}