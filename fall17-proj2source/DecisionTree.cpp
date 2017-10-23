#include "DecisionTree.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <math.h>

DecisionTree::DecisionTree() {}

void DecisionTree::BuildTree(std::string filename) {
	ParseFile(filename);

	int depth = 2;
	Node* root = GenerateRoot();
	std::cout << "root attr: " << root->mNodeDescription;

	Node* parent = root;
	std::unordered_map<char, Node*>::iterator it = parent->mChildrenMap.begin();
	for (it; it != parent->mChildrenMap.end(); it++) {

		std::cout << "\negde: " << it->first << " child node attr: " << it->second->mNodeDescription;
	}
	// while (depth >= 0) {
	// 	for (it; it != parent->mChildrenMap.end(); it++) {
	// 		parent = 
	// 		GenerateSuccessor(parent, )
	// 		// std::cout << "\negde: " << it->first << " child node attr: " << it->second->mNodeDescription;
	// 	}
	// 	--depth;
	// }
	
}

void DecisionTree::ParseFile(std::string filename){
	
	std::ifstream inFile;
	inFile.open(filename);
	std::string line;

	// std::vector<std::pair<char, std::vector<char> > > allDatasets; // temporarily store all datasets
	std::vector<Example> allDatasets;
	int exampleIndex = 0;
	if (inFile.is_open()) {
		while (std::getline(inFile, line)) {
			std::stringstream ss(line);

			if (mAttributes.empty()) { // storing attributes

				std::string attr;
				while (std::getline(ss, attr,',')) {
					// mAttributes.emplace_back(attr);
					Attribute a;
					a.mDescription = attr;
					a.mIsExpanded = false;
					mAttributes.emplace_back(a);
				}

				mAttributes.erase(mAttributes.begin()); // pop the first element since it's the classification
			} else { // storing datasets
				std::string st;
				char tempClass;
				std::vector<char> tempV;
				int attrIndex = 0;

				while (std::getline(ss, st,',')) {
					if (st == "0" || st == "1")  // if classification
						tempClass = st[0];
					else {// if attribute values
						tempV.emplace_back(st[0]);

						// if attr value hasn't exists yet
						std::unordered_map<char, std::vector<Example>>::iterator iter = mAttributes[attrIndex].mValues.find(st[0]);
						if (iter == mAttributes[attrIndex].mValues.end()) {
							// store attr values for each attr
							mAttributes[attrIndex].mValues[st[0]];
						}
						++attrIndex;
					}
				}
				
				// create Example struct obj
				Example s;
				s.mClassification = tempClass;
				s.mAttrValues = tempV;
				s.mExampleIndex = exampleIndex;

				//store this example to allDatasets
				allDatasets.emplace_back(s);
				++exampleIndex;
			}
		}
	}
	inFile.close();

	// for (int i = 0; i < mAttributes.size(); ++i) {
	// 	std::cout << "attr: " << mAttributes[i].mDescription << " values: " ;
	// 	for (std::unordered_map<char, std::vector<Example>>::iterator it = mAttributes[i].mValues.begin(); it != mAttributes[i].mValues.end(); it++) {
	// 		std::cout << it->first << ",";
	// 	}
	// 	std::cout << std::endl;
	// }


	// for (int i = 0; i < allDatasets.size(); ++i) {
	// 	std::cout << "E: " << allDatasets[i].mExampleIndex << "classification " << allDatasets[i].mClassification << " values: " ;
	// 	for (int j = 0; j < allDatasets[i].mAttrValues.size(); ++j) 
	// 		std::cout << allDatasets[i].mAttrValues[j] << ",";
	// 	std::cout << std::endl;
	// }

	// shuffle datasets
	std::random_shuffle(allDatasets.begin(), allDatasets.end());
	// assign to training, validation, and test
	AssignSets(allDatasets);
	allDatasets.clear();
}

void DecisionTree::AssignSets(std::vector<Example>& allDatasets) {

	int allSize = allDatasets.size();
	int size = 0.6 * allSize; // assign 60% of data to training set

	for (int i=0; i<size; ++i) {
		mTrainingSet.emplace_back(allDatasets[i]);
		mTrainingMap[allDatasets[i].mExampleIndex] = allDatasets[i];
	}
	
	allDatasets.erase(allDatasets.begin(), allDatasets.begin() + size);	

	// std::cout << "\nTraining data set\n";
	// for (int i = 0; i <mTrainingSet.size(); ++i)
	// {
	// 	std::cout << "E" << mTrainingSet[i].mExampleIndex << ":";
	// 	std::cout << "classification: " << mTrainingSet[i].mClassification << " value: ";
	// 	for (int j=0 ;j<mTrainingSet[i].mAttrValues.size(); ++j) 
	// 		std::cout <<mTrainingSet[i].mAttrValues[j] << ",";
	// 	std::cout << std::endl;
	// }
	// std::cout << "size: " << mTrainingSet.size();

	size = 0.2 * allSize; // assign 20% of data to validation test
	for (int i=0; i<size; ++i) 
		mValidationMap[allDatasets[i].mExampleIndex] = allDatasets[i];
	
	allDatasets.erase(allDatasets.begin(), allDatasets.begin() + size);

	// std::cout << "\nValidation data set\n";
	// for (int i = 0; i <mValidationSet.size(); ++i)
	// {
	// 	std::cout << "E" << mValidationSet[i].mExampleIndex << ":";
	// 	std::cout << "classification: " << mValidationSet[i].mClassification << " value: ";
	// 	for (int j=0 ;j<mValidationSet[i].mAttrValues.size(); ++j) 
	// 		std::cout <<mValidationSet[i].mAttrValues[j] << ",";
	// 	std::cout << std::endl;
	// }
	// std::cout << "size: " << mValidationSet.size();

	for (int i=0; i<allDatasets.size(); ++i)  // assign 20% of data to test set
		mTestMap[allDatasets[i].mExampleIndex] = allDatasets[i];

	// std::cout << "\nTest data set\n";
	// for (int i = 0; i <mTestSet.size(); ++i)
	// {
	// 	std::cout << "E" << mTestSet[i].mExampleIndex << ":";
	// 	std::cout << "classification: " << mTestSet[i].mClassification << " value: ";
	// 	for (int j=0 ;j<mTestSet[i].mAttrValues.size(); ++j) 
	// 		std::cout <<mTestSet[i].mAttrValues[j] << ",";
	// 	std::cout << std::endl;
	// }
	// std::cout << "size: " << mTestSet.size() << std::endl << std::endl;
}

double CalculateEntropy(double pRatio) {
	// std::cout << "\npRatio: " << pRatio;
	double entropy = -1 * ( (pRatio * log2 (pRatio)) + ((1 - pRatio) * log2 (1 - pRatio)) );
	
	if (entropy > 0)
		return entropy;
	
	return 0;
}

void ResetAttribute(std::vector<Attribute>& mAttributes) {
	for (auto& attr : mAttributes) {
		std::unordered_map<char, std::vector<Example>>::iterator it = attr.mValues.begin();
		for (it; it!=attr.mValues.end(); it++) { // reset all the example for each attribute
			it->second.clear();
		}
	}
}

void DecisionTree::CalculateGain(std::vector<Example>& exampleSet) {

	// std::cout << "~~~~~~~~~~~~~~ in calculate gain ~~~~~~~~~~~~~\n";
	// std::cout << "total example " << exampleSet.size() << std::endl;
	ResetAttribute(mAttributes);

	// grouping examples based on attributes of the example set
	for (auto example : exampleSet) {
		int attrIndex = 0;

		for (auto value : example.mAttrValues) {
			int count = 0;
			if (! mAttributes[attrIndex].mIsExpanded) { 
				// find attr value of the current attribute
				std::unordered_map<char, std::vector<Example>>::iterator it = mAttributes[attrIndex].mValues.find(value);
				it->second.emplace_back(example);
			}
			++attrIndex;
		}	
	}

	std::cout << "\n\n EXAMPLES OF EACH ATTRIBUTES\n";
	for (auto attr : mAttributes) {
		std::cout << attr.mDescription << std::endl;
		std::unordered_map<char, std::vector<Example>>::iterator it = attr.mValues.begin();
		for (it; it != attr.mValues.end(); it++) {
			if (! attr.mIsExpanded) {
				std::cout << it->first << " (";
				// for (auto example : it->second) {
				// 	std::cout << "E" << example.mExampleIndex << ", ";
				// }
				std::cout << it->second.size();
			}
			std::cout << ")" << std::endl;
		}
	}
		
	// calculate gain for each attribute
	double totalExample = static_cast<double>(exampleSet.size()); // total example 
	for (auto &attr : mAttributes) {
		std::unordered_map<char, std::vector<Example>>::iterator it = attr.mValues.begin();
		
		if (! attr.mIsExpanded) {
			// std::cout << "\n\nCalculating gain for attribute: " << attr.mDescription;
			// std::cout << "";
			double remainder = 0.0;
			for (it; it != attr.mValues.end(); ++it) { // for each attr value in the map

				double attrExample =  static_cast<double>(it->second.size()); // store number of example for this attr value
				double countTrue = 0.0;

				// std::cout << "\n\nvalue: " << it->first;
				// std::cout << "\nattrExample: " << attrExample;

				for (auto elem : it->second) { // count true classification for this attr value
					if (elem.mClassification == '1') 
						++countTrue;
				}
				// std::cout << "\ncount true: " << countTrue;
				if (attrExample > 0) {
					double pRatio = (countTrue / attrExample);
					double entropy = CalculateEntropy(pRatio);
					// std::cout << "\ncountTrue: " << countTrue;
					// std::cout << "\nEntropy: " << entropy;
					// std::cout << "\nold remainder: " << remainder;
					remainder += ( (attrExample / totalExample) * entropy);
					// std::cout << "\nremainder: " << remainder;
				}
			}	
		// store the gain of this attribute
		attr.mGain = 1.0 - remainder;
		// std::cout << "\ngain = " << attr.mDescription << " = " << attr.mGain;
		}
	}
	// std::cout << "\n####################################\n";
}

int GetMaxGainIndex(std::vector<Attribute>& attrVector) {
	double maxGain = 0.0; int maxIndex = 0;
	for (int i =0; i<attrVector.size(); ++i) {
		// skip attribute that is already expanded
		if (! attrVector[i].mIsExpanded) {
			if (attrVector[i].mGain > maxGain) {
				maxGain = attrVector[i].mGain;
				maxIndex = i;
			}
		}
	}
	std::cout << "MAX GAIN = " << maxGain << std::endl;
	return maxIndex;
}

// get root
Node* DecisionTree::GenerateRoot() {

	// calculate gain for each attribute
	CalculateGain(mTrainingSet);
	// get attr index where it has the max gain
	int maxIndex = GetMaxGainIndex(mAttributes);
	std::cout << "Selected attribute: " << mAttributes[maxIndex].mDescription << std::endl;
	Node *root = new Node(mAttributes[maxIndex].mDescription);
	// set explored to true
	mAttributes[maxIndex].mIsExpanded = true;
	root->mIsLeaf = false;
	root->parent = NULL;

	GenerateSuccessor(root, mAttributes[maxIndex]);

	return root;
}

Node* DecisionTree::GenerateNode(std::vector<Example>& exampleSet, Node* parentNode, bool isLeaf) {

	Node *node;
	if (! isLeaf) { // split node
		// calculate gain for each attribute than hasn't been expanded
		CalculateGain(exampleSet);
		// get attr index where it has the max gain
		int maxIndex = GetMaxGainIndex(mAttributes);
		std::cout << "Selected attribute: " << mAttributes[maxIndex].mDescription << std::endl;
		// set explored to true
		mAttributes[maxIndex].mIsExpanded = true;
		// create Node obj 
		node = new Node(mAttributes[maxIndex].mDescription);
		node->mIsLeaf = false;
		
	} else { // leaf node
		node = new Node(std::string(1, exampleSet[0].mClassification));
		// set is leaf to false
		node->mIsLeaf = true;
	}
	
	// set its parent to parentNode
	node->parent = parentNode;
	
	// if (parentNode == NULL) {
	// 	std::cout << "\n\n Attribute *" << mAttributes[maxIndex].mDescription << "* is the root\n";
	// }
	// std::cout << "\n\n CHILDREN of Attribute *" << mAttributes[maxIndex].mDescription << "*\n";
	// std::unordered_map<char, std::vector<Example>>::iterator it = mAttributes[maxIndex].mValues.begin();
	// for (it; it != mAttributes[maxIndex].mValues.end(); it++) {
	// 	std::cout << "value: " << it->first << " (";
	// 	for (auto example : it->second) {
	// 		std::cout << "E" << example.mExampleIndex << " ";
	// 	}
	// 	std::cout << ")" << std::endl;
	// }
	return node;
}

// recursively generate successors
void DecisionTree::GenerateSuccessor(Node* parentNode, Attribute attr) {

	std::unordered_map<char, std::vector<Example>>::iterator it = attr.mValues.begin();
	for (it; it != attr.mValues.end(); it++) { // for each attr value
		
		std::cout << "PROCESSING VALUE " << it->first << std::endl;
		int x;
	
		// ######################### if example is empty #############################
		if (it->second.empty()) {
			std::cout << "\nempty example\n";
			std::cin >> x;
			// return plurality-value(parent_examples)
		}
		// ######################### if attribute is empty ###########################
		else if (mAttributes.empty()) {
			std::cout << "\nempty attribute";
			std::cin >> x;
		}
		// ################# if num of example for this attr value is > 1 ###########
		else if (it->second.size() >= 1) { 
			
			int trueClass = 0;
			Node* childNode = NULL;
			for (int i=0; i<it->second.size(); ++i) { // for each example in this attr value

				if (it->second[i].mClassification == '1')
					++trueClass;
			}

			// ############# if all of the classification value is the same ############
			if (trueClass == it->second.size() || trueClass == 0) {
				std::cout << "\nGenerating leaf node for edge value: " << it->first << std::endl;
				std::cin >> x;	
				// generate leaf node
				childNode = GenerateNode(it->second, parentNode, true);
				std::cout << "leaf node for edge " << it->first << " is " << childNode->mNodeDescription << std::endl;
				
			}
			// ############# if all the classification value is not the same ###########	
			else { 
				std::cout << "\nGenerating child node for edge value: " << it->first << std::endl;
				std::cout << "size of example " << it->second.size() << std::endl;
				std::cin >> x;	
				childNode = GenerateNode(it->second, parentNode, false);
			}

			// add child node to parent's children map
			parentNode->mChildrenMap[it->first] = childNode;	
		}
	}
}