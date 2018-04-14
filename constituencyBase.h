#ifndef CONSTITBASE_H
#define CONSTITBASE_H

#include <string>
#include <map>
#include <vector>
#include "vars.h"
#include <memory>
#include <random>
#include "votingArea.h"

using namespace std;

class constituencyBase{

protected:
	string name;
	string country;
	string county;
	int area;
	map<string,votingArea> votingAreas;
	bool doNotSwing = false;
	vector<string> parties;	

public:

	constituencyBase(string name_, string country_, string county_, int area_, bool preventSwing = false){
		name = name_;
		country = country_;
		county = county_;
		area = area_;
		doNotSwing = preventSwing;
	}
	constituencyBase(){
		name = "null";
	}
	virtual ~constituencyBase();

	virtual void swing(unique_ptr<map<string,double>> swingVals, bool randomness = false);

	string getName(){return name;};
	string getCountry(){return country;};
	string getCounty(){return county;};
	int getArea(){return area;};
	int getElectorate();

	void addVoteArea(string nameArea, int electorateArea, map<string,int> votesCast_);
	votingArea getVoteArea(string nameArea);
	
	int getVotesCast(const string& party);
	int getVotesCast();
	double getVoteShare(const string& party);
	double getTurnout();
	map<string,double> getVoteShareMap();
	map<string,int> getVotesMap();
	bool partyContestsSeat(const string& party);
	vector<string> partiesContestingSeat(){return parties;}
	bool preventSwing(){return doNotSwing;}
	void setPreventSwing(bool opt);

	map<string,double> getSwings(constituencyBase& oldResult);

	virtual void print(int opt = 1) = 0;

};

#endif