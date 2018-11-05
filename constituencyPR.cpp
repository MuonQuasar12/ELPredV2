#include "constituencyPR.h"
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

using namespace std;

void constituencyPR::swing(unique_ptr<map<string,double>> swingVals, bool randomness){

	constituencyBase::swing(std::move(swingVals),randomness);

	seatsMap = getSeats();

}
int constituencyPR::getNumSeats(string party){

	if(seatsMap.empty()){
		getSeats();
	}

	return seatsMap[party];

}
map<string,int> constituencyPR::getSeats(map<string,int> seatPreMap){

	map<string,double> votesMap;

	//cout<<"Integer votes map is size "<<getVotesMap().size()<<endl;

	for(auto votePair : getVotesMap()){

		votesMap[votePair.first] = (double) votePair.second;

	}

	//cout<<"Size of vote area map = "<<votingAreas.size()<<endl;
	//cout<<"Size of first vote area map = "<<votingAreas.begin()->second.getVotesMap().size()<<endl;

	if(seatPreMap == null_map()){
		for(auto votesPair : votesMap){
	
			seatsMap[votesPair.first] = 0;
	
		}
	}
	else{
		seatsMap = seatPreMap;
	}

	int seatsAwarded = 0;

	double methodFactor;

	if(system == elPred::countSys::DHONDT) methodFactor = 1;
	else if(system == elPred::countSys::STLAGUE) methodFactor = 2;

	//cout<<"Awarding seats"<<endl;

	while(seatsAwarded < numSeats){

		//cout<<"Awarded "<<seatsAwarded<<"/"<<numSeats<<endl;

		//first find party with most votes
		pair<string,double> largestShare = make_pair("null",0);

		for(auto votesPair : votesMap){

			if(votesPair.first == "OTH") continue; //do not award seats to others

			double partyVote = votesPair.second;
			double divisor = methodFactor*((double) (seatsMap[votesPair.first]+1));

			//cout<<" 	Checking "<<votesPair.first<<" votes = " << partyVote << ", divisor = "<<divisor<< ", divided votes = "<<(partyVote/divisor) << endl;

			if((partyVote/divisor) > largestShare.second){
				largestShare.first = votesPair.first;
				largestShare.second = votesPair.second/divisor;
			}

			//cout<<"Largest share now "<< largestShare.first << " with " << largestShare.second << endl;

		}

		//cout<<"Awarding seat to "<<largestShare.first<<endl;

		++seatsMap[largestShare.first];
		++seatsAwarded;

	}

	//cout<<"Awarded "<<seatsAwarded<<"/"<<numSeats<<endl;

	return seatsMap;

}
void constituencyPR::print(int opt){
	if(opt == 1){

		cout<<"Seat of "<<getName()<<" in "<<getCounty()<<", "<<getCountry()<<endl
			<<"Turnout was "<<(100*getTurnout())<<"%"<<" at last vote, with the seat being contested by "<<partiesContestingSeat().size()<<" parties with "<<numSeats<<" being contested" <<endl;

		cout<<"The seats are held by: ";
		for(auto seatPair : seatsMap){

			cout<<seatPair.first<<" ("<<seatPair.second<<"), ";

		}
		cout<<endl;
	
	}
	if(opt == 2){
		cout<<"Seat of "<<getName()<<" results"<<endl;
		for(auto it : getVotesMap()){
			cout<<it.first<<": "<<it.second<<" ("<<(round(1000*getVoteShare(it.first))/10)<<"%), "<< seatsMap[it.first] << "(" << (seatsMap[it.first]-initSeats[it.first]) << ")" << " seats " <<endl;
		}
	}
	if(opt == 3){

		map<string,int> seatDiff;
		for(auto seatPair : seatsMap){

			seatDiff[seatPair.first] = seatPair.second - initSeats[seatPair.first];

		}

		cout<<"Country: "<<country<<", county: "<<county<<", seat: "<<name;

		for(auto seatPair : seatDiff){

			string sign;
			if(seatPair.second > 0) sign = "+";
			else if (seatPair.second < 0) sign = "-";

			cout<<seatPair.first<<" "<<sign<<abs(seatPair.second)<<endl;

		}
		
	}
}
string constituencyPR::lineInfo(){

	map<string,int> diffMap;

	for(auto seatPair : initSeats){

		int change = seatsMap[seatPair.first] - seatPair.second;

		if(change != 0){
			diffMap[seatPair.first] = change;
		}

	}

	string outStr = (name + " (");

	for(auto diffPair : diffMap){

		string sign;
		if(diffPair.second > 0) sign = "+";
		else sign = "";

		outStr += (diffPair.first + ": "+sign+to_string(diffPair.second)+" ");

	}

	outStr += ")";

	return outStr;

}

unique_ptr<constituencyBase> constituencyPR::clone()const{

	return unique_ptr<constituencyBase>(new constituencyPR(*this));

}

bool constituencyPR::changedHands(){

	for(auto seatPair : initSeats){

		if(seatPair.second != seatsMap[seatPair.first]) return true;

	}

	return false;

}