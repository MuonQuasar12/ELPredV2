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

map<string,int> constituencyPR::getSeats(map<string,int> seatPreMap){

	map<string,double> votesMap;

	for(auto votePair : getVotesMap()){

		votesMap[votePair.first] = (double) votePair.second;

	}

	map<string,int> seatsMap;

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

	while(seatsAwarded < numSeats){

		//first find party with most votes
		pair<string,double> largestShare = make_pair("null",0);

		for(auto votesPair : votesMap){

			if(votesPair.second/ (methodFactor*((double) (seatsMap[votesPair.first]+1))) > largestShare.second){
				largestShare.first = votesPair.first;
				largestShare.second = votesPair.second;
			}

		}

		++seatsMap[largestShare.first];
		++seatsAwarded;

	}

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
			cout<<it.first<<": "<<it.second<<" ("<<(100*getVoteShare(it.first))<<"%)"<<endl;
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
unique_ptr<constituencyBase> constituencyPR::clone()const{

	return unique_ptr<constituencyBase>(new constituencyPR(*this));

}