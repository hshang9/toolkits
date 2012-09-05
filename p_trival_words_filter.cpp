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
	char* dicFileName;
	char* inputFileName;
	char* outputFileName;
	while((ch = getopt(argc, argv, "i:o:d:"))!=EOF){
		switch(ch){
			case 'i':
				inputFileName = optarg;
				printf("Input file: %s\n", inputFileName);
				break;
			case 'o':
				outputFileName = optarg;
				printf("Output file: %s\n", outputFileName);
				break;
			case 'd':
				dicFileName = optarg;
				printf("Dictionary file: %s\n", dicFileName);
				break;
		}
	}

	ifstream inFile (inputFileName);
	ifstream dicFile (dicFileName);
	ofstream outFile (outputFileName);
	if(!inFile.is_open()){
		printf("Cannot open input file: %s\n", inputFileName);
		return 0;
	}
	if(!outFile.is_open()){
		printf("Cannot open output file: %s\n", outputFileName);
		return 0;
	}
	if(!dicFile.is_open()){
		printf("Cannot open dictionary file: %s\n", dicFileName);
		return 0;
	}

	map<string, int> dic;
	map<string, int>::iterator itdic;
	clock_t start = clock(), finish;
	string line = "";
	dic.clear();
	int num = 0, tmp;
	vector<string> vec;
	
	cout << "Start getting the dictionary" << endl;
	system("date");
	
	//get the dictionay from dicFile into the map dic
	while(dicFile.good()){
		getline(dicFile, line);
		if(line.size() > 1){
			dic.insert( pair<string, int>(line, num++) );
			
		}
	}
	cout << "Got the dictionary with " << num-1 << " words" << endl;
	system("date");

	cout << "Start filting the corpus" << endl;
	num = 0;
	long filted = 0;
	while(inFile.good()){
		getline(inFile, line);
		if(line.size() < 3) continue;
		num++;
		vec.clear();
		boost::split(vec, line, is_any_of("\t "));
		for(int i = 0; i < vec.size()-1; i+=2){
			tmp = atoi(vec[i+1].c_str());
			if(!tmp) break;
			if(dic.count(vec[i])){//the word exist in the dic
				outFile << vec[i] << " " << vec[i+1] << " ";
			} else filted++;
		}
		outFile << endl;
	}
	
	cout << "Corpus with " << num << " files got filted and " << filted << " trival words removed" << endl;
	system("date");
	
	inFile.close();
	outFile.close();
	dicFile.close();

}


