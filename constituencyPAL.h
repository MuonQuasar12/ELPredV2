#ifndef CONSTITUENCYPAL_H
#define CONSTITUENCYPAL_H

#include <string>
#include <map>
#include <vector>
#include "vars.h"
#include <memory>
#include <random>
#include "constituencyBase.h"
#include <iostream>

//plurality at large (multimember FPTP)

class constituencyPAL : public constituencyBase {

protected:

	int numSeats = 0;
	map<string, int> seatsMap;
	map<string, int> initSeats;
	int segmentUpForEl = 0; //0 for all, 1 for segment 1 etc
	map<string,bool> hold;
	bool swingFromAllBool = false; //if true swing is applied from sum of all seats rather than just the one
	map<string, votingArea> areaOverrides;
	vector<string> seatHolders;

public:

	static normal_distribution<double> dist;

	constituencyPAL(string name_, string country_, string county_, int area_, bool preventSwing = false) :
	constituencyBase(name_, country_, county_, area_, preventSwing)
	{

	}
	virtual ~constituencyPAL(){
	}

	void setPreventSwing(int seg = 0, bool opt = true);

	virtual void swing(unique_ptr<map<string, double>> swingVals, bool randomness = false) override;
	virtual void addVoteArea(string nameArea, int electorateArea, map<string,int> votesCast_) override;
	virtual map<string, int> getSeats();
	virtual int getNumSeats(string party) override;
	virtual int getNumSeats() override{return numSeats;}

	virtual map<string, double> getVoteShareMap() override;

	virtual void addNewResult(string voteAreaName, map<string, int> results) override;

	void setElSeg(int elSeg);

	virtual void print(int opt = 1) override;
	virtual string lineInfo() override;

	virtual unique_ptr<constituencyBase> clone() const override;

	virtual bool changedHands() override;

	void swingFromAll(bool opt) { swingFromAllBool = opt; }

};

#endif // !CONSTITUENCYPAL_H