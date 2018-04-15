#include "constituency.h"
#include "election.h"
#include "helpers.h"
#include "elProj.h"
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <random>

//void testrun();
//void swingtest();
//mvoid printAreas();
void regswingtest();
void regswingtest2();
void newResultsTest();
void newResultsTest2();

normal_distribution<double> constituency::dist = normal_distribution<double>(0.0,0.04);

int main(){

	newResultsTest2();
	
	return 0;
}
void newResultsTest2(){

	election GE2015 = elPred::loadfile("results2015b.csv");

	elProj testProj(GE2015);
	testProj.run();


}

void regswingtest(){

	election GE2015 = elPred::loadfile("results2015.csv");	

	//election GE2017 = elPred::loadfile("results2017.csv");	

	map<int,map<string,double>> swingVals;

	map<string,double> swingValsEng;
	swingValsEng["CON"] = 0.044;
	swingValsEng["LAB"] = 0.103;
	swingValsEng["LD"] = -0.004;
	swingValsEng["UKIP"] = -0.121;
	swingValsEng["GRN"] = -0.023;

	map<string,double> swingValsWal;
	swingValsWal["CON"] = 0.063;
	swingValsWal["LAB"] = 0.121;
	swingValsWal["LD"] = -0.020;
	swingValsWal["UKIP"] = -0.116;
	swingValsWal["GRN"] = -0.022;
	swingValsWal["PC"] = -0.017;

	map<string,double> swingValsSco;
	swingValsSco["CON"] = 0.137;
	swingValsSco["LAB"] = 0.028;
	swingValsSco["LD"] = -0.008;
	swingValsSco["UKIP"] = -0.014;
	swingValsSco["GRN"] = -0.011;
	swingValsSco["SNP"] = -0.131;

	map<string,double> swingValsNI;
	swingValsNI["DUP"] = 0.103;
	swingValsNI["SF"]  = 0.049;
	swingValsNI["UUP"] = -0.058;
	swingValsNI["SDLP"] = -0.022;
	swingValsNI["AL"] = -0.006;
	
	for(int i = 2; i<13; i++){
		swingVals[i] = swingValsEng;
	}
	swingVals[1]=swingValsNI;
	swingVals[2]=swingValsSco;
	swingVals[6]=swingValsWal;

	//election GEnever= GE2015.swing(&swingVals);

	//GEnever.print();

	//elPred::constitSearch(GE2017);

}
void regswingtest2(){

	election GE2015 = elPred::loadfile("results2015.csv");	

	//election GE2017 = elPred::loadfile("results2017.csv");	

	map<vector<int>,map<string,double>> swingVals;

	map<string,double> swingValsEng;
	swingValsEng["CON"] = 0.044;
	swingValsEng["LAB"] = 0.103;
	swingValsEng["LD"] = -0.004;
	swingValsEng["UKIP"] = -0.121;
	swingValsEng["GRN"] = -0.023;

	map<string,double> swingValsLon;
	swingValsLon["CON"] = -0.017;
	swingValsLon["LAB"] = 0.108;
	swingValsLon["LD"]  = 0.011;
	swingValsLon["UKIP"]= -0.068;
	swingValsLon["GRN"] = -0.031;

	map<string,double> swingValsWal;
	swingValsWal["CON"] = 0.063;
	swingValsWal["LAB"] = 0.121;
	swingValsWal["LD"] = -0.020;
	swingValsWal["UKIP"] = -0.116;
	swingValsWal["GRN"] = -0.022;
	swingValsWal["PC"] = -0.017;

	map<string,double> swingValsSco;
	swingValsSco["CON"] = 0.137;
	swingValsSco["LAB"] = 0.028;
	swingValsSco["LD"] = -0.008;
	swingValsSco["UKIP"] = -0.014;
	swingValsSco["GRN"] = -0.011;
	swingValsSco["SNP"] = -0.131;

	map<string,double> swingValsNI;
	swingValsNI["DUP"] = 0.103;
	swingValsNI["SF"]  = 0.049;
	swingValsNI["UUP"] = -0.058;
	swingValsNI["SDLP"] = -0.022;
	swingValsNI["AL"] = -0.006;


	swingVals[{1}] = swingValsNI;
	swingVals[{2}] = swingValsSco;
	swingVals[{6}] = swingValsWal;
	swingVals[{3,4,5,7,8,9,10,11,12}] = swingValsEng;
	swingVals[{11}]= swingValsLon;

	auto swingMap = elPred::generateSwingMap(swingVals,GE2015);

	//election GEnever = GE2015.swing(&swingMap);

	//GE2015.print();
	//GEnever.print();

	//constituency constit = elPred::constitSearch(GEnever);
	//constit.print(2);
}
/*
void testrun(){
	map<string,int> votes;
	votes["Con"] = 500;
	votes["Lab"] = 300;
	votes["LD"] = 100;
	votes["UKIP"] = 100;

	map<string,int> votes2;
	votes2["Con"] = 510;
	votes2["Lab"] = 500;
	votes2["LD"] = 200;
	votes2["UKIP"] = 10;

	vector<constituency> ConstVec; 
	ConstVec.push_back(constituency("Woodby on Sea North","Jeff Smith","England","Woodshire",42,10000,votes));
	ConstVec.push_back(constituency("Woodby on Sea South","Janice Smyth","England","Woodshire",42,10000,votes2));

	election testElection(ConstVec);
	testElection.print();

	map<string,double> swingVals;
	swingVals["Con"] = -0.05;
	swingVals["Lab"] = 0.03;
	swingVals["LD"] = 0.02;
	swingVals["UKIP"] = 0;

	election test2 = testElection.swing(swingVals,{42});
	test2.print();
}

void swingtest(){

	election GE2015 = elPred::loadfile("results2015.csv");	
	GE2015.print();

	map<string,double> swingVals;
	swingVals["CON"] = 0.055;
	swingVals["LAB"] = 0.096;
	swingVals["LD"] = -0.005;
	swingVals["UKIP"] = -0.108;
	swingVals["PC"] = -0.001;
	swingVals["GRN"] = -0.022;
	swingVals["SNP"] = -0.017;
	swingVals["DUP"] = 0.003;
	swingVals["SF"]  = 0.002;
	swingVals["UUP"] = -0.001;

	election GEnever = GE2015.swing(swingVals,{1,2,3,4,5,6,7,8,9,10,11,12});
	GEnever.print();

	srand (time(NULL));

	int r = (rand() %  650);
	GEnever.getConstit(r).print(1);
	GEnever.getConstit(r).print(2);

}
void printAreas(){
	election GE2015 = elPred::loadfile("results2015.csv");

	int areaVal = 1;

	while(true){

		cout<<"------------ Area = "<<areaVal<<" ------------"<<endl;
		bool noArea = true;
		for(int i=0; i<GE2015.numConstits(); i++){
			constituency tmpConstit = GE2015.getConstit(i);
			if(tmpConstit.getArea() == areaVal){
				noArea = false;
				tmpConstit.print(3);
			}
		}
		if(noArea) break;
		areaVal++;
	}
}*/