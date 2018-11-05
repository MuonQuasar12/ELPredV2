#include "constituencyFPTP.h"
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <iostream>
#include <random>
#include <chrono>
#include <memory>
#include "constituencyBase.h"
#include <ciso646>

using namespace std;

void constituencyFPTP::swing(unique_ptr<map<string,double>> swingVals, bool randomness){

	string initParty = getParty();

	constituencyBase::swing(std::move(swingVals),randomness);

	if(initParty != getParty())
		hold = false;

}

string constituencyFPTP::getParty(){

	pair<string,int> largestSoFar("null",0);

	for(auto it : getVotesMap()){
		if(it.second>largestSoFar.second){
			largestSoFar = it;
		}
	}
	return largestSoFar.first;

}
string constituencyFPTP::getSecondPlace(){

	string party = getParty();

	pair<string,int> largestSoFar("null",0);
	for(auto it : getVotesMap()){
		if(it.second>largestSoFar.second and it.first != party){
			largestSoFar = it;
		}
	}	

	return largestSoFar.first;

}
int constituencyFPTP::getMajority(){

	return getVotesCast(getParty())-getVotesCast(getSecondPlace());

}
double constituencyFPTP::getMajorityFrac(){

	double majority = getMajority();
	double totalVotes = getVotesCast();
	return majority/totalVotes;

}
int constituencyFPTP::getNumSeats(string party){

	if(party == getParty()) return 1;
	else return 0;

}
void constituencyFPTP::addNewResult(string voteAreaName, map<string, int> results){

	//check if hold/gain
	string initParty = getParty();

	//now apply changes
	constituencyBase::addNewResult(voteAreaName, results);

	string newParty = getParty();

	hold = (initParty == newParty);

}
string constituencyFPTP::lineInfo(){

	string gainHold;
	if(this->preventSwing()){
		if(isHeld())
			gainHold = " HOLD";
		else
			gainHold = " GAIN";
	}

	string outStr = name + " (" + getParty() + gainHold + ")";

	return outStr;

}

void constituencyFPTP::print(int opt){

	if(opt == 1)
		cout<<"Seat of "<<getName()<<" in "<<getCounty()<<", "<<getCountry()<<endl
			<<"Held by "<<getMP()<<" of "<<getParty()<<endl 
			<<"Turnout was "<<(100*getTurnout())<<"%"<<" at last vote, with the seat being contested by "<<partiesContestingSeat().size()<<" parties"<<endl
			<<getMP()<<" earned "<<getVotesCast(getParty())<<" votes ("<<(100*getVoteShare(getParty()))<<"%) when the seat was last contested, and currently holds a majority of "<<getMajority()<<" ("<<(getMajorityFrac()*100)<<"%) over "<<getSecondPlace()<<endl;

	if(opt == 2){
		cout<<"Seat of "<<getName()<<" results"<<endl;
		for(auto it : getVoteShareMap()){
			cout<<it.first<<": "<<it.second<<" ("<<(100*getVoteShare(it.first))<<"%)"<<endl;
		}
	}
	if(opt == 3){
		cout<<"Country: "<<country<<", county: "<<county<<", seat: "<<name;

		if(doNotSwing){

			string HOLD;
			if(hold) HOLD = "HOLD";
			else HOLD = "GAIN";

			cout<<" ("<<getParty()<<" "<<HOLD<<")";
		}

		cout<<endl;
	}
	if(opt == 4)
		cout<<"Seat: "<<getName()<<" area: "<<getArea()<<" ("<<getParty()<<")"<<" turnout "<<(100*getTurnout())<<"%, "<<partiesContestingSeat().size()<<" parties contesting seat. Majority "<<(getMajorityFrac()*100)<<"% over "<<getSecondPlace()<<endl;

}
unique_ptr<constituencyBase> constituencyFPTP::clone()const{

	return unique_ptr<constituencyBase>(new constituencyFPTP(*this));

}
