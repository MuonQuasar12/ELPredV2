#ifndef VOTINGAREA_H
#define VOTINGAREA_H

#include <string>
#include <map>
#include <vector>
#include "vars.h"
#include <memory>
#include <random>

using namespace std;

class votingArea{

private:

	string name;
	int electorate;
	map<string,int> votesCast;
	bool doNotSwing = false;

public:

	static normal_distribution<double> dist;

	votingArea(string name_, int electorate_, map<string,int> votesCast_,bool preventSwing = false){
		name = name_;
		electorate = electorate_;
		votesCast = votesCast_;
		doNotSwing = preventSwing;
	}
	votingArea(){
		name = "null";
	}

	void swing(unique_ptr<map<string,double>> swingVals, bool randomness = false);

	int getElectorate(){return electorate;}
	string getName(){return name;}
	int getVotesCast(const string& party){return votesCast[party];}
	int getVotesCast();
	double getVoteShare(const string& party);
	double getTurnout();
	map<string,double> getVoteShareMap();
	void setPreventSwing(bool opt){doNotSwing = opt;}
	map<string,double> getSwing(votingArea oldResult);
	bool partyContestsSeat(const string& party);


};

#endif