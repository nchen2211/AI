proj3: main.o bitmap.o NaiveBayesianNetwork.o
	g++ -o proj3 main.o bitmap.o NaiveBayesianNetwork.o
main.o: main.cpp
	g++ -std=c++11 -c main.cpp 
bitmap.o: bitmap.cpp
	g++ -std=c++11 -c bitmap.cpp
NaiveBayesianNetwork.o: NaiveBayesianNetwork.cpp
	g++ -std=c++11 -c NaiveBayesianNetwork.cpp 
clean:
	rm proj3 main.o bitmap.o NaiveBayesianNetwork.o