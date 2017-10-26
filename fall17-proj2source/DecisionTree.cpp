#include "DecisionTree.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <cfloat>

DecisionTree::DecisionTree(int depth) {
	mMaxDepth = depth;
}

void DecisionTree::StartTree(std::string filename) {
	std::vector<Example> allDatasets = ParseFile(filename);
	AssignSets(allDatasets, 0.8);
	Node* currNode = new Node();
	mRoot = BuildTree(currNode, mTrainingSet, mTrainingSet, mTreeAttributes);
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
				while (std::getline(ss, attr,',')) {
					TreeAttribute a;
					a.mDescription = attr;
					a.mIsExpanded = false;
					mTreeAttributes.emplace_back(a);
				}
				// pop the first element since it's the classification
				mTreeAttributes.erase(mTreeAttributes.begin());
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

	// shuffle datasets
	// std::random_shuffle(allDatasets.begin(), allDatasets.end());
	// get the size of n% data
	int size = percentage * allDatasets.size(); 
	
	// store n % data to training set
	for (int i=0; i<size-1; ++i) 
		mTrainingSet.emplace_back(allDatasets[i]);

	if (mMaxDepth != -1) { // bounded tree
		
		int limit = 0.2 * allDatasets.size() + 1;

		// assign validation set
		int validStartIndex = size;
		for (int i=0; i<limit; ++i)  
			mValidationSet.emplace_back(allDatasets[size + i]);

		// assign test set
		int testStartIndex = limit;
		for (int i=0; i< limit; ++i)  
			mTestSet.emplace_back(allDatasets[testStartIndex + i]);
	} else { // unbounded tree
		// store 100 - n% of data to test set
		int limit = 0.2 * allDatasets.size();
		for (int i=0; i< limit; ++i)  
			mTestSet.emplace_back(allDatasets[size +i]);
	}

	std::cout << "training: " << mTrainingSet.size();
	std::cout << "test: " << mTestSet.size();

	// mTrainingSet = allDatasets;

	// for (int i=0; i<size; ++i) {
	// 	mTrainingSet.emplace_back(allDatasets[i]);
	// 	mTrainingMap[allDatasets[i].mExampleIndex] = allDatasets[i];
	// }
	
	// allDatasets.erase(allDatasets.begin(), allDatasets.begin() + size);	

	// for (int i=0; i<allDatasets.size(); ++i)  // assign 100 - n% of data to test set
	// 	mTestMap[allDatasets[i].mExampleIndex] = allDatasets[i];

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

	// size = 0.2 * allSize; // assign 20% of data to validation test
	// for (int i=0; i<size; ++i) 
	// 	mValidationMap[allDatasets[i].mExampleIndex] = allDatasets[i];
	
	// allDatasets.erase(allDatasets.begin(), allDatasets.begin() + size);

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
		// std::cout << "!! attr: " << attr.mDescription << std::endl;
		if (! currTreeAttributes[index].mIsExpanded) {//(! attr.mIsExpanded) {
			newAttributes.emplace_back(attr);
			std::vector<std::pair<char, int> > numLabelExamples;
			std::vector<std::pair<char, std::vector<Example> > > labelExamples;

			for (auto label : attr.mAttributeLabels) {// store the label(s) for this attribute
				numLabelExamples.emplace_back(std::make_pair(label, 0));
				std::vector<Example> e;
				labelExamples.emplace_back(std::make_pair(label, e));
				// std::cout << "!! label: " << numLabelExamples.back().first << std::endl;
			}
			
			int j = 0;
			for (auto& label : numLabelExamples) { // count # of example for each label attr
				// go through each example
				// std::cout << "!! processing label " << label.first << std::endl;
		
				for (int i=0; i<exampleSet.size(); ++i) {	
					// std::cout << "comparing example " << exampleSet[i].mAttrValues[eIndex] << " with label value " << label.first << std::endl;
					if (label.first == exampleSet[i].mAttrValues[eIndex] ) {
						// std::cout << "!! E" << eIndex;
						++label.second;
						labelExamples[j].second.emplace_back(exampleSet[i]);
						// std::cout << "SAME " << label.first << " increment value to " << label.second << std::endl;
					}

				}
				// std::cout << "!! num example: " << labelExamples[j].second.size() << std::endl;
				++j;
			}

			allAttrNumExamples.emplace_back(numLabelExamples);
			allAttrEx.emplace_back(labelExamples);
		}
		// else {
		// 	std::cout << "\n..MARKED " << attr.mDescription << std::endl;
		// 	std::cin >> x;
		// }
		// if (attr.mDescription == "gillattachment")
		// 	std::cin >> x;
		++index;
		++eIndex;
	}
	// ###################################################################################
	

	// ########################## calculate gain for each attribute #####################
	std::vector<double> allAttrGain;
	int i = 0;
	for (auto attrExamples : allAttrEx) {
		
		std::cout << "new current attribute: " << newAttributes[i].mDescription << std::endl;
		++i;
		double currAttrTotalExample = 0.0;
		double attrEntropy = 0.0;
		for (auto labelExamples : attrExamples) {
			currAttrTotalExample += static_cast<double>(labelExamples.second.size()); // total example for this attribute
		}

		double entropy = 0.0;
		double remainder = 0.0;
		double informationGain = 0.0;


		double countTrue = 0.0;
		double countFalse = 0.0;
		double pRatio = 0.0;
		double nRatio = 0.0;
		// 	double entropy = 0.0;
		double featureRatio = 0.0;

		
		for (auto labelExamples : attrExamples) {
			
			double currLabelTotalExample = static_cast<double>(labelExamples.second.size());
		
			// std::cout << "label " << labelExamples.first << std::endl;
			if (currLabelTotalExample > 0) {
				countTrue = GetSameValue(labelExamples,'1');
				countFalse = GetSameValue(labelExamples, '0');
				pRatio = countTrue / currLabelTotalExample;
				nRatio = countFalse / currLabelTotalExample;
				entropy = CalculateEntropy(pRatio, nRatio);
				

				featureRatio = currLabelTotalExample / currAttrTotalExample;
				attrEntropy -= (featureRatio * entropy);



				// std::cout << "pos " << countTrue << " neg " << countFalse << std::endl;
				// std::cout << "feature ratio " << featureRatio << std::endl;
			}
		}
		// informationGain = 
		// allAttrGain.emplace_back(1.0 - remainder);
		// std::cout << "feature entropy " << attrEntropy << std::endl << std::endl;
		allAttrGain.emplace_back(attrEntropy);
	}
	// ##################################################################################

	// std::cin >> x;
	// for (auto gain : allAttrGain)
	// 	std::cout << "gain " << gain <<  std::endl;

	// get max gain index
	// double maxGain = 0.0; int maxIndex = 0;
	// for (int i = 0; i < allAttrGain.size(); ++i) {
	// 	if (allAttrGain[i] > maxGain) {
	// 		maxGain = allAttrGain[i];
	// 		maxIndex = i;
	// 	}
	// }

	double minGain = DBL_MAX; int minIndex = 0;
	for (int i = 0; i < allAttrGain.size(); ++i) {
		if (allAttrGain[i] < minGain) {
			minGain = allAttrGain[i];
			minIndex = i;
		}
	}

	// std::cout << "\n gain size " << allAttrGain.size() << std::endl;
	// std::cout << "MAX INDEX " << maxIndex << " gain = " << maxGain <<std::endl;
	// std::cin >>x;
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
	// std::cout << "here" << std::endl;
	
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

	// std::cout << "~~~~~~~~~~~~~~~~~~~~~~~ in prunedataset ~~~~~~~~~~~~~~~~~~~~~\n";
	// std::cout << "attribute: " << mTreeAttributes[attrIndex].mDescription <<  " label: " << label << std::endl;
	
	for (int i = 0; i < exampleSet.size(); ++i) {
		if (exampleSet[i].mAttrValues[attrIndex] == label) {
			// std::cout << label;
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
		 std::cout << "\n hit example 0 \n";
		return currNode;
	}

	// if all attributes runs out
	if (IsAttributeEmpty(mTreeAttributes)) {
		currNode->mIsLeaf = true;
		currNode->mNodeValue = GetPluralityValue(currExamples);
		std::cout << "\n hit attributes empty \n";
		return currNode;
	}

	// if the tree depth reaches max depth for bounded tree only
	if (currNode->mNodeDepth == mMaxDepth) {
		currNode->mIsLeaf = true;
		currNode->mNodeValue = GetPluralityValue(currExamples);
		 std::cout << "\n hit max depth \n";
		return currNode;
	}

	// if all example classifications are the same
	if (IsSameClassification(currExamples)){ 
		currNode->mIsLeaf = true;
		currNode->mNodeValue = currExamples[0].mClassification;
		std::cout << "\n hit same classification \n";
		return currNode;
	} else { // if they are different, split feature
		
		std::pair<std::string, std::vector<char> > attrToSplit = GetAttributeToSplit(currExamples, currTreeAttributes);
		currNode->mNodeAttribute = attrToSplit.first;

		std::cout << "\nattr to split: " << currNode->mNodeAttribute;
		// std::cin >> x;
		
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

			// std::cout << "\nlabel " << attrLabel << std::endl;
			Node* childNode = new Node();
			childNode->mIsLeaf = false;
			childNode->mNodeValue = attrLabel;
			childNode->mNodeDepth = currNode->mNodeDepth + 1;
			childNode->mNodeParent = currNode;

	
			// get child's example set
			std::vector<Example> childExample = PruneDataset(attrLabel, attrIndex, currExamples);
			
			std::cout << "\nattr inside for loop " << attrToSplit.first;
			std::cout << "\nprune on: " << attrLabel << std::endl; 
			std::cout << "CHILD EXAMPLE " <<childExample.size() << std::endl;
			// std::cout << "for attr " << attrToSplit.first << " label " << attrLabel << std::endl;
			
			// for (auto example : childExample) {
			// 	// std::cout << "E" << example.mExampleIndex << " ";
			// 	std::cout << example.mClassification << " ";
			// 	for (auto value : example.mAttrValues) {
			// 		std::cout << value << " ";
			// 	}
			// 	std::cout << std::endl;
			// }

			++y;
			std::cout << "##############   count " << y << "    #################\n";
			// std::cin >> x;

			// add this attr label to 
			currNode->mNodeLabels.emplace_back(attrLabel);
			currNode->mNodeChildren.emplace_back(BuildTree(childNode, childExample, currExamples, currTreeAttributes));
		}
	}
	return currNode;
}