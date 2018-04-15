#ifndef CONSTITFPTP_H
#define CONSTITFPTP_H

#include <string>
#include <map>
#include <vector>
#include "vars.h"
#include <memory>
#include <random>
#include "constituencyBase.h"

using namespace std;

class constituencyFPTP: public constituencyBase{

private:
	string MP;
	bool hold = true;
	double weight = 1;

public:

	static normal_distribution<double> dist;

	constituencyFPTP(string name_, string MP_, string country_, string county_, int area_, bool isHeld = true, bool preventSwing = false):
	constituencyBase(name_,country_,county_,area_,preventSwing)
	{
		MP = MP_;
		hold = isHeld;
	}
	constituencyFPTP(){
		name = "null";
	}

	virtual void swing(unique_ptr<map<string,double>> swingVals, bool randomness = false) override;

	string getMP(){return MP;}
	string getParty();
	string getSecondPlace();
	
	int getMajority();
	double getMajorityFrac();
	bool isHeld(){return hold;}
	void setHold(bool isAHold){hold = isAHold;}
	void setWeight(double w){weight = w;}
	double getWeight(){return weight;}
	virtual int getNumSeats(string party) override;

	virtual void print(int opt = 1) override;

	virtual unique_ptr<constituencyBase> clone() const override;

};

#endif