#ifndef ELECTION_H
#define ELECTION_H

#include "constituencyBase.h"
#include "date.h"
#include <map>
#include <vector>
#include <string>
#include <random>
#include <memory>
#include <iostream>

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
	election(){
		//cout<<"election::Default constructor"<<endl;
	}
	election(unique_ptr<vector<unique_ptr<constituencyBase>>>& constits, bool isAProj = false){
		
		//cout<<"election::Constructor A"<<endl;

		constitVec.clear();

		//cout<<"Constit vector cleared, iterating over input constits"<<endl;

		for(auto& constitPtr : *constits){

			//cout<<".";
			constitVec.push_back(std::move(constitPtr));

		}
		
		//cout<<endl<<"Intialising el object"<<endl;
		init();
		//cout<<"Finished Intialising"<<endl;
		isProj_ = isAProj;
		if(isAProj) name = "GE2017proj";
		//cout<<"Done constructing"<<endl;
	}
	election(date Date_,vector<unique_ptr<constituencyBase>> constits){
		
		//cout<<"election::Contructor B"<<endl;

		Date = Date_;
			
		constitVec.clear();

		for(auto& constit : constits){

			constitVec.push_back(std::move(constit));

		}

		init();

		//cout<<"Done constructing"<<endl;
	}
	election(const election &el){
		
		//cout<<"election::copy constructor"<<endl;

		//el.constitVec[1]->print(1);

		Date = date(el.getDate());

		constitVec.clear();

		for(auto& constit : el.constitVec){

			constitVec.push_back(constit->clone());

		}

		isProj_ = el.isProj_;
		init();

		//cout<<"Done constructing"<<endl;
	}
	election& operator=(const election& otherEl){

		//cout<<"Election copy assignment"<<endl;

		name = otherEl.name;
		Date = otherEl.Date;

		constitVec.clear();

		for(auto& constit : otherEl.constitVec){

			constitVec.push_back(constit->clone());

		}

		isProj_ = otherEl.isProj_;

		init();

		//cout<<"Returning copy"<<endl;

		return *this;

	}

	election addNewResult(unique_ptr<constituencyBase> constit);

	unique_ptr<constituencyBase> getConstit(int num);
	unique_ptr<constituencyBase> getConstit(const string& name);

	vector<unique_ptr<constituencyBase>> const& getConstitVec() const;
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