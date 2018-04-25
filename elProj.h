#include <vector>
#include "election.h"
#include "constituencyBase.h"
#include <utility>
#include <map>
#include <memory>
#include <iostream>

using namespace std;

class elProj{

private:

	vector<election> projectionList;
	vector<pair<unique_ptr<constituencyBase>,unique_ptr<constituencyBase>>> setConstits; //first old, second new
	election oldElWDec; //old election with new constits, no swing applied
	election initialEl;
	vector<map<string,int>> majoritiesMap;
	vector<pair<int,map<string,pair<double,double>>>> seatSpread;
	map<int,vector<map<string,pair<double,double>>>> seatSpreadRegions;
	vector<int> splitRegions = {1,2,6};

	bool autoProject = false;
	bool autoSim = false;
	bool outNameSet = false;

	string outName;

public:

	elProj(election& initEl){

		projectionList.push_back(initEl);

		oldElWDec = initEl;

		initialEl = initEl;
	}

	election getLatest();

	void addNewResult();
	void removeResult();

	void run();
	void setOpts();

	void project(bool randomness = false);

	void print(int choice = -1);
	void printDeclared();
	void printProjectedGains(election el = election());
	void checkResults(vector<election> elVec);

	void saveProj();
	void loadProj(bool randomness = false);
	void writeProjResults();
	void writeSimResults();
	void writeSeatSpread(const string& name, const string& input, int area = 0);

	map<int,map<string,double>> getSwingMap(bool randomness = false);

};