#ifndef CONSTITMMP_H
#define CONSTITMMP_H

#include <string>
#include <map>
#include <vector>
#include "vars.h"
#include <memory>
#include <random>
#include "constituencyPR.h"

using namespace std;

class constituencyMMP: public constituencyPR{

private:

	vector<constituencyFPTP> subConstits;

public:

	static normal_distribution<double> dist;

	constituencyMMP(string name_, string country_, string county_, int area_, int electorate_, map<string,int> votesCast_, elPred::countSys sys,bool preventSwing = false):
	constituencyPR(name_,country_,county_,area_,electorate_,votesCast_,sys,preventSwing)
	{

	}
	constituencyMMP(string name_, string country_, string county_, int area_, int electorate_, map<string,int> votesCast_, elPred::countSys sys, vector<constituencyFPTP> subCons,bool preventSwing = false):
	constituencyPR(name_,country_,county_,area_,electorate_,votesCast_,sys,preventSwing)
	{
		subConstits = subCons;
	}

	void addSubConstit(constituencyFPTP subConstit);

	void swing(unique_ptr<map<string,double>> swingVals, bool randomness = false);

	map<string,int> getSeats();
	int numSeats(string party);

	map<string,int> getFPTPSeats();
	map<string,int> getPRSeats();
	int numFPTPSeats(string party);
	int numPRSeats(string party);

};

#endif