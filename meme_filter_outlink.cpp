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

class record{
	public:
	int id;
	int postAmount;
	int linkedAmount;
	int linkingAmount;
	int validLinkingAmount;
	record(){}
	record(int p1, int p2, int p3, int p4, int p5){
		id = p1; 
		postAmount = p2; 
		linkedAmount = p3; 
		linkingAmount = p4; 
		validLinkingAmount = p5;
	}
};

int main(int argc, char** argv){

	opterr = 0;
	int ch;
	char* inputFileName;
	char* outputFileName;
	char* nodeFileName;
	char* edgeFileName;
	while((ch = getopt(argc, argv, "i:o:n:e:"))!=EOF){
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
				nodeFileName = optarg;
				printf("Nodes file: %s\n", nodeFileName);
				break;
			case 'e':
				edgeFileName = optarg;
				printf("Edges file: %s\n", edgeFileName);
				break;
		}
	}

	ifstream inFile (inputFileName);
	ofstream outFile (outputFileName);
	ofstream nodeFile (nodeFileName);
	ofstream edgeFile (edgeFileName);
	if(!inFile.is_open()){
		printf("Cannot open input file: %s\n", inputFileName);
		return 0;
	}
	if(!outFile.is_open()){
		printf("Cannot open output file: %s\n", outputFileName);
		return 0;
	}
	if(!nodeFile.is_open()){
		printf("Cannot open nodes' file: %s\n", nodeFileName);
		return 0;
	}
	if(!edgeFile.is_open()){
		printf("Cannot open edges' file: %s\n", edgeFileName);
		return 0;
	}

	map<string, record> nodes;
	map<string, record>::iterator itNodes;
	clock_t start = clock(), finish;
	string line = "";
	nodes.clear();
	int num = 0, tmp;
	long nodeAmount = 0;
	long edgeAmount = 0;
	long postAmount = 0;
	long linkAmount = 0;
	long invalidLinkAmount = 0;//edge + invalidLink = link
	vector<string> vec;

	time_t rawtime;
	time ( &rawtime );
	string timeStr = asctime(localtime ( &rawtime ));
	printf("%s\n", timeStr.c_str());
	cout << "Start recording the sites' name." << endl;
	system("date");
	while(inFile.good()){
		getline(inFile, line);
		if(line[0] == 'P' && line.size() > 2){
			vec.clear();
			boost::split(vec, line, boost::is_any_of("/ \t"));
			itNodes = nodes.find(vec[3]);
			if(itNodes == nodes.end()){
				nodeFile << nodeAmount << " " << vec[3] << endl;
				record *tmp = new record(nodeAmount++, 1, 0, 0, 0);
				nodes.insert(pair<string, record>(vec[3], *tmp));
			} else {
				itNodes->second.postAmount++;
			}
			postAmount++;
		}
	}
	

	inFile.clear();
	inFile.close();
	finish = clock();
	int seconds = (finish - start) / CLOCKS_PER_SEC;
	printf("%ld posts collected \n", postAmount);
	printf("%ld sites recorded \n", nodeAmount);
	printf("Used: %d seconds\n", seconds);

	time ( &rawtime );
	timeStr = asctime(localtime ( &rawtime ));
	printf("%s\n", timeStr.c_str());
	printf("Collecting edges\n");

	ifstream inFile2(inputFileName);
	if(!inFile2.is_open()){
		printf("Cannot open input file again: %s\n", inputFileName);
	}
	string pLine = "";
	string current = "";
	string linking = "";
	int id = -1;
	bool parsed = false;
	//recording edges
	while(inFile2.good()){
		getline(inFile2, line);
		if(line[0] == 'P'){
			pLine = line;
			parsed = false;
		} else if(line[0] == 'L'){
			if(!parsed){
				vec.clear();
				boost::split(vec, pLine, boost::is_any_of("/ \t"));
				current = vec[3];
				id = nodes[vec[3]].id;
				parsed = true;
			}
			vec.clear();
			boost::split(vec, line, boost::is_any_of("/ \t"));
			linking = vec[3];
			itNodes = nodes.find(vec[3]);
			if(itNodes != nodes.end()){//linking inside
				edgeFile << edgeAmount++ << " " << id << " " << itNodes->second.id << endl;
				itNodes->second.validLinkingAmount++;
				nodes[current].linkedAmount++;
			} else {//linking outside
				invalidLinkAmount++;
			}
			itNodes->second.linkingAmount++;
			linkAmount++;
		}
	}
	printf("Collected, writing to output files\n");
	for(itNodes = nodes.begin(); itNodes != nodes.end(); itNodes++){
		outFile << itNodes->second.id << " " << itNodes->first << " " << itNodes->second.postAmount << " " << itNodes->second.linkedAmount << " " << itNodes->second.validLinkingAmount << " " << itNodes->second.linkingAmount << endl;
	}
	inFile2.close();
	outFile.close();
	nodeFile.close();
	edgeFile.close();
	finish = clock();
	seconds = (finish - start) / CLOCKS_PER_SEC;
	printf("%ld nodes collected \n", nodeAmount);
	printf("%ld edges collected \n", edgeAmount);
	printf("%ld posts recorded \n", postAmount);
	printf("%ld outgoing links \n", invalidLinkAmount);
	printf("%ld total links \n", linkAmount);
	printf("Used: %d seconds\n", seconds);

	time ( &rawtime );
	timeStr = asctime(localtime ( &rawtime ));
	printf("%s\n", timeStr.c_str());
	printf("Collecting edges\n");

}
