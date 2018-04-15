#ifndef CONSTITMMP_H
#define CONSTITMMP_H

#include <string>
#include <map>
#include <vector>
#include "vars.h"
#include <memory>
#include <random>
#include "constituencyPR.h"
#include "constituencyFPTP.h"

using namespace std;

class constituencyMMP: public constituencyPR{

private:

	vector<constituencyFPTP> subConstits;

public:

	static normal_distribution<double> dist;

	constituencyMMP(string name_, string country_, string county_, int area_, int electorate_, map<string,int> votesCast_, elPred::countSys sys,int numPRSeats,bool preventSwing = false):
	constituencyPR(name_,country_,county_,area_,sys,numPRSeats,preventSwing)
	{

	}
	constituencyMMP(string name_, string country_, string county_, int area_, int electorate_, map<string,int> votesCast_, elPred::countSys sys,int numPRSeats, vector<constituencyFPTP> subCons,bool preventSwing = false):
	constituencyPR(name_,country_,county_,area_,sys,numPRSeats,preventSwing)
	{
		subConstits = subCons;
		initSeats = getSeats();
		seatsMap = initSeats;
	}

	void addSubConstit(constituencyFPTP subConstit){
		subConstits.push_back(subConstit);
		initSeats = getSeats();
		seatsMap = initSeats;
	}

	virtual void swing(unique_ptr<map<string,double>> swingVals, bool randomness = false) override;

	virtual map<string,int> getSeats(map<string,int> seatPreMap = null_map()) override;
	virtual int getNumSeats(string party) override{return seatsMap[party];} ;

	map<string,int> getFPTPSeats();
	map<string,int> getPRSeats();
	int numFPTPSeats(string party){return getFPTPSeats()[party];}
	int numPRSeats(string party){return getPRSeats()[party];}

	virtual void print(int opt = 1) override;

	virtual unique_ptr<constituencyBase> clone() const override;

};

#endif