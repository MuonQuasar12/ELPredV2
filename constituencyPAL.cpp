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
void constituencyPAL::swing(unique_ptr<map<string, double>> swingVals, bool randomness) {

	constituencyBase::swing(std::move(swingVals), randomness);

	seatsMap = getSeats();

}
map<string, int> constituencyPAL::getSeats() {

	vector<map<string, int>> voteMap;

	for (auto vt : votingAreas) {

		voteMap.push_back(vt.second.getVotesMap());

	}

	seatsMap.clear();
	for (string party : parties) {
		seatsMap[party] = 0;
	}
	
	vector<pair<string, int>> topN;
	

	while (topN.size() < numSeats) {

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

		++seatsMap[tmp.first];

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