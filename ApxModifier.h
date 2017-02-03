#pragma once

#include <fstream>
#include <queue>
#include "Row.h"

using namespace std;

class ApxModifier
{
public:
	Row* rowBefore;
	Row* rowAfter;
	RowInterpolated* rowInterpolated;
	
	ApxModifier(char*);
	void findAdjacentData();
	void writeNewFile(char*);
	void convertRawInterpolated2TM();
private:
	ifstream f;
	ofstream fnew;

	queue<RowGPGGA*> qGPGGA; //Location Queue
	queue<RowPASHR*> qPASHR; //Attitude Queue
	queue<RowPTNL*> qPTNL;   //Event Queue
	queue<Row*> qResult;     //Queue for matched result (by time)
		
	char* filename;
	char* newFilename;

	void loadData();
	void matchData();	
};