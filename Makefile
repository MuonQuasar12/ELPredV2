EXE_NAME     := elPredV2
$(EXE_NAME): *.cpp *.h
	g++ -std=c++14 *.cpp  -o  elPredV2