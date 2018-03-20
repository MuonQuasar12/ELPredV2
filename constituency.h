#ifndef CONSTIT_H
#define CONSTIT_H

#include <string>
#include <map>
#include <vector>
#include "vars.h"
#include <memory>
#include <random>

using namespace std;

class constituency{

private:
	string name;
	string MP;
	string country;
	string county;
	int area;
	int electorate;
	map<string,int> votesCast;
	bool hold = true;
	bool doNotSwing = false;
	

public:

	static normal_distribution<double> dist;

	constituency(string name_, string MP_, string country_, string county_, int area_, int electorate_, map<string,int> votesCast_, bool isHeld = true,bool preventSwing = false,elPred::elSys system = elPred::elSys::FPTP){
		name = name_;
		MP = MP_;
		country = country_;
		county = county_;
		area = area_;
		electorate = electorate_;
		votesCast = votesCast_;
		doNotSwing = preventSwing;
		hold = isHeld;
	}
	constituency(){
		name = "null";
	}

	void swing(unique_ptr<map<string,double>> swingVals, bool randomness = false);

	string getName();
	string getMP();
	string getCountry();
	string getCounty();
	string getParty();
	string getSecondPlace();
	int getArea();
	int getElectorate();
	
	int getVotesCast(const string& party);
	int getVotesCast();
	int getMajority();
	double getMajorityFrac();
	double getVoteShare(const string& party);
	double getTurnout();
	map<string,double> getVoteShareMap();
	bool partyContestsSeat(const string& party);
	vector<string> partiesContestingSeat();
	bool isHeld();
	bool preventSwing();
	void setHold(bool isAHold);
	void setPreventSwing(bool opt);

	map<string,double> getSwings(constituency oldResult);

	void print(int opt = 1);

};

#endif