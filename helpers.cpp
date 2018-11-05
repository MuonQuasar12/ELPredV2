#include "helpers.h"
#include "constituencyBase.h"
#include "constituencyFPTP.h"
#include "constituencyPAL.h"
#include "constituencyPR.h"
#include "votingArea.h"
#include "election.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <utility>
#include <math.h>
#include <memory>
#include <cctype>

using namespace std;

namespace elPred{

	election loadfile(string fileName, string type){

		ifstream inputFile(fileName);
		if (!inputFile.is_open()) {
			cerr << "Error: could not open file" << endl;
			//exit(1);
		}

		string line;

		//cout<<"Creating constitVec pointer"<<endl;

		unique_ptr<vector<unique_ptr<constituencyBase>>> constitVec(new vector<unique_ptr<constituencyBase>>());

		int line_no = 2;
		vector<string> partyVec;

		//get header
		getline(inputFile,line);
		istringstream header_ss(line);
		string tmpstr;

		//iterate over unimportant headers

		int maxHeaders = 0; //column at which party names begin
		if(type == "PR"){
			maxHeaders = 7;
		}
		else if(type == "FPTP"){
			maxHeaders = 6;
		}

		for(int i=0; i<maxHeaders; i++){
			getline(header_ss,tmpstr,',');
		}

		while(getline(header_ss,tmpstr,',')){
			tmpstr.erase( std::remove(tmpstr.begin(), tmpstr.end(), '\r'), tmpstr.end() );
			partyVec.push_back(tmpstr);
		}
		cout<<endl;

		cout<<"Opening file"<<endl;

		while (getline(inputFile, line)) {

			//read the line into a string stream
			stringstream line_ss(line);

			int area, electorate, nSeats;

			map<string,int> results;

			string temp_entry;

			//get the first entry. This is comma delimiter
			getline(line_ss, temp_entry, ',');
			string name(temp_entry);
			getline(line_ss, temp_entry, ',');
			string MP(temp_entry);
			getline(line_ss, temp_entry, ',');
			string Country(temp_entry);
			getline(line_ss, temp_entry, ',');
			try{
				area = stoi(temp_entry);
			}
			catch(std::invalid_argument){
				cout<<"Error, could not convert area value"<<endl;
				cout<<"Temp entry: "<<temp_entry<<endl;
				cout<<"Line: "<<line_no<<endl;
				exit(1);
			}
			getline(line_ss, temp_entry, ',');
			string County(temp_entry);

			if(type == "PR"){
				getline(line_ss, temp_entry, ',');
				try{
					nSeats = stoi(temp_entry);
				}
				catch(std::invalid_argument){
					cout<<"Error, could not convert nSeats value"<<endl;
					cout<<"Temp entry: "<<temp_entry<<endl;
					cout<<"Line: "<<line_no<<endl;
					exit(1);
				}
			}

			getline(line_ss, temp_entry, ',');
			try{
				electorate = stoi(temp_entry);
			}
			catch(std::invalid_argument){
				cout<<"Error, could not convert electorate value"<<endl;
				cout<<"Temp entry: "<<temp_entry<<endl;
				cout<<"Line: "<<line_no<<endl;
				exit(1);
			}

			int counter = 0;
			
			while(getline(line_ss,temp_entry,',')){

				string tmptmp(temp_entry);				

				if(tmptmp.length()>1){
					try{
						results[partyVec[counter]] = stoi(temp_entry);
					}
					catch(std::invalid_argument){
						cout<<"Error, could not convert result"<<endl;
						cout<<"Temp entry: "<<temp_entry<<endl;
						cout<<"Counter: "<<counter<<endl;
						cout<<"Party: "<<partyVec[counter]<<endl;
						cout<<"Line: "<<line_no<<endl;
						exit(1);
					}
				}
				counter++;
			}

			if(type == "FPTP"){
				constitVec->emplace_back(unique_ptr<constituencyBase>(new constituencyFPTP(name,MP,Country,County,area)));
				constitVec->back()->addVoteArea("",electorate,results);
			}
			else if(type == "PAL"){

				if(constitVec->empty() || constitVec->back()->getName() != name){
					constitVec->emplace_back(unique_ptr<constituencyBase>(new constituencyPAL(name,Country,County,area)));
					constitVec->back()->addVoteArea("1",electorate,results);
					
					(dynamic_cast<constituencyPAL*>(constitVec->back().get()))->swingFromAll(true);
				}
				else{
					int numVt = constitVec->back()->getNumVotingAreas();
					++numVt;
					constitVec->back()->addVoteArea(to_string(numVt),electorate,results);
					if(numVt == 3) (dynamic_cast<constituencyPAL*>(constitVec->back().get()))->setElSeg(3);
				}
				
			}
			else if(type == "PR"){

				if(nSeats > 1)
					constitVec->emplace_back(unique_ptr<constituencyBase>(new constituencyPR(name,Country,County,area,countSys::DHONDT,nSeats)));
				else
					constitVec->emplace_back(unique_ptr<constituencyBase>(new constituencyFPTP(name,MP,Country,County,area)));
				
				constitVec->back()->addVoteArea("",electorate,results);

			}

			constitVec->back()->init();

			line_no++;
		}		

		//cout<<"returning election object"<<endl;

		return election(std::move(constitVec));

	}

	unique_ptr<constituencyBase> constitSearch(election el){
			
		vector<unique_ptr<constituencyBase>> constitVec;

		for(int i = 0 ; i < el.numConstits() ; i++ ){
			constitVec.push_back(std::move(el.getConstit(i)));
		}
	
		vector<unique_ptr<constituencyBase>> slimmedConstitVec;

		while(true){

			cout<<"Enter part (or all) of the constituency's name"<<endl;
			cout<<"Or type ALL to list all"<<endl;


			string input;
			getline(cin,input);


			if(input == "ALL"){
			
				slimmedConstitVec.clear();
				for(auto& constit : constitVec){

					slimmedConstitVec.push_back(constit->clone());

				}

			}
			else{

				transform(input.begin(), input.end(), input.begin(), ::tolower);
	
				for(auto& constit : constitVec){
	
					string constitName = constit->getName();

					transform(constitName.begin(), constitName.end(), constitName.begin(), ::tolower);

					if (constitName.find(input) != std::string::npos) {
    					slimmedConstitVec.push_back(constit->clone());
					}
	
				}
			}

			bool list = true;

			if(slimmedConstitVec.size()>30)
				while(true){
					cout<<"Warning, about to list "<<slimmedConstitVec.size()<<" constituencies, continue? (y/n)"<<endl;
					getline(cin,input);
					if(input == "y") {
						list = true;
						break;
					}
					else if(input == "n"){
						list = false;
						break;
					}

				}
			else if(slimmedConstitVec.size()==0){
				cout<<"None found, try again"<<endl;
				continue;
			}

			if(!list) {
				slimmedConstitVec.clear();
				continue;
			}
			else{
				break;
			}

		}

		bool isProj = el.isProj();

		for(int i = 0; i<slimmedConstitVec.size(); i++){

			cout<<"("<<i<<") "<< slimmedConstitVec[i]->lineInfo() << endl;

		}

		while(true){

			cout<<"Enter the number of the constituency to select it, or E to exit"<<endl;

			string input;
			getline(cin,input);

			int tmpVal;

			if(input == "E"){
				break;
			}

			else if(stringstream(input)>>tmpVal){
				
				if(tmpVal >= slimmedConstitVec.size() || tmpVal<0);

				else
				{
					cout<<"Returning the constit"<<endl;
					return slimmedConstitVec[tmpVal]->clone();
				}
				continue;
			}
			
			cout<<"Input not valid, try again"<<endl;
			
		}	

		return nullptr;

	}

	map<int,map<string,double>> generateSwingMap(map<vector<int>,map<string,double>> rawSwings, election el){

		//Some polls may contain overlapping areas, e.g. London & England, this adjusts for that
		//Input map <vector of areas covered, map of swings per party>

		map<int,map<string,double>> outMap;
		map<int,bool> areaDone;
		map<vector<int>,double> areaPopulations; 

		//count number of times area appears
		map<int,int> areaInstances;

		for(auto Pair : rawSwings){

			for(int area : Pair.first){

				areaInstances[area]++;
				areaDone[area] = false;
				areaPopulations[Pair.first] += el.getVotes(area);

			}

		}

		//DEFINITIONS
		//major area = larger area containing the smaller area (e.g. England)
		//minor area = the smaller one (e.g. London)
		//minor areas are those which are double counted
		//majors are more difficult to find

		//iterate of minor areas and find the major areas containing them
		//assume minor areas are only 1 in size
		//Can't handle nesting of >2

		for(auto areaPair : areaInstances){

			if(areaPair.second > 1){

				cout<<"Major area found"<<endl;

				areaDone[areaPair.first] = true;

				vector<int> areatmp = {areaPair.first};

				outMap[areaPair.first] = rawSwings[areatmp];

				//now find corresponding major area

				for(auto Pair : rawSwings){

					if(Pair.first.size() < 2) continue;

					//check if this is the major area
					//iterate over all the areas covered
					bool isMajorArea = false;

					for(int area : Pair.first){

						if(area == areaPair.first) {
							isMajorArea = true;
							break;
						}

					}

					if(!isMajorArea) continue;

					for(int area : Pair.first){
						if(areaDone[area]) continue; //skip the minor area

						//iterate over parties
						for(auto swingPair : Pair.second){
							outMap[area][swingPair.first] = ((areaPopulations[Pair.first]*swingPair.second)-(areaPopulations[areatmp]*rawSwings[areatmp][swingPair.first]))/abs(areaPopulations[Pair.first]-areaPopulations[areatmp]);
							areaDone[area] = true;
						}
					}

					break;

				}

			}

		}


		

		for(auto Pair : rawSwings){
	
			for(int area : Pair.first){
				if(!areaDone[area])	{	
					//cout<<"Not yet done area "<<area<<", doing now"<<endl;
					outMap[area] = Pair.second;
				}
			}
		}
		

		return outMap;

	}

	unique_ptr<constituencyBase> enterNewResults(unique_ptr<constituencyBase> constit){

		unique_ptr<constituencyBase> emptyPtr = nullptr;

		string vtAreaName;

		cout<<"Please choose a voting area by entering its name, or E to exit"<<endl;
		while(true && constit->getNumVotingAreas() > 1){

			cout<<"Voting areas:"<<endl;
			for(auto vt : constit->getVotingAreas()){

				cout<<vt.first<<endl;

			}
			cout<<"Choice: ";

			string input;
			getline(cin,input);

			if(input == "E")
				return emptyPtr;
			else{

				for(auto vt : constit->getVotingAreas()){

					if(vt.first == input) {vtAreaName = input; break;}

				}

				if(vtAreaName.empty()){
					cout<<input<<" is not valid, please try again"<<endl;
					continue;
				}
				else break;

			}

		}

		while(true){

			cout<<"Enter new results for "<<constit->getName()<<", area "<<vtAreaName<<endl;

			map<string,int> newRes;

			for(string party : constit->partiesContestingSeat()){

				if(party == "OTH") continue;
				int numVotes;

				while(true){
					string inputString;
		
					cout<<party<<": ";
					getline(cin,inputString);
					stringstream inStream(inputString);

					if(inStream>>numVotes)
						break;

					cout<<"Input invalid, please try again"<<endl;

				}

				if(numVotes > 0)
					newRes[party] = numVotes;

			}

			while(true){

				cout<<"Add additional parties (enter to skip)"<<endl;

				string party;
				
				cout<<"Enter party name/abbreviation, enter then number of votes:"<<endl;

				getline(cin,party);
				stringstream inStream(party);

				party.erase(remove(party.begin(),party.end(),' '),party.end());

				if(party == "") break;

				cout<<party<<": ";

				int numVotes;

				string inputString;
				getline(cin,inputString);
				stringstream inStreamVT(inputString);

				if(inStreamVT>>numVotes){

					newRes[party] = numVotes;
					continue;

				}

				cout<<"Input invalid, please try again"<<endl;

			}

			int numVotesOTH;

			while(true){
				string inputString;
	
				cout<<"OTH: ";
				getline(cin,inputString);
				stringstream inStream(inputString);

				if(inStream>>numVotesOTH)
					break;

				cout<<"Input invalid, please try again"<<endl;

			}

			if(numVotesOTH > 0)
				newRes["OTH"] = numVotesOTH;

			cout<<endl<<"results entered were"<<endl;

			for(auto result : newRes){

				cout<<result.first<<": "<<result.second<<endl;

			}

			cout<<"Y to confirm N to retry"<<endl;

			while(true){

				string input;
				getline(cin,input);

				if(input == "Y"){

					unique_ptr<constituencyBase> outConstit = constit->clone();

					outConstit->addNewResult(vtAreaName, newRes);
					//outConstit->swing(std::move(constit));

					return std::move(outConstit);
				}
				else if(input == "N")
					break;

				else
					cout<<"Invalid response, please try again"<<endl;

			}

			

		}

		return emptyPtr;

	}

	double adjustedSigma(int declaredConstits, int totalConstits, double initialSigma){

		double sigma;

		if(declaredConstits == 1) sigma = initialSigma;

		else{
			double A = 0.02119506;
			double b = 0.564737057;
			double x = (double) declaredConstits / (double) totalConstits;
	
			sigma= initialSigma*A*exp(sqrt(-log(x))/b);
		}

		return sigma;

	}

	void meanSD(unique_ptr<vector<int>> data, double& mean, double& SD)
	{
	    double sum = 0.0;
	    mean = 0;
	    SD = 0.0;

	    double nPoints = (double) data->size();

	    for(int i = 0; i < nPoints; ++i)
	    {
	        sum += (double) data->at(i);
	    }

	    mean = sum/nPoints;

	    for(int i = 0; i < nPoints; ++i)
	        SD += pow((double) data->at(i) - mean, 2);

	    SD = sqrt(SD / nPoints);
	}

}