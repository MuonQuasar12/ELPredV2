#ifndef CONSTITBASE_H
#define CONSTITBASE_H

#include <string>
#include <map>
#include <vector>
#include "vars.h"
#include <memory>
#include <random>

using namespace std;

class constituencyBase{

protected:
	string name;
	string country;
	string county;
	int area;
	int electorate;
	map<string,int> votesCast;
	bool doNotSwing = false;
	

public:

	static normal_distribution<double> dist;

	constituencyBase(string name_, string country_, string county_, int area_, int electorate_, map<string,int> votesCast_, bool preventSwing = false){
		name = name_;
		country = country_;
		county = county_;
		area = area_;
		electorate = electorate_;
		votesCast = votesCast_;
		doNotSwing = preventSwing;
	}
	constituencyBase(){
		name = "null";
	}

	void swing(unique_ptr<map<string,double>> swingVals, bool randomness = false);

	string getName();
	string getCountry();
	string getCounty();
	int getArea();
	int getElectorate();
	
	int getVotesCast(const string& party);
	int getVotesCast();
	double getVoteShare(const string& party);
	double getTurnout();
	map<string,double> getVoteShareMap();
	bool partyContestsSeat(const string& party);
	vector<string> partiesContestingSeat();
	bool preventSwing();
	void setPreventSwing(bool opt);

	map<string,double> getSwings(constituencyBase oldResult);

	void print(int opt = 1);

};

#endif