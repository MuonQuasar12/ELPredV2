#include "constituencyMMP.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

void constituencyMMP::swing(unique_ptr<map<string,double>> swingVals, bool randomness){

	constituencyBase::swing(std::move(swingVals),randomness);

	seatsMap = getSeats();

}

map<string,int> constituencyMMP::getFPTPSeats(){
	
	map<string,int> FPTPSeats;

	for(string party : parties){

		FPTPSeats[party] = 0;

	}

	for(constituencyFPTP constit : subConstits){

		++FPTPSeats[constit.getParty()];

	}

	return FPTPSeats;

}
map<string,int> constituencyMMP::getSeats(map<string,int> seatPreMap){

	map<string,int> FPTPSeats = getFPTPSeats();
	map<string,int> totalSeats = constituencyPR::getSeats(FPTPSeats);

	seatsMap = totalSeats;

	return totalSeats;

}

map<string,int> constituencyMMP::getPRSeats(){

	map<string,int> PRSeats;

	map<string,int> FPTPSeats = getFPTPSeats();
	map<string,int> totalSeats = constituencyPR::getSeats(FPTPSeats);

	for(auto seatPair : totalSeats){

		PRSeats[seatPair.first] = seatPair.second - FPTPSeats[seatPair.first];

	}

	return PRSeats;

}
void constituencyMMP::print(int opt){

	if(opt == 1){

		int seatNo = (numSeats+subConstits.size());

		cout<<"Seat of "<<getName()<<" in "<<getCounty()<<", "<<getCountry()<<endl
			<<"Turnout was "<<(100*getTurnout())<<"%"<<" at last vote, with the seat being contested by "<<partiesContestingSeat().size()<<" parties with "<<seatNo<<" being contested" <<endl;

		cout<<"The FPTP seats are held by"<<endl;
		for(constituencyFPTP constit : subConstits){
			cout<<constit.getName()<<" "<<constit.getParty()<<endl;
		}

		cout<<"The PR seats are held by: ";
		for(auto seatPair : getPRSeats()){

			cout<<seatPair.first<<" ("<<seatPair.second<<"), ";

		}
		cout<<endl;

	}
	else if(opt	== 2){
		cout<<"Seat of "<<getName()<<" FPTP seat results: "<<endl;

		for(constituencyFPTP constit : subConstits){
			constit.print(2);
		}

		cout<<"PR results:"<<endl;
		constituencyPR::print(2);

	}
	else if(opt == 3){
		cout<<"Country: "<<country<<", county: "<<county<<", seat: "<<name;
		cout<<"FPTP seats:"<<endl;
		for(constituencyFPTP subConstit : subConstits){
			subConstit.print(3);
		}
		cout<<"PR seats: "<<endl;
		constituencyPR::print(3);

		cout<<endl;
	}
}