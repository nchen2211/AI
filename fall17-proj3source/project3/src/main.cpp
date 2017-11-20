#include <iostream>
#include <vector>
#include <map>
#include "mnist_reader.hpp"
#include "mnist_utils.hpp"
#include "bitmap.hpp"
#include "NaiveBayesianNetwork.h"
#include <sstream>
#include <iomanip>

#define MNIST_DATA_DIR "../mnist_data"
int main(int argc, char* argv[]) {
    //Read in the data set from the files
    mnist::MNIST_dataset<std::vector, std::vector<uint8_t>, uint8_t> dataset =
    mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>(MNIST_DATA_DIR);
    //Binarize the data set (so that pixels have values of either 0 or 1)
    mnist::binarize_dataset(dataset);
    //There are ten possible digits 0-9 (classes)
    int numLabels = 10;
    //There are 784 features (one per pixel in a 28x28 image)
    int numFeatures = 784;
    //Each pixel value can take on the value 0 or 1
    int numFeatureValues = 2;
    //image width
    int width = 28;
    //image height
    int height = 28;
    // get training images
    std::vector<std::vector<unsigned char>> trainImages = dataset.training_images;
    // get training labels
    std::vector<unsigned char> trainLabels = dataset.training_labels;
    // get test images
    std::vector<std::vector<unsigned char>> testImages = dataset.test_images;
    // get test labels
    std::vector<unsigned char> testLabels = dataset.test_labels;
    
    NaiveBayesianNetwork bayesianNetwork(numLabels, numFeatures);
    bayesianNetwork.SetTrainImages(trainImages);
    bayesianNetwork.SetTrainLabels(trainLabels);
    bayesianNetwork.SetTestImages(testImages);
    bayesianNetwork.SetTestLabels(testLabels);

    // train and test bayesian network
    bayesianNetwork.CalculateClassProb();
    bayesianNetwork.CalculatePixelProb();
    bayesianNetwork.EvaluateTestImages();

    // evaluate
    bayesianNetwork.VisualEvaluation();
    std::vector<Digit> digitVector = bayesianNetwork.GetDigitVector();

    // ##################  output network ############################
    std::ofstream outputFile;
    outputFile.open("../output/network.txt");
    if (outputFile.is_open())
    {
        for (int digit = 0; digit < numLabels; ++digit)
        {
            for (int pixel = 0; pixel < numFeatures; ++pixel)
            {
                outputFile << digitVector[digit].mWhitePixelProb[pixel] << std::endl;
            }
        }
        // final 10 lines
        for (int digit = 0; digit < numLabels; ++digit)
        {
            outputFile << digitVector[digit].mProb << std::endl;
        }
    }
    // ################################################################

    // ################ output classification #########################
    std::ofstream outputClassification;
    outputClassification.open("../output/classification-summary.txt");
    if (outputClassification.is_open())
    {
        for (int i = 0; i < numLabels; ++i)
        {
            for (int j = 0; j < numLabels; ++j)
            {
                outputClassification << std::setw(5) << digitVector[i].mPredictionVector[j];
            }
            outputClassification << std::endl;
        }
    }   
    outputClassification << "Accuracy " << bayesianNetwork.GetAccuracy() << "%\n";
      // ################################################################

    outputFile.close();
    outputClassification.close();

    return 0;
}

