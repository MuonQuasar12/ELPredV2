#ifndef ELECTION_H
#define ELECTION_H

#include "constituencyBase.h"
#include "date.h"
#include <map>
#include <vector>
#include <string>
#include <random>
#include <memory>

using namespace std;

class election{

private:

	//set externally
	string name = "GE2015";
	date Date;
	vector<unique_ptr<constituencyBase>> constitVec;
	bool isProj_ = false; //true when object is a projection

	//set internally by init()
	int electorate = 0;
	map<string,int> seatVec;
	map<string,int> totalVotes;
	

	void init();

public:
	election(){}
	election(unique_ptr<vector<unique_ptr<constituencyBase>>> constits, bool isAProj = false){
		constitVec = *constits;
		init();
		isProj_ = isAProj;
		if(isAProj) name = "GE2017proj";
	}
	election(date Date_,vector<unique_ptr<constituencyBase>> constits){
		Date = Date_;
		constitVec = constits;
		init();
	}
	election(const election &el){
		Date = date(el.getDate());
		constitVec = el.constitVec;
		isProj_ = el.isProj_;
		init();
	}

	election addNewResult(constituencyBase &constit);

	unique_ptr<constituencyBase> getConstit(int num);
	unique_ptr<constituencyBase> getConstit(const string& name);

	vector<constituencyBase> getConstitVec() const;
	int numConstits();
	int numConstits(int area);

	election swing(unique_ptr<map<int,map<string,double>>> swingVals, bool randomness = false);

	int getVotes();
	int getVotes(string party);
	int getVotes(int area);
	int getVotes(int area, string party);

	string Name(){ return name; }

	double getVoteShare(string party);
	int getSeats(string party);
	int getSeats(const string& party, int area);
	vector<string> getParties(int area = 0);
	void print(int option = 1);
	int getElectorate();
	double getTurnout();
	date getDate() const;
	bool isProj();

};

#endif