#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include "constituencyBase.h"
#include "election.h"
#include <map>
#include <utility>
#include <chrono>
#include <memory>

namespace elPred{

	static std::default_random_engine generator (std::chrono::system_clock::now().time_since_epoch().count());

	election loadfile(std::string fileName);

	unique_ptr<constituencyBase> constitSearch(election el);

	unique_ptr<constituencyBase> enterNewResults(unique_ptr<constituencyBase> constit);

	map<int,map<string,double>> generateSwingMap(map<vector<int>,map<string,double>> rawSwings, election el);

	double adjustedSigma(int declaredConstits, int totalConstits, double initialSigma);

	void meanSD(unique_ptr<vector<int>> data, double& mean, double& SD);

}

#endif