#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <ext/hash_map>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;
using namespace boost;
using namespace __gnu_cxx;
namespace __gnu_cxx {
template<> struct hash<string> {
  size_t operator() (const string& x) const {
    return hash<const char*>() (x.c_str());
  }
};
}

int ntopics;
bool debug = false;

class post{
public:
	long pid;
	long siteid;
	long timestamp;
	vector<int> theta;
	vector<string> linking;
	vector<post *> children;
	post(){};
	post(long ppid, long psiteid, long ptimestamp, vector<int> ptheta, vector<string> plinking){
		pid = ppid;
		siteid = psiteid;
		timestamp = ptimestamp;
		theta.clear();
		for(int i = 0; i < ntopics; i++) theta.push_back(ptheta[i]);
		linking.clear();
		for(int i = 0; i < plinking.size(); i++) linking.push_back(plinking[i]);
	}
};

void printPost(post p){
	printf("pid: %ld //siteid: %ld\ntimestamp: %ld\n", p.pid, p.siteid, p.timestamp);
	printf("linking %ld edges:\n", p.linking.size());
	for(int i = 0; i < p.linking.size(); i++) printf("%s\n", p.linking[i].c_str());
	printf("%ld children:\n", p.children.size());
	for(int i = 0; i < p.children.size(); i++) printf("%ld\n", p.children[i]->pid);
}

struct eqstr
{
  bool operator()(const string s1, const string s2) const
  {
    return strcmp(s1.c_str(), s2.c_str()) == 0;
  }
};

class cascade{
public:
	vector<long> postids;//record postids to remove circle and dirty data
	vector<long> postsiteids;//postsiteids and posttimes should have one to one relationship in order
	vector<long> posttimes;
	cascade(){}
};

//record the post information in cascade. Posts may have same siteid and all 
//their children information will be recorded, but only one siteid will be recorded. 
void recordcascade(cascade &c, post* root){
	//for(int i = 0; i < c.postids.size(); i++) if(root->pid == c.postids[i]) return;
	c.postids.push_back(root->pid);
	bool recorded = false;
	for(int i  = 0; i < c.postsiteids.size(); i++)
		if(root->siteid == c.postsiteids[i]) recorded = true;
	if(!recorded){
		c.postsiteids.push_back(root->siteid);
		c.posttimes.push_back(root->timestamp);
	}
	for(int i = 0; i < root->children.size(); i++){
		recordcascade(c, root->children[i]);
	}
}

void outputcascade(ofstream file, cascade c){
	for(int i = 0; i < c.postsiteids.size(); i++){
		file << c.postsiteids[i] << "," << c.posttimes[i] << ",";
	}
	file << endl;
}

//return the time in seconds since 1970.1.1 00:00:00 
long gettime(string t){
	vector<string> vec;
	vec.clear();
	boost::split(vec, t, is_any_of("-: \t"));
	if(vec.size() < 7){
		cout << "error in time format " << t << endl;
		return -1;
	}
			if(debug) cout << "ggggettimeE1" << endl;
	struct tm timeinfo;
	timeinfo.tm_year = atoi(vec[1].c_str()) - 1900;
	timeinfo.tm_mon = atoi(vec[2].c_str());
	timeinfo.tm_mday = atoi(vec[3].c_str());
	timeinfo.tm_sec = atoi(vec[6].c_str());
	timeinfo.tm_min = atoi(vec[5].c_str());
	timeinfo.tm_hour = atoi(vec[4].c_str());
	timeinfo.tm_isdst = -1;
	time_t time = mktime(&timeinfo);
	return (long)time;
}

vector<int> gettheta(string q){
	vector<int> result;
	result.push_back(1);
	return result;
}

int main(int argc, char** argv){

	opterr = 0;
	int ch;
	char* nodesFileName;
	char* inputFileName;
	char* outputFileName;
	while((ch = getopt(argc, argv, "i:o:n:t:"))!=EOF){
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
				nodesFileName = optarg;
				printf("Nodes file: %s\n", nodesFileName);
				break;
			case 't':
				ntopics = atoi(optarg);
				printf("Topic amount: %d\n", ntopics);
				break;
		}
	}

	ifstream inFile (inputFileName);
	ifstream nodeFile (nodesFileName);
	ofstream outFile (outputFileName);
	if(!inFile.is_open()){
		printf("Cannot open input file: %s\n", inputFileName);
		return 0;
	}
	if(!outFile.is_open()){
		printf("Cannot open output file: %s\n", outputFileName);
		return 0;
	}
	if(!nodeFile.is_open()){
		printf("Cannot open node file: %s\n", nodesFileName);
		return 0;
	}

	map<string, int> sites;
	hash_map<string, post > posts;
	map<string, int>::iterator itsites;
	hash_map<string, post >::iterator itposts;
	vector<post*> sources;

	clock_t start = clock(), finish;
	string line = "", outLine = "";
	sites.clear();
	posts.clear();
	int num = 0;
	vector<string> vec;
	long sitenum = 0, postnum = 0;
	
	cout << "Start recording sites and posts" << endl;
	system("date");

	while(nodeFile.good()){
		getline(nodeFile, line);
		vec.clear();
		boost::split(vec, line, is_any_of(" \t"));
		sites.insert(pair<string, int>(vec[1], atoi(vec[0].c_str())));
		if(line.size() > 2) sitenum++;
	}
	
	string currentP = "";
	string currentT = "";
	string currentQ = "";
	vector<string> currentL;
	long postsiteid, posttime, pustnum;
	vector<int> posttheta;
	cout << "Start reading input" << endl;
	int gggline = 0;
	getline(inFile, line);
	while(inFile.good()){
		gggline++;
		vec.clear();
		if(line[0] == 'P'){
			if(debug) cout << "gggP" << endl;
			boost::split(vec, line, is_any_of(" \t"));
			currentP = vec[1];
		} else if(line[0] == 'T'){
			currentT = line;
			if(debug) cout << "gggT" << endl;
		} else if(line[0] == 'Q'){
			currentQ += " ";
			currentQ += line;
			if(debug) cout << "gggQ" << endl;
		} else if(line[0] == 'L'){
			boost::split(vec, line, is_any_of(" \t"));
			currentL.push_back(vec[1]);
			if(debug) cout << "gggL" << endl;
		} else if(line.size() == 0){
			if(debug) cout << "gggE0:" << gggline << endl;
			boost::split(vec, currentP, is_any_of("/ \t"));
			postsiteid = sites[vec[2]];
			posttime = gettime(currentT);
			if(debug) cout << "gggE1" << endl;
			posttheta = gettheta(currentQ);
			post *p = new post(postnum, postsiteid, posttime, posttheta, currentL);
			posts.insert( pair<string, post>(currentP, *p) );
			if(debug) cout << "gggE2" << endl;
			postnum++;
			currentP = "";
			currentT = "";
			currentQ = "";
			currentL.clear();
			if(debug) cout << "gggE" << endl;
		}
		getline(inFile, line);
	}
	cout << sitenum << "sites and " << postnum << " posts collected" << endl;
	printf("Start to analysising structure\n");

	hash_map<string, post>::iterator itlinking;
	for(itposts = posts.begin(); itposts != posts.end(); itposts++){
		for(int i = 0; i < itposts->second.linking.size(); i++){
			itlinking = posts.find(itposts->second.linking[i]);
			if(itlinking == posts.end()){//linking outside the corpus
				itposts->second.linking.erase(itposts->second.linking.begin()+i);
			} else {//linking inside the corpus
				itlinking->second.children.push_back(&(itposts->second));
			}
		}
		if(itposts->second.linking.size() == 0){//no parents, a potential source post
			sources.push_back((post*)&(itposts->second));
		}
	}
	if(debug) cout << "ggg sources size: " << sources.size() << endl;
	post *tmp;
	cascade c;
	int cascadeAmount = 0;
	while(!sources.empty()){
		c.postids.clear();
		c.postsiteids.clear();
		c.posttimes.clear();
		tmp = sources.back();
		sources.pop_back();
		recordcascade(c, tmp);
		if(debug) cout << "ggg cascade size: " << c.posttimes.size() << endl;
		if(c.postsiteids.size() > 1){
			if(debug) cout << "ggg cascades" << cascadeAmount++ << endl;
			for(int i = 0; i < c.postsiteids.size(); i++){
				outFile << c.postsiteids[i] << "," << c.posttimes[i]-c.posttimes[0] << ",";
			}
			outFile << endl;
		}
	}

}
