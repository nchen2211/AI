#include "NaiveBayesianNetwork.h"
#include <iostream>
#include <exception>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <sstream>

NaiveBayesianNetwork::NaiveBayesianNetwork(unsigned int possibleDigits, unsigned int numFeatures)
{
	std::vector<int> v (784, 0);
	std::vector<double> vec (784, 0.0);

	// Go through 10 possible digits. 
	// Create and instantiate Digit object for each possible digit then add it to a vector
	for (int i = 0; i < possibleDigits; ++i) {
		Digit d;
		d.mNumImages = 0;
		d.mProb = 0.0;
		d.mWhitePixel = v;
		d.mWhitePixelProb = vec;
		d.mPredictionVector = v;

		mDigitVector.emplace_back(d);
	}
	// Store the number of possible digits and the number of pixels in an image
	mPossibleDigits = possibleDigits;
	mNumFeatures = numFeatures;
}

// Setting training images to a vector of vectors
void NaiveBayesianNetwork::SetTrainImages(const std::vector<std::vector<unsigned char>>& trainImages)
{
	mTrainImages = trainImages;
}

// Set training labels associated with training images to a vector
void NaiveBayesianNetwork::SetTrainLabels(const std::vector<unsigned char>& trainLabels)
{
	mTrainLabels = trainLabels;
}

// Setting test images to a vector of vectors
void NaiveBayesianNetwork::SetTestImages(const std::vector<std::vector<unsigned char>>& testImages)
{
	mTestImages = testImages;
}

// Set test labels associated with training images to a vector
void NaiveBayesianNetwork::SetTestLabels(const std::vector<unsigned char>& testLabels)
{
	mTestLabels = testLabels;
}

/*
This function is to calculate prior probability for each digit
*/
void NaiveBayesianNetwork::CalculateClassProb()
{
	// Get # of images of each digit c from training labels
	for (int i = 0; i < mTrainLabels.size(); ++i)
	{
		// Increment the number of image associated with each digit. 
		// Note: each index of mDigitVectors is identical to its element, so mTrainLabels[i] 
		//       can be utilized as mDigitVector index to increment the number of image
		// mDigitVector[0] = 0
		// mDigitVector[1] = 1 ...
		// mDigitVector[9] = 9
		++mDigitVector[static_cast<int>(mTrainLabels[i])].mNumImages;
	}

	// Determine the prior probabilities for each class (digit)
	double sum = 0.0;
	for (int i = 0; i < mPossibleDigits; ++i)
	{
		// Prior probability for each digit = num of images of digit i / total number of all training image set
		double prob = static_cast<double>(mDigitVector[i].mNumImages) / static_cast<double>(mTrainImages.size());
		
		// Store prior probability of each digit to "mProb" (Digit struct member variable)
		mDigitVector[i].mProb = prob;

		// Sum is not used anywhere else. It's for testing to make sure that the sum of prior probability = 1
		sum += prob;
	}
}

// calculate conditional prob for each pixel in each class (digit)
void NaiveBayesianNetwork::CalculatePixelProb()
{
	for (int digit = 0; digit < mPossibleDigits; ++digit) // for each digit
	{
		for (int pixel = 0; pixel < mNumFeatures; ++pixel) // for each pixel of this digit
		{
			for (int i = 0; i < mTrainImages.size(); ++ i) // go through each training image
			{
				if (static_cast<int>(mTrainLabels[i]) == digit) // if training label of training image i == digit
				{
					if (static_cast<int>(mTrainImages[i][pixel]) == 1) // if the feature == 1
					{
						++mDigitVector[digit].mWhitePixel[pixel];
					}
				}
			}

			// calculate and store conditional probability of white pixel given each digit
			double conditionalProb = static_cast<double>(mDigitVector[digit].mWhitePixel[pixel] + 1.0) / static_cast<double>(mDigitVector[digit].mNumImages + 2.0);
			mDigitVector[digit].mWhitePixelProb.at(pixel) = conditionalProb;
		}
	}
}

void NaiveBayesianNetwork::EvaluateTestImages()
{
	std::vector<double> digitProb;
	// calculate log of each class probability P(C = c)
	for (int digit = 0; digit < mPossibleDigits; ++digit) // 
	{
		digitProb.emplace_back(log(mDigitVector[digit].mProb) / log(2));
	}

	// calculate summation of probability of feature equal to pixel given class on each image
	for (int i = 0; i < mTestImages.size(); ++i) // for each test image
	{
		std::vector<double> allDigitMatchProb;
		for(int digit = 0; digit < mPossibleDigits; ++digit) // go through each digit to sum the prob of this pixel
		{
			double pixelProb = 0.0;
			double summation = 0.0;
			for (int pixel = 0; pixel < mNumFeatures; ++pixel) // for each pixel of this trained digit 
			{
				if (static_cast<int>(mTestImages[i][pixel]) == 1) // if the pixel is 1
				{
					pixelProb = mDigitVector[digit].mWhitePixelProb[pixel];
				}
				else 
				{
					pixelProb = 1 - mDigitVector[digit].mWhitePixelProb[pixel];
				}
				summation += (log(pixelProb) / log(2));
				
			}
			// store each digit match probabilities
			allDigitMatchProb.emplace_back(summation + digitProb[digit]);
		}
		// get the highest probability and classified this image test 
		int highestMatchDigit = std::max_element(allDigitMatchProb.begin(), allDigitMatchProb.end()) - allDigitMatchProb.begin();
		mTestImageMatch.emplace_back(highestMatchDigit);
	}

	// get accuracy
	int countMatch = 0;
	for (int i = 0; i < mTestLabels.size(); ++i)
	{
		if (mTestLabels[i] == mTestImageMatch[i])
		{
			++countMatch;
		}
	}
	mAccuracy = (static_cast<double>(countMatch) / static_cast<double>(mTestLabels.size())) * 100;

	// store num images that match the prediction
	for (int i = 0; i < mTestLabels.size(); ++i)
	{
		++mDigitVector[static_cast<int>(mTestLabels[i])].mPredictionVector[mTestImageMatch[i]];
	}
}

void NaiveBayesianNetwork::VisualEvaluation()
{

	for (int digit = 0; digit < mPossibleDigits; ++digit) {
		std::vector<unsigned char> classFs(mNumFeatures);
	
		for (int pixel = 0; pixel < mNumFeatures; pixel++) {
			//get probability of pixel f being white given class c
			double p = mDigitVector[digit].mWhitePixelProb[pixel];
			uint8_t v = 255*p;
			classFs[pixel] = (unsigned char)v;
		}
		std::stringstream ss;
		ss << "../output/digit" << digit <<".bmp";
		Bitmap::writeBitmap(classFs, 28, 28, ss.str(), false);
	}
}

std::vector<Digit> NaiveBayesianNetwork::GetDigitVector() const
{
	return mDigitVector;
}

double NaiveBayesianNetwork::GetAccuracy() const
{
	return mAccuracy;
}