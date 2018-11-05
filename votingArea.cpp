#include <memory>
#include <random>
#include "votingArea.h"
#include <chrono>
#include <ciso646>

using namespace std;

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
				//cout<<it->first<<" swings by "<<(100*swingMap[it->first]/normFactor)<<" %"<<endl;
			}
			else {
				//cout << "Seat not contested by " << it->first << endl;
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

void votingArea::setVals(string party, int votes) {

	votesCast[party] = votes;

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

		if(oldVoteShares.find(partyVotes.first) == oldVoteShares.end() ) continue; //probably not the best way to deal with parties not previously having stood

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
void votingArea::print() {

	cout << "Results for voting area: " << name << endl;
	for (auto votePair : votesCast) {

		cout << votePair.first << ": " << votePair.second << endl;

	}

}

vector<string> votingArea::getPartyList(){

	vector<string> parties;
	for(auto vtpair : votesCast){

		parties.push_back(vtpair.first);

	}

	return parties;

}

votingArea votingArea::operator+(const votingArea& vt){

	votingArea outVt;

	outVt.name = "sum";
	outVt.electorate += vt.electorate;
	
	outVt.doNotSwing = true;
	outVt.votesCast = votesCast;

	for(auto vtPair : vt.votesCast){

		if(outVt.votesCast[vtPair.first])
			outVt.votesCast[vtPair.first] += vtPair.second;
		else
			outVt.votesCast[vtPair.first] = vtPair.second;
	}

	return outVt;

}
votingArea& votingArea::operator+=(const votingArea& vt){

	*this = *this + vt;

	return *this;

}