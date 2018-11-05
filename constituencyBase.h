#ifndef CONSTITBASE_H
#define CONSTITBASE_H

#include <string>
#include <map>
#include <vector>
#include "vars.h"
#include <memory>
#include <random>
#include "votingArea.h"
#include <iostream>

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
		
		//cout<<"constituencyBase::Parameterised Constructor"<<endl;

		name = name_;
		country = country_;
		county = county_;
		area = area_;
		doNotSwing = preventSwing;
	}

	virtual ~constituencyBase(){
	}
	constituencyBase(){

		//cout<<"constituencyBase::default Constructor"<<endl;

		name = "null";
	}
	constituencyBase(const constituencyBase& con){

		//cout<<"constituencyBase::copy Constructor"<<endl;

		name = con.name;
		country = con.country;
		county = con.county;
		area = con.area;
		doNotSwing = con.doNotSwing;

		parties = con.parties;

		votingAreas.clear();

		for(auto& vt : con.votingAreas){

			votingAreas[vt.first] = votingArea(vt.second);

		}

	}

	virtual void swing(unique_ptr<map<string,double>> swingVals, bool randomness = false);
	virtual void swing(unique_ptr<constituencyBase> constit); //swings unset areas based on the set ones

	virtual void init() {/*nothing to be done*/};

	string getName(){return name;};
	string getCountry(){return country;};
	string getCounty(){return county;};
	int getArea(){return area;};
	int getElectorate();
	virtual int getNumSeats() = 0;

	virtual bool changedHands() = 0;

	virtual void addVoteArea(string nameArea, int electorateArea, map<string,int> votesCast_);
	votingArea& getVoteArea(string nameArea);
	int getNumVotingAreas(){return votingAreas.size();}
	map<string,votingArea>getVotingAreas(){return votingAreas;}
	
	int getVotesCast(const string& party);
	int getVotesCast();
	double getVoteShare(const string& party);
	double getTurnout();
	virtual map<string,double> getVoteShareMap();
	map<string,int> getVotesMap();
	bool partyContestsSeat(const string& party);
	vector<string> partiesContestingSeat(){return parties;}
	bool preventSwing(){return doNotSwing;}
	virtual void setPreventSwing(bool opt);
	virtual int getNumSeats(string party) = 0;

	virtual void addNewResult(string voteAreaName, map<string, int> results);
	map<string,double> getSwings(unique_ptr<constituencyBase> oldResult);

	virtual void print(int opt = 1) = 0;
	virtual string lineInfo() = 0;

	virtual unique_ptr<constituencyBase> clone() const = 0;

};

#endif