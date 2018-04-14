#include <string>
#include "constituencyBase.h"
#include "election.h"
#include <iostream>
#include <map>
#include <vector>
#include "date.h"
#include <math.h>
#include <memory>

using namespace std;

election election::addNewResult(constituencyBase &newConstit){

	unique_ptr<vector<constituencyBase>> newConstVec(new vector<constituencyBase>());

	for(auto constit : constitVec){

		if(constit.getName() == newConstit.getName()){
			newConstVec->push_back(newConstit);
			//cout<<"Adding the new one"<<endl;
		}
		else
			newConstVec->push_back(constit);

	}

	return election(std::move(newConstVec));

}

void election::init(){

	for(auto votepair : totalVotes){
		seatVec[votepair.first] = 0;
	}

	for(auto& constit : constitVec){

		electorate += constit.getElectorate();

		string winner = constit.getParty();
		
		seatVec[winner]++;

		for(string party : constit.partiesContestingSeat()){

			if(totalVotes.count(party)==1){
				totalVotes[party] += constit.getVotesCast(party);
			}
			else{
				totalVotes[party] = constit.getVotesCast(party);
			}

		}

	}
}

unique_ptr<constituencyBase> election::getConstit(int num){
	return constitVec[num];
}

unique_ptr<constituencyBase> election::getConstit(const string& name){

	for(auto& constit : constitVec){

		if(constit.getName() == name) return constit;

	}

	cout<<"Error: could not find constituency: "<<name<<endl;
	exit(1);

}

vector<constituencyBase> election::getConstitVec()const{
	return constitVec;
}

int election::numConstits(){
	return constitVec.size();
}
int election::numConstits(int area){

	int count = 0;

	for(auto& constit : constitVec){

		if(constit.getArea() == area) count++;

	}

	return count;

}

election election::swing(unique_ptr<map<int,map<string,double>>> swingVals, bool randomness){

	unique_ptr<vector<constituencyBase>> newConstVec(new vector<constituencyBase>());

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
		
		unique_ptr<map<string,double>> swingArea(new map<string,double>(swingVals->at(constit.getArea())));

		constit.swing(std::move(swingArea),randomness);
		
		newConstVec->push_back(constit);

	}

	return election(std::move(newConstVec),true);

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

		if(constit.getArea() == area)
			total += constit.getVotesCast();

	}

	return total;

}
int election::getVotes(int area, string party){

	int total = 0;

	for(auto& constit : constitVec){

		if(constit.getArea() == area)
			total += constit.getVotesCast(party);

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

	for(constituency constit : constitVec){
		if(constit.getArea() == area and constit.getParty()==party){
			++seats;
		}
	}

	return seats;
}
vector<string> election::getParties(int area){

	vector<string> parties;

	for(auto partyPair : totalVotes){

		if(area != 0)
			for(constituency constit : constitVec){

				if(constit.partyContestsSeat(partyPair.first) and area == constit.getArea()){
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