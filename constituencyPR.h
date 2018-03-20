#ifndef CONSTITPR_H
#define CONSTITPR_H

#include <string>
#include <map>
#include <vector>
#include "vars.h"
#include <memory>
#include <random>
#include "constituencyBase.h"

using namespace std;

class constituencyPR: public constituencyBase{

protected:

	elPred::countSys system;


public:

	static normal_distribution<double> dist;

	constituencyPR(string name_, string country_, string county_, int area_, int electorate_, map<string,int> votesCast_ ,elPred::countSys sys, bool preventSwing = false):
	constituencyBase(name_,country_,county_,area_,electorate_,votesCast_,preventSwing)
	{
		system = sys;
	}

	void swing(unique_ptr<map<string,double>> swingVals, bool randomness = false);

	map<string,int> getSeats();
	int numSeats(string party);

	void print(int opt = 1);

};

#endif