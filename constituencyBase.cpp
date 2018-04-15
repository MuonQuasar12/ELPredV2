#include <string>
#include <map>
#include "constituencyBase.h"

using namespace std;

void constituencyBase::swing(unique_ptr<map<string,double>> swingVals, bool randomness){

	for(auto& areaPair : votingAreas){

		areaPair.second.swing(std::move(swingVals),randomness);

	}

}

int constituencyBase::getElectorate(){

	int total = 0;

	for(auto areaPair : votingAreas){

		total += areaPair.second.getElectorate();

	}

	return total;

}

void constituencyBase::addVoteArea(string nameArea, int electorateArea, map<string,int> votesCast_){

	votingAreas[nameArea] = votingArea(nameArea, electorateArea, votesCast_);

	for(auto votesPair : votesCast_){

		bool inList = false;

		for(string party : parties){
			if(party == votesPair.first){
				inList = true;
				break;
			}
		}

		if(not inList) parties.push_back(votesPair.first);

	}

}
unique_ptr<votingArea> constituencyBase::getVoteArea(string nameArea){

	for(auto areaPair : votingAreas){

		if(areaPair.first == nameArea)
			return unique_ptr<votingArea>(new votingArea(areaPair.second));

	}

	return unique_ptr<votingArea>();

}
int constituencyBase::getVotesCast(const string& party){

	int total = 0;

	for(auto areaPair : votingAreas){

		total += areaPair.second.getVotesCast(party);

	}

	return total;

}
int constituencyBase::getVotesCast(){

	int total = 0;

	for(auto areaPair : votingAreas){

		total += areaPair.second.getVotesCast();

	}

	return total;

}
double constituencyBase::getVoteShare(const string& party){

	if(!partyContestsSeat(party)) return 0;

	double votes = getVotesCast(party);
	double totalVotes = getVotesCast();

	double share = votes/totalVotes;

	return share;

}
double constituencyBase::getTurnout(){

	double totalVotes = getVotesCast();
	double electorate_d = getElectorate();

	double turnout = totalVotes/electorate_d;

	return turnout;

}
map<string,double> constituencyBase::getVoteShareMap(){

	map<string,double> voteShareMap;

	for(auto it : parties){
		voteShareMap[it] = getVoteShare(it);
	}

	return voteShareMap;

}
map<string,int> constituencyBase::getVotesMap(){

	map<string,int> votesMap;

	for(auto it : parties){
		votesMap[it] = getVotesCast(it);
	}

	return votesMap;

}
bool constituencyBase::partyContestsSeat(const string& party){

	for(string partie : parties){
		if(party == partie) return true;
	}

	return false;

}
void constituencyBase::setPreventSwing(bool opt){
	
	for(auto& areaPair : votingAreas){

		areaPair.second.setPreventSwing(opt);

	}
}
map<string,double> constituencyBase::getSwings(unique_ptr<constituencyBase> oldResult){

	map<string,double> swings;

	map<string,double> newVoteShares = getVoteShareMap();
	map<string,double> oldVoteShares = oldResult->getVoteShareMap();

	for(auto partyVotes : newVoteShares){

		swings[partyVotes.first] = partyVotes.second - oldVoteShares[partyVotes.first];

	}

	return swings;

}