#pragma once
#include "bitmap.hpp"
#include <vector>
#include <unordered_map>

struct Digit 
{
	int mNumImages;
	double mProb; // probability of the number of this digit found in training images
	std::vector<int> mWhitePixel; // to store number of white pixel on this digit
	std::vector<double> mWhitePixelProb; // to store probability of pixel that are white
	std::vector<int> mPredictionVector;
};

class NaiveBayesianNetwork
{
public:
	NaiveBayesianNetwork(unsigned int numLabels, unsigned int numFeatures);
	// setter functions
	void SetTrainImages(const std::vector<std::vector<unsigned char>>& trainImages);
	void SetTrainLabels(const std::vector<unsigned char>& trainLabels);
	void SetTestImages(const std::vector<std::vector<unsigned char>>& testImages);
	void SetTestLabels(const std::vector<unsigned char>& testLabels);

	// calculate each class {0,1,2,3,4,5,6,7,8,9} probability
	void CalculateClassProb();
	// calculate conditional prob white pixel on each class
	void CalculatePixelProb();
	// evaluate test images
	void EvaluateTestImages();
	// visual evaluation
	void VisualEvaluation();
	
	// for outputting network and classification
	std::vector<Digit> GetDigitVector() const; 
	double GetAccuracy() const;

private:
	std::vector<std::vector<unsigned char>> mTrainImages;
	std::vector<std::vector<unsigned char>> mTestImages;
	std::vector<unsigned char> mTrainLabels;
	std::vector<unsigned char> mTestLabels;
	unsigned int mNumLabels;
	unsigned int mNumFeatures;

	std::vector<Digit> mDigitVector; // store 10 Digit struct in vector
	std::vector<int> mTestImageMatch; 
	// std::vector<std::vector<int>> mPredictionVector; // to store num images that match the prediction
	double mAccuracy;

};


