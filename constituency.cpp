#include "constituency.h"
#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <random>
#include <chrono>
#include <memory>

using namespace std;

void constituency::swing(unique_ptr<map<string,double>> swingVals, bool randomness){

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator (seed);

	if(!doNotSwing){

		string initParty = getParty();
	
		map<string,double> swingMap;
		double normFactor = getVotesCast();
	
		//get swing in terms of votes

		double totalRand = 0;

		for(auto it = swingVals->begin(); it != swingVals->end(); it++){
			if(partyContestsSeat(it->first)){

				double randSwing = 0;

				if(randomness and next(it) != swingVals->end()){
					randSwing = dist(generator);
					totalRand += randSwing;
				}
				else if(randomness){
					randSwing = -totalRand;
				}

				swingMap[it->first] = (it->second+randSwing)*normFactor;
			//cout<<it->first<<" swings by "<<swingMap[it->first]<<endl;
			}
		}
	
		//apply swing
		for(auto it : swingMap){
			//cout<<it.first<<" had "<<votesCast[it.first]<<" votes"<<endl;
			votesCast[it.first] += it.second;
			//cout<<it.first<<" now has "<<votesCast[it.first]<<" votes"<<endl;
		}
	
		if(initParty != getParty())
			hold = false;
	}

}

string constituency::getName(){
	return name;
}
string constituency::getMP(){
	return MP;
}
string constituency::getCountry(){
	return country;
}
string constituency::getCounty(){
	return county;
}
string constituency::getParty(){
	pair<string,int> largestSoFar("null",0);

	for(auto it : votesCast){
		if(it.second>largestSoFar.second){
			largestSoFar = it;
		}
	}
	return largestSoFar.first;
}	
string constituency::getSecondPlace(){

	string party = getParty();

	pair<string,int> largestSoFar("null",0);
	for(auto it : votesCast){
		if(it.second>largestSoFar.second && it.first != party){
			largestSoFar = it;
		}
	}	

	return largestSoFar.first;

}
int constituency::getArea(){
	return area;
}
int constituency::getElectorate(){
	return electorate;
}
int constituency::getVotesCast(const string& party){

	if(partyContestsSeat(party))
		return votesCast[party];
	else 
		return 0;
}
int constituency::getVotesCast(){
	
	int total = 0;
	for(auto it : votesCast){
		total += it.second;
	}

	return total;
}
int constituency::getMajority(){

	return votesCast[getParty()]-votesCast[getSecondPlace()];

}
double constituency::getMajorityFrac(){
	double majority = getMajority();
	double totalVotes = getVotesCast();
	return majority/totalVotes;
}
double constituency::getVoteShare(const string& party){
	if(!partyContestsSeat(party)) return 0;

	double votes = votesCast[party];
	double totalVotes = getVotesCast();

	double share = votes/totalVotes;

	return share;

}
double constituency::getTurnout(){

	double totalVotes = getVotesCast();
	double electorate_d = electorate;

	double turnout = totalVotes/electorate_d;

	return turnout;
}
map<string,double> constituency::getVoteShareMap(){

	map<string,double> voteShareMap;

	for(auto it : votesCast){
		voteShareMap[it.first] = getVoteShare(it.first);
	}

	return voteShareMap;

}
bool constituency::partyContestsSeat(const string& party){
	for(auto it : votesCast){
		if(party == it.first) return true;
	}
	return false;
}
vector<string> constituency::partiesContestingSeat(){

	vector<string> contestVec;

	for(auto it : votesCast){
		contestVec.push_back(it.first);
	}

	return contestVec;

}
void constituency::print(int opt){

	if(opt == 1)
		cout<<"Seat of "<<getName()<<" in "<<getCounty()<<", "<<getCountry()<<endl
			<<"Held by "<<getMP()<<" of "<<getParty()<<endl 
			<<"Turnout was "<<(100*getTurnout())<<"%"<<" at last vote, with the seat being contested by "<<partiesContestingSeat().size()<<" parties"<<endl
			<<getMP()<<" earned "<<getVotesCast(getParty())<<" votes ("<<(100*getVoteShare(getParty()))<<"%) when the seat was last contested, and currently holds a majority of "<<getMajority()<<" ("<<(getMajorityFrac()*100)<<"%) over "<<getSecondPlace()<<endl;

	if(opt == 2){
		cout<<"Seat of "<<getName()<<" results"<<endl;
		for(auto it : votesCast){
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

}

map<string,double> constituency::getSwings(constituency oldResult){

	map<string,double> swings;

	map<string,double> newVoteShares = getVoteShareMap();
	map<string,double> oldVoteShares = oldResult.getVoteShareMap();

	for(auto partyVotes : newVoteShares){

		swings[partyVotes.first] = partyVotes.second - oldVoteShares[partyVotes.first];

	}

	return swings;

}

bool constituency::isHeld(){
	return hold;
}
bool constituency::preventSwing(){
	return doNotSwing;
}
void constituency::setPreventSwing(bool opt){
	doNotSwing = opt;
}
void constituency::setHold(bool isAHold){
	hold = isAHold;
}