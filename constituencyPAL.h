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

class constituencyPAL : constituencyBase {

protected:

	int numSeats;
	map<string, int> seatsMap;
	map<string, int> initSeats;
	int segmentUpForEl = 0; //0 for all, 1 for segment 1 etc

public:

	static normal_distribution<double> dist;

	constituencyPAL(string name_, string country_, string county_, int area_, int numSeats_, bool preventSwing = false) :
	constituencyBase(name_, country_, county_, area_, preventSwing)
	{
		numSeats = numSeats_;

		initSeats = getSeats();
		seatsMap = initSeats;
	}

	void setPreventSwing(int seg = 0, bool opt = true);

	virtual void swing(unique_ptr<map<string, double>> swingVals, bool randomness = false) override;

	virtual map<string, int> getSeats();
	virtual int getNumSeats(string party) override { return seatsMap[party]; }

	virtual void print(int opt = 1) override;

	virtual unique_ptr<constituencyBase> clone() const override;

};

#endif // !CONSTITUENCYPAL_H