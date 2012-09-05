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
using namespace boost;

int main(int argc, char** argv){

	opterr = 0;
	int ch;
	char* inputFileName;
	char* outputFileName;
	char* nodeFileName;
	char* edgeFileName;
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

	map<string, int> words;
	map<string, int>::iterator itWords;
	clock_t start = clock(), finish;
	string line = "";
	words.clear();
	vector<string> vec;
	int num = 0, tmp;
	time_t rawtime;
	time ( &rawtime );
	string timeStr = asctime(localtime ( &rawtime ));
	printf("%s\n", timeStr.c_str());
	cout << "Start recording the sites' name." << endl;
	system("date");
	while(inFile.good()){
		getline(inFile, line);
		vec.clear();
		boost::split(vec, line, boost::is_any_of(" \t\n\r"));
		words[vec[0]]++;
	}
	for(itWords = words.begin(); itWords != words.end(); itWords++){
		outFile << itWords->first << endl;
	}
	
	finish = clock();
	int seconds = (finish - start) / CLOCKS_PER_SEC;
	time ( &rawtime );
	timeStr = asctime(localtime ( &rawtime ));
	printf("%s\n", timeStr.c_str());
	printf("%ld stopwords sorted\n", words.size());
	inFile.close();
	outFile.close();
}		
