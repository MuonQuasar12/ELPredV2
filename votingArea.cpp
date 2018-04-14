#include <memory>
#include <random>
#include "votingArea.h"

using namespace std;

normal_distribution<double> votingArea::dist;

void votingArea::swing(unique_ptr<map<string,double>> swingVals, bool randomness){

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator (seed);

	if(!doNotSwing){
	
		map<string,double> swingMap;
		double normFactor = getVotesCast();
	
		//get swing in terms of votes

		double totalRand = 0;

		for(auto it = swingVals->begin(); it != swingVals->end(); it++){
			if(partyContestsSeat(it->first)){

				double randSwing = 0;

				if(randomness and next(it) != swingVals->end()){
					randSwing = votingArea::dist(generator);
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
	}
}

int votingArea::getVotesCast(){

	int total = 0;

	for(auto votepair : votesCast){
		total += votepair.second;
	}

	return total;

}
double votingArea::getVoteShare(const string& party){

	if(!partyContestsSeat(party)) return 0;

	double votes = votesCast[party];
	double totalVotes = getVotesCast();

	double share = votes/totalVotes;

	return share;

}
double votingArea::getTurnout(){
	double totalVotes = getVotesCast();
	double electorate_d = electorate;

	double turnout = totalVotes/electorate_d;

	return turnout;
}
map<string,double> votingArea::getVoteShareMap(){
	map<string,double> voteShareMap;

	for(auto it : votesCast){
		voteShareMap[it.first] = getVoteShare(it.first);
	}

	return voteShareMap;
}
map<string,double> votingArea::getSwing(votingArea oldResult){

	map<string,double> swings;

	map<string,double> newVoteShares = getVoteShareMap();
	map<string,double> oldVoteShares = oldResult.getVoteShareMap();

	for(auto partyVotes : newVoteShares){

		swings[partyVotes.first] = partyVotes.second - oldVoteShares[partyVotes.first];

	}

	return swings;
}
bool votingArea::partyContestsSeat(const string& party){
	for(auto it : votesCast){
		if(party == it.first) return true;
	}
	return false;
}