#include <string>
#include "constituencyBase.h"
#include "constituencyFPTP.h"
#include "election.h"
#include <iostream>
#include <map>
#include <vector>
#include "date.h"
#include <math.h>
#include <memory>
#include <ciso646>

using namespace std;

election election::addNewResult(unique_ptr<constituencyBase> newConstit){

	unique_ptr<vector<unique_ptr<constituencyBase>>> newConstVec(new vector<unique_ptr<constituencyBase>>());

	for(auto& constit : constitVec){

		if(constit->getName() == newConstit->getName()){
			newConstVec->push_back(newConstit->clone());
			//cout<<"Adding the new one"<<endl;
		}
		else
			newConstVec->push_back(constit->clone());

	}

	return election(newConstVec);

}

void election::init(){

	//cout<<"Beginning election::init"<<endl;

	for(auto votepair : totalVotes){
		seatVec[votepair.first] = 0;
	}

	//cout<<"Empty seat vector created, iterating over constituencies"<<endl;

	//cout<<"Constit vec has "<<constitVec.size()<<endl;
	//constitVec[1]->print(4);
	//cout<<"Const name = "<<constitVec[0]->getName()<<endl;

	for(auto& constit : constitVec){

		//cout<<constit->getName()<<": ";
		electorate += constit->getElectorate();

		//cout<<"Totaling votes";
		for(string party : constit->partiesContestingSeat()){

			//cout<<party<<",";

			if(totalVotes.count(party)==1){
				totalVotes[party] += constit->getVotesCast(party);
				//cout<<"(rep)";
			}
			else{
				totalVotes[party] = constit->getVotesCast(party);
				//cout<<"(new)"<<endl;
			}

		}

	}

	vector<string> parties = this->getParties();

	for (auto& constit : constitVec) {
		for (string party : parties) {
			seatVec[party] += constit->getNumSeats(party);
		}
	}

	//cout<<"Finished election::init"<<endl;
}

unique_ptr<constituencyBase> election::getConstit(int num){
	return constitVec[num]->clone();
}

unique_ptr<constituencyBase> election::getConstit(const string& name){

	for(auto& constit : constitVec){

		if(constit->getName() == name) return constit->clone();

	}

	cout<<"Error: could not find constituency: "<<name<<endl;
	exit(1);

}

vector<unique_ptr<constituencyBase>> const& election::getConstitVec() const{
	return constitVec;
}

int election::numConstits(){
	return constitVec.size();
}
int election::numConstits(int area){

	int count = 0;

	for(auto& constit : constitVec){

		if(constit->getArea() == area) count++;

	}

	return count;

}

election election::swing(unique_ptr<map<int,map<string,double>>> swingVals, bool randomness){

	unique_ptr<vector<unique_ptr<constituencyBase>>> newConstVec(new vector<unique_ptr<constituencyBase>>());

	//iterate over areas
	for(auto it = swingVals->begin(); it != swingVals->end(); it++){
	
		//check swing vals sum to 0

		double sum = 0;
	
		for(auto vals : it->second){
			sum += vals.second;
		}
	
		if(round(10000*sum) != 0){
			it->second["OTH"] = -sum;
		}
	
		//cout<<"OTH will swing by "<<sum<<endl;
		
	}

	for(auto& constit : constitVec){
		
		unique_ptr<map<string,double>> swingArea(new map<string,double>(swingVals->at(constit->getArea())));

		unique_ptr<constituencyBase> constitCopy = constit->clone();

		constitCopy->swing(std::move(swingArea),randomness);
		
		newConstVec->push_back(std::move(constitCopy));

	}

	return election(newConstVec,true);

}

int election::getVotes(){
	int total = 0;

	for(auto it : totalVotes){
		total += it.second;
	}

	return total;

}
int election::getVotes(string party){

	return totalVotes[party];

}
int election::getVotes(int area){

	int total = 0;

	for(auto& constit : constitVec){

		if(constit->getArea() == area)
			total += constit->getVotesCast();

	}

	return total;

}
int election::getVotes(int area, string party){

	int total = 0;

	for(auto& constit : constitVec){

		if(constit->getArea() == area)
			total += constit->getVotesCast(party);

	}

	return total;

}

double election::getVoteShare(string party){

	double total = getVotes();
	double partyShare = totalVotes[party];

	return partyShare/total;

}
int election::getSeats(string party){
	return seatVec[party];
}
int election::getSeats(const string& party, int area){
	int seats = 0;

	for(auto& constit : constitVec){
		if(constit->getArea() == area){
			seats += constit->getNumSeats(party);
		}
	}

	return seats;
}
vector<string> election::getParties(int area){

	vector<string> parties;

	for(auto partyPair : totalVotes){

		if(area != 0)
			for(auto& constit : constitVec){

				if(constit->partyContestsSeat(partyPair.first) and area == constit->getArea()){
					parties.push_back(partyPair.first);
					break;
				}
				
			}

		else
			parties.push_back(partyPair.first);

	}

	return parties;

}
void election::print(int option){
	cout<<"Result of election"<<endl
		<<"Seats: "<<endl;
	for(auto it : seatVec){
		cout<<it.first<<": "<<it.second<<endl;
	}
	if(option == 2){
		cout<<"Votes: "<<endl;
		for(auto it : totalVotes){
			cout<<it.first<<": "<<(getVoteShare(it.first)*100)<<"%"<<endl;
		}
	}
}

int election::getElectorate(){
	return electorate;
}
double election::getTurnout(){
	double voters = electorate;
	double total = getVotes();

	return total/voters;

}
date election::getDate() const{
	return Date;
}
bool election::isProj(){
	return isProj_;
}