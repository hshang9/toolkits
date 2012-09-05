
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

class edge{
	public:
	int fromId;
	int toId;
	edge(){fromId = -1; toId = -1;}
	edge(int from, int to){fromId = from; toId = to;}
	bool operator==(edge e);
	bool operator<(edge e);
	bool operator>(edge e);
	bool operator<=(edge e);
	bool operator>=(edge e);
};
bool edge::operator== (edge e){
	return (fromId==e.fromId && toId==e.toId);
}
bool edge::operator> (const edge e){
	if(fromId > e.fromId) return true;
	else if(fromId == e.fromId) return (toId > e.toId);
	else return false;
}
bool edge::operator>= (const edge e){
	if(fromId < e.fromId) return false;
	else if(fromId == e.fromId) return (toId >= e.toId);
	else return true;
}
bool edge::operator<= (const edge e){
	if(fromId > e.fromId) return false;
	else if(fromId == e.fromId) return (toId <= e.toId);
	else return true;
}
bool edge::operator< (const edge e){
	if(fromId > e.fromId) return false;
	else if(fromId == e.fromId) return (toId < e.toId);
	else return true;
}

struct compareFunc{
	bool operator()(const edge e1, const edge e2) const{
		return ( (e1.fromId < e2.fromId) || ((e1.fromId == e2.fromId) && (e1.toId < e2.toId)) );
	}
};

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

	map<edge, int, compareFunc> structure;
	map<edge, int, compareFunc>::iterator it;
	clock_t start = clock(), finish;
	string line = "";
	structure.clear();
	vector<string> vec;
	edge *e;
	long num = 0;

	time_t rawtime;
	time ( &rawtime );
	string timeStr = asctime(localtime ( &rawtime ));
	printf("%s\n", timeStr.c_str());
	cout << "Start building the sturcture." << endl;
	system("date");
	
	while(inFile.good()){
		getline(inFile, line);
		vec.clear();
		boost::split(vec, line, is_any_of(" \t"));
		if(vec.size() > 2){
			e = new edge(atoi(vec[1].c_str()), atoi(vec[2].c_str()));
			if(e->fromId == e->toId) delete e;
			else structure[*e]++;//.insert(pair<edge, int>(e, 1));
		}
	}

	for(it = structure.begin(); it != structure.end(); it++){
		outFile << num++ << " " << it->first.fromId <<" " << it->first.toId << endl;
	}
	inFile.close();
	outFile.close();
	finish = clock();
	int seconds = (finish - start) / CLOCKS_PER_SEC;
	printf("Built the structure with %ld edge\n", num);
	printf("Used: %d seconds\n", seconds);
	system("date");
}
