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
	char* stopwordsFileName;
	char* inputFileName;
	char* outputFileName;
	while((ch = getopt(argc, argv, "i:o:s:"))!=EOF){
		switch(ch){
			case 'i':
				inputFileName = optarg;
				printf("Input file: %s\n", inputFileName);
				break;
			case 'o':
				outputFileName = optarg;
				printf("Output file: %s\n", outputFileName);
				break;
			case 's':
				stopwordsFileName = optarg;
				printf("Stopwords file: %s\n", stopwordsFileName);
				break;
		}
	}

	ifstream inFile (inputFileName);
	ifstream stopwordsFile (stopwordsFileName);
	ofstream outFile (outputFileName);
	if(!inFile.is_open()){
		printf("Cannot open input file: %s\n", inputFileName);
		return 0;
	}
	if(!outFile.is_open()){
		printf("Cannot open output file: %s\n", outputFileName);
		return 0;
	}
	if(!stopwordsFile.is_open()){
		printf("Cannot open dictionary file: %s\n", stopwordsFileName);
		return 0;
	}

	map<string, int> dic;
	map<string, int> stopDic;
	map<string, int>::iterator itdic;
	clock_t start = clock(), finish;
	string line = "", outLine = "";
	dic.clear();
	stopDic.clear();
	int num = 0, tmp;
	vector<string> vec;
	
	cout << "Start recording tokens for building the dictionary" << endl;
	system("date");

	while(stopwordsFile.good()){
		getline(stopwordsFile, line);
		stopDic[line]++;
	}
	
	while(inFile.good()){
		getline(inFile, line);
		if(line[0] == 'P' && outLine.size() > 2){
			vec.clear();
			boost::split(vec, outLine, boost::is_any_of("\t "));
			for(int i = 1; i < vec.size(); i++){
				dic[vec[i]]++;
			}
			outLine = "";
		} else if(line[0] == 'Q'){
			outLine += line.substr(1,line.size()-1);
		}
	}
	if(outLine.size() > 2){
		vec.clear();
		boost::split(vec, outLine, boost::is_any_of("\t "));
		for(int i = 1; i < vec.size(); i++){
			dic[vec[i]]++;
		}
		outLine = "";
	}
	//get the dictionay from stopwordsFile into the map dic
	while(stopwordsFile.good()){
		getline(stopwordsFile, line);
		if(line.size() > 1){
			dic.insert( pair<string, int>(line, num++) );
			
		}
	}
	long wordAmount = 0;
	for(itdic = dic.begin(); itdic != dic.end(); itdic++){
		if(stopDic.count(itdic->first) == 0){
			outFile << itdic->first << endl;
			wordAmount++;
		}
	}
	inFile.close();
	stopwordsFile.close();
	outFile.close();
	int seconds = (finish - start) / CLOCKS_PER_SEC;
	printf("Built the dictionary with %ld words.\n", wordAmount);
	printf("Used: %d seconds\n", seconds);
	system("date");

}
