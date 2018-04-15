#include "helpers.h"
#include "constituencyBase.h"
#include "constituencyFPTP.h"
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

using namespace std;

namespace elPred{

	election loadfile(string fileName){

		ifstream inputFile(fileName);

		string line;

		unique_ptr<vector<unique_ptr<constituencyBase>>> constitVec(new vector<unique_ptr<constituencyBase>>());

		int line_no = 2;
		vector<string> partyVec;

		//get header
		getline(inputFile,line);
		istringstream header_ss(line);
		string tmpstr;

		//iterate over unimportant headers
		for(int i=0; i<6; i++){
			getline(header_ss,tmpstr,',');
		}

		while(getline(header_ss,tmpstr,',')){
			tmpstr.erase( std::remove(tmpstr.begin(), tmpstr.end(), '\r'), tmpstr.end() );
			partyVec.push_back(tmpstr);
		}
		cout<<endl;

		while (getline(inputFile, line)) {

			//read the line into a string stream
			stringstream line_ss(line);

			int area, electorate;

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

			constitVec->emplace_back(unique_ptr<constituencyBase>(new constituencyFPTP(name,MP,Country,County,area)));
			constitVec->back()->addVoteArea("",electorate,results);

			line_no++;
		}		

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

					slimmedConstitVec.push_back(std::move(constit));

				}

			}
			else{

				transform(input.begin(), input.end(), input.begin(), ::tolower);
	
				for(auto& constit : constitVec){
	
					string constitName = constit->getName();

					transform(constitName.begin(), constitName.end(), constitName.begin(), ::tolower);

					if (constitName.find(input) != std::string::npos) {
    					slimmedConstitVec.push_back(std::move(constit));
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

			string gainHold;
			if(slimmedConstitVec[i]->preventSwing()){
				if((dynamic_cast<constituencyFPTP*> (slimmedConstitVec[i].release()))->isHeld())
					gainHold = " HOLD";
				else
					gainHold = " GAIN";
			}

			cout<<"("<<i<<") "<< slimmedConstitVec[i]->getName()<<" ("<<(dynamic_cast<constituencyFPTP*> (slimmedConstitVec[i].release()))->getParty()<<gainHold<<")"<<endl;

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
					return std::move(slimmedConstitVec[tmpVal]);
				}
				continue;
			}
			
			cout<<"Input not valid, try again"<<endl;
			
		}	

		

		return unique_ptr<constituencyBase>();

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

		while(true){

			cout<<"Enter new results for "<<constit->getName()<<endl;

			map<string,int> newRes;

			for(string party : constit->partiesContestingSeat()){

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

				newRes[party] = numVotes;

			}

			cout<<endl<<"results entered were"<<endl;

			pair<string,int> winner = pair<string,int>("null",0);

			for(auto result : newRes){

				cout<<result.first<<": "<<result.second<<endl;

				if(result.second > winner.second){
					winner = result;
				}

			}

			cout<<"Y to confirm N to retry"<<endl;

			while(true){

				string input;
				getline(cin,input);

				if(input == "Y"){

					unique_ptr<constituencyFPTP> outConstit (dynamic_cast<constituencyFPTP*>(constit.release()));
					string originalWinner = outConstit->getParty();
					outConstit->getVoteArea("")->setVals(newRes);
					outConstit->setPreventSwing(true);
					outConstit->setHold(originalWinner==winner.first);

					return outConstit;
				}
				else if(input == "N")
					break;

				else
					cout<<"Invalid response, please try again"<<endl;

			}

			

		}

		return unique_ptr<constituencyBase> (new constituencyFPTP());

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