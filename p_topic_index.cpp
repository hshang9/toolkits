#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

int main(int argc, char** argv){
	opterr = 0;
	int ch;
	char* inputFileName;
	char* outputFileName;
	int topicAmount = 50;
	while((ch = getopt(argc, argv, "i:o:"))!=EOF){
		switch(ch){
			case 'i':
				inputFileName = optarg;
				printf("Input file: %s\n", inputFileName);
				break;
			case 'o':
				outputFileName = optarg;
				printf("Output file: %s\n", outputFileName);
				break;
			case 'n':
				topicAmount = optarg;
				printf("Topic amount: %d\n", topicAmount);
				break;
		}
	}

	ifstream inFile (inputFileName);
	ofstream outFile (outputFileName);
	if(!inFile.is_open()){
		printf("Cannot open input file: %s\n", inputFileName);
		return 0;
	}
	if(!outFile.is_open()){
		printf("Cannot open output file: %s\n", outputFileName);
		return 0;
	}
	string line = "";
	string outLine = "", tmp;
	while(inFile.good()){
		getline(inFile, line);
		if(line.size() > 20){//valid line
			records.clear();
			vec.clear();
			boost::split(ec, line, boost::is_any_of("\t ");
			
		}
	}
