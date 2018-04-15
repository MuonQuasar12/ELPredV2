#include <vector>
#include <iostream>
#include <fstream>
#include "election.h"
#include "constituencyBase.h"
#include "constituencyFPTP.h"
#include "elProj.h"
#include "helpers.h"
#include <sstream>
#include <ctime>
#include <chrono>
#include <memory>
#include <algorithm>

using namespace std;

election elProj::getLatest(){

	return projectionList.back();

}

void elProj::addNewResult(){

	unique_ptr<constituencyBase> constit = std::move(elPred::constitSearch(oldElWDec));

	if(constit->getName() == "null") return;

	unique_ptr<constituencyFPTP> newConstit (dynamic_cast<constituencyFPTP*> (elPred::enterNewResults(std::move(constit)).get()));

	if(newConstit->getParty()== "null") return;

	election tmpEl = std::move(oldElWDec);

	oldElWDec = std::move(tmpEl.addNewResult(std::move(newConstit)));

	//check if result has already been added
	for(int i =0; i<setConstits.size(); i++){

		if(setConstits[i].first->getName() == newConstit->getName()){

			setConstits[i] = make_pair(std::move(constit),std::move(newConstit));

			return;
		}

	}	

	setConstits.push_back(make_pair(std::move(constit),std::move(newConstit)));

	cout<<"Checking options"<<endl;

	if(autoProject) project();
	if(autoSim) project(true);
	if(outNameSet){
		if(autoProject)
			writeProjResults();
		if(autoSim)
			writeSimResults();
	}

}

void elProj::removeResult(){

	cout<<"Enter a constituency's number to remove it, E to exit"<<endl;

	for(int i = 0; i < setConstits.size(); i++){

		unique_ptr<constituencyFPTP> FPTPconstit (dynamic_cast<constituencyFPTP*> (setConstits[i].second.get()));

		string holdGain;
		if(FPTPconstit->isHeld())
			holdGain = " HOLD";
		else 
			holdGain = " GAIN";

		cout<<"("<<i<<") "<<setConstits[i].first->getName()<<" ("<<FPTPconstit->getParty()<<holdGain<<")"<<endl;

	}

	string input;
	int choice;

	while(true){

		cout<<"Choice: ";
		getline(cin,input);

		if(input == "E") break;

		else if(!stringstream(input)>>choice){

			cout<<"Input not valid, please try again"<<endl;

		}
		else if(choice > setConstits.size()-1 or choice < 0){

			cout<<"Out of range, again please"<<endl;

		}
		else{

			setConstits.erase(setConstits.begin()+choice);


		}

	}

}

map<int,map<string,double>> elProj::getSwingMap(bool randomness){

	//oragnise constits by area

	map<int,map<string,double>> swingMap;
	map<int,vector<pair<unique_ptr<constituencyBase>,unique_ptr<constituencyBase>>>> constitsByArea;
	map<int,double> totalVotesArea;
	map<string,double> totalSwing;
	map<int,map<string,double>> randomMap;
	double totalVotes = 0;

	normal_distribution<double> dist(0,0.03);

	//init total votes in each area to 0
	for(auto& constit : initialEl.getConstitVec()){
		totalVotesArea[constit->getArea()] = 0;
	}

	//iterate over set constits
	//sort set constits by area and add to total votes by area and total votes
	for(auto& constitPair : setConstits){
		constitsByArea[constitPair.first->getArea()].push_back(std::move(constitPair));
		totalVotesArea[constitPair.first->getArea()] += constitPair.second->getVotesCast();
		totalVotes += constitPair.second->getVotesCast();
	}

	//iterate over set constits by area
	for(auto& AreaConsitVec : constitsByArea){

		//if(randomness)
		//	cout<<"Area "<<AreaConsitVec.first<<" has entries"<<endl;

		map<string,double> areaSwing;

		for(auto& constitPair : AreaConsitVec.second){

			map<string,double> constitSwing = constitPair.second->getSwings(std::move(constitPair.first));

			for(auto partySwing = constitSwing.begin(); partySwing != constitSwing.end(); partySwing++){

				//check swing has been applied to party in area
				if(areaSwing.find(partySwing->first) == areaSwing.end()){
					areaSwing[partySwing->first] = 0;
				}
				//check swing has been applied globaly
				if(totalSwing.find(partySwing->first) == totalSwing.end()){
					totalSwing[partySwing->first] = 0;					
				}

				areaSwing[partySwing->first] += partySwing->second*constitPair.second->getVotesCast();
				totalSwing[partySwing->first]+= partySwing->second*constitPair.second->getVotesCast();

			}

		}

		vector<string> partiesInRegion = initialEl.getParties(AreaConsitVec.first);

		double sigma_area = elPred::adjustedSigma(AreaConsitVec.second.size(), initialEl.numConstits(AreaConsitVec.first),0.04);

		//if(randomness)
		//	cout<<"Frac = "<<AreaConsitVec.second.size()<<"/"<<initialEl.numConstits(AreaConsitVec.first)<<", sigma = "<<sigma_area<<endl;

		normal_distribution<double> distArea(0,sigma_area);

		double totalRand = 0;

		for(auto party_it = partiesInRegion.begin(); party_it != partiesInRegion.end(); party_it++){

			//generate random numbers for this area
			if(randomness){	

				if(next(party_it) != partiesInRegion.end()){
					double randNum = distArea(elPred::generator);					
					randomMap[AreaConsitVec.first][*party_it] = randNum;
					totalRand += randNum;

					//cout<<"Party: "<<*party_it<<" rand = "<<randNum<<endl;
					
				}
				else{
					randomMap[AreaConsitVec.first][*party_it] = -totalRand;

					//cout<<"Party: "<<*party_it<<" rand = "<<(-totalRand)<<endl;
				}
			}

		}

		for(auto it = areaSwing.begin(); it != areaSwing.end(); it++){

			areaSwing[it->first] /= totalVotesArea[AreaConsitVec.first];
			areaSwing[it->first] += randomMap[AreaConsitVec.first][it->first];

		}

		swingMap[AreaConsitVec.first] = areaSwing;

	}

	//get global swing
	for(auto it = totalSwing.begin(); it != totalSwing.end(); it++){

		totalSwing[it->first] /= totalVotes;

		//apply fluctuations to total swing
		if(randomness)
			for(auto randArea : randomMap){
				totalSwing[it->first] += randArea.second[it->first]*(totalVotesArea[randArea.first]/totalVotes);
			}

	}

	for(auto totalVotePair : totalVotesArea){

		//if area swing not available, set it to global
		if(totalVotePair.second == 0){

			//if(randomness)
			//	cout<<"Setting area "<<totalVotePair.first<<endl;

			swingMap[totalVotePair.first] = totalSwing;

			if(randomness){
				double totalTmp = 0;
	
				for(auto partyPair = swingMap[totalVotePair.first].begin(); partyPair != swingMap[totalVotePair.first].end(); partyPair++){
	
					if(next(partyPair) != swingMap[totalVotePair.first].end()){
						double randNum = dist(elPred::generator);
						swingMap[totalVotePair.first][partyPair->first] += randNum;
						totalTmp += randNum;
						//cout<<"Party: "<<partyPair->first<<", rand = "<<randNum<<endl;
					}
					else{
						swingMap[totalVotePair.first][partyPair->first] -= totalTmp;
						//cout<<"Party: "<<partyPair->first<<", rand = "<<(-totalTmp)<<endl;
					}
	
					
				}
			}
		}

	}

	if(false){
		for(auto areaPair : swingMap){
			for(auto partyPair : areaPair.second){
				cout<<"Area: "<<areaPair.first<<", party: "<<partyPair.first<<" = "<<partyPair.second<<endl;
			}
		}
	}

	return swingMap;

}

void elProj::print(int choice){

	if(choice == -1 or choice > projectionList.size()-1){
		projectionList.back().print();
	}

	else{
		projectionList[choice].print();
	}

}
void elProj::printDeclared(){

	for(auto& constitPair : setConstits){

		unique_ptr<constituencyFPTP> FPTPConstit1 (dynamic_cast<constituencyFPTP*> (elPred::enterNewResults(std::move(constitPair.first)).get()));
		unique_ptr<constituencyFPTP> FPTPConstit2 (dynamic_cast<constituencyFPTP*> (elPred::enterNewResults(std::move(constitPair.second)).get()));

		string holdGain;

		if(FPTPConstit2->isHeld()){
			holdGain = " ("+FPTPConstit2->getParty()+" HOLD)";
		}
		else{
			holdGain = " ("+FPTPConstit2->getParty()+" GAIN from "+FPTPConstit1->getParty()+")";
		}

		cout<<FPTPConstit2->getName()<<holdGain<<endl;

	}

}
void elProj::printProjectedGains(election el){

	if(el.numConstits()==0){
		el = projectionList.back();
	}

	for(auto& constitBase : el.getConstitVec()){

		bool declared = false;

		constituencyFPTP constit = dynamic_cast<constituencyFPTP&>(*constitBase);

		for(auto& constitPair : setConstits){

			if(constit.getName() == constitPair.first->getName()){
				declared = true;
				break;
			}

		}
		if(declared) continue;

		if(constit.isHeld()) continue;

		cout<<"Area: "<<constit.getArea()<<", "<<constit.getName()<<" ("<<dynamic_cast<constituencyFPTP*>((oldElWDec.getConstit(constit.getName())).get())->getParty()<<" -> "<<constit.getParty()<<")"<<endl;

	}

}

void elProj::project(bool randomness){


	if(randomness){

		clock_t begin = clock();

		vector<election> elVec;

		for(int i = 0; i<100; i++){
			unique_ptr<map<int,map<string,double>>> swingMap (new map<int,map<string,double>>(getSwingMap(randomness)));
			elVec.emplace_back(oldElWDec.swing(std::move(swingMap),true));
			//printProjectedGains(elVec.back());
		}

		checkResults(elVec);

		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

		cout<<"Simulation run in "<<elapsed_secs<<" s"<<endl;

	}


	else{
		unique_ptr<map<int,map<string,double>>> swingMap (new map<int,map<string,double>>(getSwingMap(randomness)));
		projectionList.emplace_back(oldElWDec.swing(std::move(swingMap)));
	}
}
void elProj::checkResults(vector<election> elVec){

	int majority = elVec[0].numConstits()/2+1;

	vector<string> parties = elVec[0].getParties();

	map<string,int> partyMajorities;

	map<string,unique_ptr<vector<int>>> seatNum;
	map<string,unique_ptr<vector<int>>> seatNumreg;
	
	for(string party : parties){

		partyMajorities[party] = 0;
		unique_ptr<vector<int>> tmpPtr(new vector<int>());
		seatNum[party] = std::move(tmpPtr);
		unique_ptr<vector<int>> tmpPtrReg(new vector<int>());
		seatNumreg[party] = std::move(tmpPtrReg);

	}
	partyMajorities["hung"] = 0;

	int area = setConstits.back().second->getArea();

	for(auto el = elVec.begin(); el != elVec.end(); ++el){

		bool hung = true;

		for(string party : parties){

			int seats = el->getSeats(party);

			if(seats > majority){
				++partyMajorities[party];
				hung = false;
			}

			seatNum[party]->push_back(seats);
			seatNumreg[party]->push_back(el->getSeats(party,area));

		}

		if(hung)
			++partyMajorities["HUNG"];

	}

	majoritiesMap.push_back(partyMajorities);

	map<string,pair<double,double>> tmpMap;
	map<string,pair<double,double>> tmpMapReg;

	for(string party : parties){

		double mean, SD;
		double meanReg, SDReg;

		elPred::meanSD(std::move(seatNum[party]), mean, SD);
		elPred::meanSD(std::move(seatNumreg[party]),meanReg,SDReg);

		tmpMap[party] = make_pair(mean,SD);
		tmpMapReg[party] = make_pair(meanReg,SDReg);

	}

	seatSpreadRegions[0].push_back(tmpMap);
	seatSpreadRegions[area].push_back(tmpMapReg);

	cout<<"Majority occurances after "<<setConstits.size()<<" declared: "<<endl;
	for(auto partyMaj : partyMajorities){
		if(partyMaj.second ==0 ) continue;
		cout<<partyMaj.first<<": "<<partyMaj.second<<endl;

	}
	cout<<"Seat spreads:"<<endl;
	for(auto tmpPair : tmpMap){
		cout<<tmpPair.first<<": "<<tmpPair.second.first<<" +/- "<<tmpPair.second.second<<endl;
	}
	cout<<endl;


}

void elProj::run(){

	while(true){

		cout<<"Choose option: "<<endl;
		cout<<"(E) Exit"<<endl
			<<"(A) Add new result or edit a mistake"<<endl
			<<"(X) Remove a result"<<endl
			<<"(P) Project current results"<<endl
			<<"(T) Project with randomness"<<endl
			<<"(R) Show last projection"<<endl
			<<"(D) Print declared seats"<<endl
			<<"(G) Print projected gains"<<endl
			<<"(S) Save current projection"<<endl
			<<"(W) Write results"<<endl
			<<"(WS) Write sim results"<<endl
			<<"(L) Load a saved projection"<<endl
			<<"(LR) Load with randomness"<<endl
			<<"(O) Set options"<<endl;

		string input;
		getline(cin,input);

		if(input == "E") return;
		else if(input == "A") addNewResult();
		else if(input == "X") removeResult();
		else if(input == "P") project();
		else if(input == "T") project(true);
		else if(input == "R") print();
		else if(input == "D") printDeclared();
		else if(input == "G") printProjectedGains();
		else if(input == "S") saveProj();
		else if(input == "W") writeProjResults();
		else if(input == "WS") writeSimResults();
		else if(input == "L") loadProj();
		else if(input == "LR") loadProj(true);
		else if(input == "O") setOpts();
		else cout<<"Input not valid, try again"<<endl;
	}

}
void elProj::setOpts(){

	while(true){

		cout<<"Options: "<<endl
		<<"		Auto project = "<<autoProject<<endl
		<<"		Auto sim = "<<autoSim<<endl;

		if(outNameSet){
			cout<<"		Output name: "<<outName<<endl;
		}
		else{
			cout<<"		Output name not set"<<endl;
		}

		cout<<"Choose option:"<<endl
			<<"(AP) toggle auto project"<<endl
			<<"(AS) toggle auto sim"<<endl
			<<"(O) set output name"<<endl
			<<"(B) go back to main menu"<<endl;

		string input;
		getline(cin,input);

		if(input == "AP"){
			if(autoProject)
				autoProject = false;
			else
				autoProject = true;
		}
		else if(input == "AS"){
			if(autoSim)
				autoSim = false;
			else
				autoSim = true;
		}
		else if(input == "O"){
			
			cout<<"Type name for output, leave blank to remove"<<endl;
			string name;
			getline(cin,name);

			if(name.empty()){
				outName.clear();
				outNameSet = false;
			}
			else{
				outName = name;
				outNameSet = true;
			}

		}
		else if(input == "B") return;
		else cout<<"Response not recognised, try again"<<endl;

	}

}

void elProj::saveProj(){

	cout<<"Enter name for output: ";

	string input;
	getline(cin,input);

	ofstream outFile;
	outFile.open(input+".csv");

	//build vector of parties

	vector<string> parties;

	for(auto& constitPair : setConstits){

		for(string party : constitPair.first->partiesContestingSeat()){

			//check if we already have this in the vector

			bool inVector = false;

			for(string party2 : parties){

				if(party2 == party) {
					inVector = true;
					break;
				}

			}

			if(inVector) continue;
			else parties.push_back(party);

		}

	}

	//header line

	outFile<<"Name,MP,Country,Area,County,Electorate";

	for(string party : parties){

		outFile<<","<<party;

	}

	outFile<<"\n";

	//results

	for(auto& constitPair : setConstits){

		outFile<<constitPair.second->getName()<<","<<"MP"<<","<<constitPair.second->getCountry()<<","<<constitPair.second->getArea()<<","<<constitPair.second->getCounty()<<","<<constitPair.second->getElectorate();

		for(string party : parties){

			if(constitPair.second->partyContestsSeat(party)){
				outFile<<","<<constitPair.second->getVotesCast(party);
			}
			else{
				outFile<<",";
			}

		}

		outFile<<"\n";

	}

	outFile.close();

}

void elProj::writeProjResults(){

	string name;

	if(outNameSet){
		name = outName;
	}
	else{
		cout<<"Enter name for output: ";
		getline(cin,name);
	}

	ofstream outFile;
	outFile.open(name+".csv");

	outFile<<"projNum";

	for(string party : oldElWDec.getParties()){

		outFile<<","<<party;

	}
	outFile<<"\n";

	int i = 0;

	for(election tmpEl : projectionList){

		outFile<<i;

		for(string party : oldElWDec.getParties()){

			outFile<<","<<tmpEl.getSeats(party);

		}

		outFile<<"\n";

		i++;

	}

}
void elProj::writeSimResults(){

	string name;

	if(outNameSet){
		name = outName;
	}
	else{
		cout<<"Enter name for output: ";	
		getline(cin,name);
	}

	ofstream outFile;
	outFile.open(name+"_Majorities.csv");

	outFile<<"projNum";

	vector<string> partiesWchance;

	for(auto tmpMap : majoritiesMap){
		for(auto partyPair : tmpMap){
			if(std::find(partiesWchance.begin(), partiesWchance.end(), partyPair.first) != partiesWchance.end()) {
				continue;
			}
			else if(partyPair.second>0)
				partiesWchance.push_back(partyPair.first);
		}
	}

	for(string party : partiesWchance){

		outFile<<","<<party;

	}
	outFile<<"\n";

	int i = 0;

	for(map<string,int> tmpMap : majoritiesMap){

		outFile<<i;

		for(string party : partiesWchance){

			outFile<<","<<tmpMap[party];
		
		}

		outFile<<"\n";

		i++;

	}
	outFile.close();

	writeSeatSpread("all",name,0);
	writeSeatSpread("NI",name,1);
	writeSeatSpread("Sco",name,2);
	writeSeatSpread("Wal",name,6);	

}
void elProj::writeSeatSpread(const string& name, const string& input, int area){
	
	vector<string> partiesRegion = oldElWDec.getParties(area);

	ofstream outFile;
	outFile.open(input+"_Seats_"+name+".csv");

	outFile<<"projNum";


	for(string party : partiesRegion){

		outFile<<","<<party;

	}
	outFile<<"\n";

	int j = 0;

	for(auto tmpMap : seatSpreadRegions[area]){

		outFile<<j;

		for(string party : partiesRegion){

			if(tmpMap.count(party)>0)
				outFile<<","<<tmpMap[party].first<<","<<tmpMap[party].second;
			else
				outFile<<","<<0<<","<<0;

		}

		outFile<<"\n";

		j++;

	}
	outFile.close();
}

void elProj::loadProj(bool randomness){

	//projections are saved the same as elections
	//read in using helper function, exctract constits and match to those in previous el using functions present
	//run projection at each point as el class saves oreder

	cout<<"Enter name of input: ";

	string input;
	getline(cin,input);

	election tmpEl = elPred::loadfile(input);

	projectionList.clear();
	setConstits.clear();
	oldElWDec = initialEl;

	for(auto& constit : tmpEl.getConstitVec()){

		unique_ptr<constituencyBase> prevConstit = oldElWDec.getConstit(constit->getName());
		unique_ptr<constituencyBase> newConstit  = tmpEl.getConstit(constit->getName());

		newConstit->setPreventSwing(true);

		election tmpEl2 = oldElWDec;
		oldElWDec = tmpEl2.addNewResult(std::move(newConstit));

		setConstits.push_back(make_pair(std::move(prevConstit),std::move(newConstit)));

		project(randomness);
		if(outNameSet and randomness){
			writeSimResults();
		}

	}

}
