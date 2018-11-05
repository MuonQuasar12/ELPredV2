#ifndef CONSTITPR_H
#define CONSTITPR_H

#include <string>
#include <map>
#include <vector>
#include "vars.h"
#include <memory>
#include <random>
#include "constituencyBase.h"

using namespace std;

typedef map<string,int> null_map;

class constituencyPR: public constituencyBase{

protected:

	elPred::countSys system;
	int numSeats;
	map<string,int> seatsMap;
	map<string,int> initSeats;

public:

	static normal_distribution<double> dist;

	constituencyPR(string name_, string country_, string county_, int area_, elPred::countSys sys, int numSeats_, bool preventSwing = false):
	constituencyBase(name_,country_,county_,area_,preventSwing)
	{
		system = sys;
		numSeats = numSeats_;

		//these two might be here due to MMP considerations
		//however, they will cause trouble as they require that the voting areas are added beforehand, which is not the case
		//initSeats = getSeats();
		//seatsMap = initSeats;
	}

	virtual void swing(unique_ptr<map<string,double>> swingVals, bool randomness = false) override;

	virtual void init() override{ initSeats = getSeats(); }
	virtual map<string,int> getSeats(map<string,int> seatPreMap = null_map());
	virtual int getNumSeats(string party) override;
	virtual int getNumSeats() override{return numSeats;}

	virtual void print(int opt = 1) override;
	virtual string lineInfo() override;

	virtual unique_ptr<constituencyBase> clone() const override;

	virtual bool changedHands() override;

};

#endif