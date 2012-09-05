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
	string line = "";
	string outLine = "", tmp;
	stringstream ss;
	map<string, int> records;
	map<string, int>::iterator rit;
	vector<string> vec;
	long long amount = 1;
	clock_t start = clock(), finish;
	
	while(inFile.good()){
		getline(inFile, line);
		if(line[0] == 'P' && outLine.size() > 2){
			records.clear();
			vec.clear();
			boost::split(vec, outLine, boost::is_any_of("\t "));
			for(int i = 1; i < vec.size(); i++){
				records[vec[i]]++;
			}
			outLine = "";
			for(rit = records.begin(); rit != records.end(); rit++){
				outLine += (*rit).first + " ";
				ss << (*rit).second;
				ss >> tmp;
				outLine += tmp + " ";
				ss.clear();
			}
			outFile << outLine << "\n";
			outLine = "";
			amount++;
		} else if(line[0] == 'Q'){
			outLine += line.substr(1,line.size()-1);
		}
	}
	if(outLine.size() > 2){
		records.clear();
		vec.clear();
		boost::split(vec, outLine, boost::is_any_of("\t "));
		for(int i = 1; i < vec.size(); i++){
			records[vec[i]]++;
		}
		outLine = "";
		for(rit = records.begin(); rit != records.end(); rit++){
			outLine += (*rit).first + " ";
			ss << (*rit).second;
			ss >> tmp;
			outLine += tmp + " ";
			ss.clear();
		}
		outFile << outLine << "\n";
		outLine = "";
	} else amount--;

	//inFile.clear(); outFile.clear();
	inFile.close();
	outFile.close();
	finish = clock();
	int seconds = (finish - start) / CLOCKS_PER_SEC;
	printf("%lld files collected with %d seconds\n", amount, seconds);
	time_t rawtime;
	time ( &rawtime );
	string timeStr = asctime(localtime ( &rawtime ));
	printf("%s\n", timeStr.c_str());
	//char* timeC = new char[timeStr.size()-1];
	char * timeC = (char *)timeStr.substr(4, timeStr.size()-5).c_str();
	for(int i = 0; i < timeStr.size()-1; i++)
		if(timeC[i] == ' ' || timeC[i] == ':') timeC[i] = '_';
	timeStr = string(timeC) + ".txt";
	ofstream logFile (timeStr.c_str());
	if(!logFile.is_open()){
		printf("Cannot open log file!");
		return 0;
	}
	logFile << amount << " files collected with " << seconds << "\n";
	logFile.close();
	return 0;
}
