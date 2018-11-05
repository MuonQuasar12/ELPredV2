#include "constituencyPAL.h"
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <tuple>

void constituencyPAL::setPreventSwing(int seg, bool opt) {

	if (seg != 0)
		votingAreas[to_string(seg)].setPreventSwing(opt);
	else
		constituencyBase::setPreventSwing(opt);

}
void constituencyPAL::setElSeg(int elSeg){

	segmentUpForEl = elSeg;

	for(auto& vt : votingAreas){

		if(vt.first == to_string(elSeg)) vt.second.setPreventSwing(false);
		else vt.second.setPreventSwing(true);

	}

}
int constituencyPAL::getNumSeats(string party){

	if(seatsMap.empty()){
		getSeats();
	}

	return seatsMap[party];

}
map<string, double> constituencyPAL::getVoteShareMap() {

	if (areaOverrides.size() == 0)
		return constituencyBase::getVoteShareMap();

	votingArea tmpArea;

	for (auto vtPair : areaOverrides) {
		tmpArea += vtPair.second;
	}
	
	return tmpArea.getVoteShareMap();
}
void constituencyPAL::addVoteArea(string nameArea, int electorateArea, map<string,int> votesCast_){

	++numSeats;
	hold[nameArea] = false;

	constituencyBase::addVoteArea(nameArea,electorateArea,votesCast_);

	initSeats = getSeats();
	seatsMap = initSeats;

}
void constituencyPAL::swing(unique_ptr<map<string, double>> swingVals, bool randomness) {

	if (swingFromAllBool) {

		votingArea tmpVt (votingAreas[to_string(segmentUpForEl)]);

		for (string party : parties) {

			int totalVotes = 0;

			for (auto vtAreaPair : votingAreas) {

				totalVotes += vtAreaPair.second.getVotesCast(party);

			}

			totalVotes /= numSeats;

			tmpVt.setVals(party, totalVotes);

		}

		if (areaOverrides.find(to_string(segmentUpForEl)) == areaOverrides.end()) {
			areaOverrides[to_string(segmentUpForEl)] = tmpVt;
		}
	}

	unique_ptr<map<string, double>> overrideSwings(new map<string, double>(*swingVals));

	constituencyBase::swing(std::move(swingVals), randomness);

	for (auto& areaPair : areaOverrides) {

		areaPair.second.swing(std::move(overrideSwings));

	}

	seatsMap = getSeats();

}
void constituencyPAL::addNewResult(string voteAreaName, map<string, int> results) {

	 votingArea tmp = votingAreas[voteAreaName];
	 tmp.setVals(results);
	 tmp.setPreventSwing(true);

	 areaOverrides[voteAreaName] = tmp;

}

map<string, int> constituencyPAL::getSeats() {

	vector<map<string, int>> voteMap;
	seatHolders.clear();
	seatHolders.push_back("0");

	for (auto vt : votingAreas) {

		voteMap.push_back(vt.second.getVotesMap());

	}

	seatsMap.clear();
	for (string party : parties) {
		seatsMap[party] = 0;
	}
	
	vector<pair<string, int>> topN;

	//fill results for last block vote
	while (topN.size() < (numSeats-areaOverrides.size())) {

		pair<string, int> tmp = make_pair("null", 0);

		for (auto tmpMap : voteMap) {

			for (auto tmpPair : tmpMap) {

				bool cont = false;
				for (auto checkPair : topN) {
					if (tmpPair.first == checkPair.first && tmpPair.second == checkPair.second) {
						cont = true;
						break;
					}
				}
				if (cont) continue;

				if (tmpPair.second > tmp.second) {
					tmp = tmpPair;
				}

			}
		}

		topN.push_back(tmp);

		if (tmp.first == "null") {
			cerr << "Error in constituencyPAL::getSeats: null returned as a winner" << endl;
			exit(1);
		}

		seatHolders.push_back(tmp.first);
		++seatsMap[tmp.first];

	}
	//now for last non-block vote
	for (auto areaPair : areaOverrides) {

		pair<string, int> tmp = make_pair("null", 0);

		for (auto pair : areaPair.second.getVotesMap()) {

			if (pair.second > tmp.second) {
				tmp = pair;
			}

		}
		++seatsMap[tmp.first];
		seatHolders.push_back(tmp.first);
	}

	return seatsMap;

}
string constituencyPAL::lineInfo(){

	map<string,int> diffMap;

	for(auto seatPair : initSeats){

		int change = seatsMap[seatPair.first] - seatPair.second;

		if(change != 0){
			diffMap[seatPair.first] = change;
		}

	}

	string outStr = (name + " (");

	if(segmentUpForEl == 0){		

		for(auto diffPair : diffMap){

			string sign;
			if(diffPair.second > 0) sign = "+";
			else sign = "";

			outStr += (diffPair.first + ": "+sign+to_string(diffPair.second)+" ");

		}
		if (diffMap.size() == 0) {
			outStr += "no change";
		}

		outStr += ")";

		return outStr;

	}
	else{

		string loser, winner;

		for(auto diffPair : diffMap){

			if(diffPair.second == 1) winner = diffPair.first;
			else if (diffPair.second == -1) loser = diffPair.first;

		}

		if( loser == winner ){

			outStr += (seatHolders[segmentUpForEl]+" HOLD)");

		}
		else{

			outStr += (winner+" GAIN from "+loser+")");

		}

		return outStr;

	}

}

void constituencyPAL::print(int opt) {
	if (opt == 1) {

		cout << "Seat of " << getName() << " in " << getCounty() << ", " << getCountry() << endl
			<< "Turnout was " << (100 * getTurnout()) << "%" << " at last vote, with the seat being contested by " << partiesContestingSeat().size() << " parties with " << numSeats << " being contested" << endl;

		cout << "The seats are held by: ";
		for (auto seatPair : seatsMap) {

			cout << seatPair.first << " (" << seatPair.second << "), ";

		}
		cout << endl;

	}
	if (opt == 2) {
		cout << "Seat of " << getName() << " results" << endl;
		for (auto it : getVotesMap()) {
			cout << it.first << ": " << it.second << " (" << (100 * getVoteShare(it.first)) << "%)" << endl;
		}
	}
	if (opt == 3) {

		map<string, int> seatDiff;
		for (auto seatPair : seatsMap) {

			seatDiff[seatPair.first] = seatPair.second - initSeats[seatPair.first];

		}

		cout << "Country: " << country << ", county: " << county << ", seat: " << name;

		for (auto seatPair : seatDiff) {

			string sign;
			if (seatPair.second > 0) sign = "+";
			else if (seatPair.second < 0) sign = "-";

			cout << seatPair.first << " " << sign << abs(seatPair.second) << endl;

		}

	}
}
unique_ptr<constituencyBase> constituencyPAL::clone()const {

	return unique_ptr<constituencyBase>(new constituencyPAL(*this));

}
bool constituencyPAL::changedHands(){

	for(auto seatPair : initSeats){

		if(seatPair.second != seatsMap[seatPair.first]) return true;

	}

	return false;

}