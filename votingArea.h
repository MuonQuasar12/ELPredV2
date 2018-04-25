#ifndef VOTINGAREA_H
#define VOTINGAREA_H

#include <string>
#include <map>
#include <vector>
#include "vars.h"
#include <memory>
#include <random>
#include <iostream>

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
			
		//cout<<"votingArea::parameterised constructor"<<endl;

		name = name_;
		electorate = electorate_;
		votesCast = votesCast_;
		doNotSwing = preventSwing;
	}
	votingArea(){

		//cout<<"votingArea::default constructor"<<endl;

		name = "null";
	}
	votingArea(const votingArea& vt){

		name = vt.name;
		electorate = vt.electorate;
		votesCast = vt.votesCast;
		doNotSwing = vt.doNotSwing;

	}

	void swing(unique_ptr<map<string,double>> swingVals, bool randomness = false);
	void setVals(map<string,int> newVotes){votesCast = newVotes;}
	int getElectorate(){return electorate;}
	string getName(){return name;}
	int getVotesCast(const string& party){return votesCast[party];}
	int getVotesCast();
	double getVoteShare(const string& party);
	double getTurnout();
	map<string, int> getVotesMap() { return votesCast; }
	map<string,double> getVoteShareMap();
	void setPreventSwing(bool opt){doNotSwing = opt;}
	map<string,double> getSwing(votingArea oldResult);
	bool partyContestsSeat(const string& party);
	void print();

};

#endif