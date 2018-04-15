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
	constituencyBase(const constituencyBase& con){

		name = con.name;
		country = con.name;
		county = con.name;
		area = con.area;
		doNotSwing = con.doNotSwing;

		parties = con.parties;
		votingAreas = con.votingAreas;

	}

	virtual void swing(unique_ptr<map<string,double>> swingVals, bool randomness = false);

	string getName(){return name;};
	string getCountry(){return country;};
	string getCounty(){return county;};
	int getArea(){return area;};
	int getElectorate();

	void addVoteArea(string nameArea, int electorateArea, map<string,int> votesCast_);
	unique_ptr<votingArea> getVoteArea(string nameArea);
	
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
	virtual int getNumSeats(string party) = 0;

	map<string,double> getSwings(unique_ptr<constituencyBase> oldResult);

	virtual void print(int opt = 1) = 0;

	virtual unique_ptr<constituencyBase> clone() const = 0;

};

#endif