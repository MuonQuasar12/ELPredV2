#ifndef CONSTITFPTP_H
#define CONSTITFPTP_H

#include <string>
#include <map>
#include <vector>
#include "vars.h"
#include <memory>
#include <random>
#include "constituencyBase.h"

using namespace std;

class constituencyFPTP: public constituencyBase{

private:
	string MP;
	bool hold = true;

public:

	static normal_distribution<double> dist;

	constituencyFPTP(string name_, string MP_, string country_, string county_, int area_, int electorate_, map<string,int> votesCast_, bool isHeld = true,bool preventSwing = false):
	constituencyBase(name_,country_,county_,area_,electorate_,votesCast_,preventSwing)
	{
		MP = MP_;
		hold = isHeld;
	}

	void swing(unique_ptr<map<string,double>> swingVals, bool randomness = false);

	string getMP();
	string getParty();
	string getSecondPlace();
	
	int getMajority();
	double getMajorityFrac();
	bool isHeld();
	void setHold(bool isAHold);

	void print(int opt = 1);

};

#endif