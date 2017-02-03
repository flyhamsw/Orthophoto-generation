#include "stdafx.h"
#include "Row.h"
#include "ApxModifier.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <stdlib.h>

using namespace std;

ApxModifier::ApxModifier(char* txt_filename)
{
	filename = txt_filename;
	f.open(filename);
	loadData();
	matchData();
}

void ApxModifier::loadData()
{
	cout << "=========================================================" << endl << "Loading file (" << filename << ") ...";
	if (f.is_open())
	{
		char line[100];
		while (!f.eof())
		{
			f.getline(line, sizeof(line));
			stringstream line_stream(line);

			while (!line_stream.eof())
			{
				char* lineHeader = new char[20];
				line_stream.getline(lineHeader, 20, ',');

				//cout << lineHeader << endl;

				if (strcmp(lineHeader, "$GPGGA") == 0)
				{
					char* time = new char[20];
					char* lat = new char[20];
					char* latHeading = new char[20];
					char* lng = new char[20];
					char* lngHeading = new char[20];
					char* gpsQuality = new char[20];
					char* numOfSV = new char[20];
					char* HDOP = new char[20];
					char* alt = new char[20];
					char* altUnit = new char[20];
					char* heightWGS84 = new char[20];
					char* heightWGS84Unit = new char[20];
					char* DGPS = new char[20];
					char* checksum = new char[20];

					line_stream.getline(time, 20, ',');
					line_stream.getline(lat, 20, ',');
					line_stream.getline(latHeading, 20, ',');
					line_stream.getline(lng, 20, ',');
					line_stream.getline(lngHeading, 20, ',');
					line_stream.getline(gpsQuality, 20, ',');
					line_stream.getline(numOfSV, 20, ',');
					line_stream.getline(HDOP, 20, ',');
					line_stream.getline(alt, 20, ',');
					line_stream.getline(altUnit, 20, ',');
					line_stream.getline(heightWGS84, 20, ',');
					line_stream.getline(heightWGS84Unit, 20, ',');
					line_stream.getline(DGPS, 20, ',');
					line_stream.getline(checksum, 20, ',');

					RowGPGGA* row = new RowGPGGA(time, lat, latHeading, lng, lngHeading, gpsQuality, numOfSV, HDOP, alt, altUnit, heightWGS84, heightWGS84Unit, DGPS, checksum);

					qGPGGA.push(row);
				}
				else if (strcmp(lineHeader, "$PASHR") == 0)
				{
					char* time = new char[20];
					char* heading = new char[20];
					char* headingTrue = new char[20];
					char* roll = new char[20];
					char* pitch = new char[20];
					char* heave = new char[20];
					char* rollAccuracy = new char[20];
					char* pitchAccuracy = new char[20];
					char* headingAccuracy = new char[20];
					char* aidingStatus = new char[20];
					char* IMUStatus = new char[20];

					line_stream.getline(time, 20, ',');
					line_stream.getline(heading, 20, ',');
					line_stream.getline(headingTrue, 20, ',');
					line_stream.getline(roll, 20, ',');
					line_stream.getline(pitch, 20, ',');
					line_stream.getline(heave, 20, ',');
					line_stream.getline(rollAccuracy, 20, ',');
					line_stream.getline(pitchAccuracy, 20, ',');
					line_stream.getline(headingAccuracy, 20, ',');
					line_stream.getline(aidingStatus, 20, ',');
					line_stream.getline(IMUStatus, 20, ',');

					RowPASHR* row = new RowPASHR(time, heading, headingTrue, roll, pitch, heave, rollAccuracy, pitchAccuracy, headingAccuracy, aidingStatus, IMUStatus);

					qPASHR.push(row);
				}
				else if (strcmp(lineHeader, "$PTNL") == 0)
				{
					char* type = new char[20];
					char* time = new char[20];
					char* field4 = new char[20];
					char* field5 = new char[20];
					char* field6 = new char[20];
					char* field7 = new char[20];
					char* field8 = new char[20];

					line_stream.getline(type, 20, ',');
					line_stream.getline(time, 20, ',');
					line_stream.getline(field4, 20, ',');
					line_stream.getline(field5, 20, ',');
					line_stream.getline(field6, 20, ',');
					line_stream.getline(field7, 20, ',');
					line_stream.getline(field8, 20, ',');

					RowPTNL* row = new RowPTNL(type, time, field4, field5, field6, field7, field8);

					qPTNL.push(row);
				}
			}
		}
		cout << "OK" << endl;

		cout << "# of GPGGA(Location): " << qGPGGA.size() << endl
			<< "# of PASHR(Attitude): " << qPASHR.size() << endl
			<< "# of PTNL(Event Time): " << qPTNL.size() << endl << endl;
	}

}

void ApxModifier::matchData()
{
	cout << "Matching Data...";

	while (!(qGPGGA.empty()) && !(qPASHR.empty()))
	{
		double time_GPGGA;
		double time_PASHR;

		RowGPGGA* currRowGPGGA = qGPGGA.front();
		RowPASHR* currRowPASHR = qPASHR.front();

		time_GPGGA = atof(currRowGPGGA->time);
		time_PASHR = atof(currRowPASHR->time);

		if (time_GPGGA < time_PASHR)
		{
			qGPGGA.pop();
		}
		else if (time_GPGGA > time_PASHR)
		{
			qPASHR.pop();
		}
		else if (time_GPGGA == time_PASHR)
		{
			Row* matchedRow = new Row(currRowGPGGA, currRowPASHR);
			qResult.push(matchedRow);
			qGPGGA.pop();
			qPASHR.pop();
		}
	}

	cout << "OK" << endl;
	cout << "# of Result Queue: " << qResult.size() << endl << endl;
}

void ApxModifier::findAdjacentData()
{
	cout << "Finding adjacent data...";

	double takenTime = atof(qPTNL.front()->time);
	double diffA;
	double diffB;

	int q_size = qResult.size();

	//Find Location/Attitude data adjacent to the event
	for (int i = 0; i < q_size - 1; i++)
	{
		rowBefore = qResult.front();
		qResult.pop();
		rowAfter = qResult.front();

		diffA = atof(rowBefore->rowGPGGA->time) - takenTime;
		diffB = atof(rowAfter->rowGPGGA->time) - takenTime;

		if (diffA * diffB < 0)
		{
			break;
		}
	}
	cout << "OK" << endl;
	cout << "The photo was taken between " << rowBefore->rowGPGGA->time << " and " << rowAfter->rowGPGGA->time << endl << endl;
	cout << "Interpolation...";

	diffA = abs(diffA);
	diffB = abs(diffB);

	double weightA = 1 / diffA;
	double weightB = 1 / diffB;

	double weightedLat = (atof(rowBefore->rowGPGGA->lat)*weightA + atof(rowAfter->rowGPGGA->lat)*weightB) / (weightA + weightB);
	double weightedLng = (atof(rowBefore->rowGPGGA->lng)*weightA + atof(rowAfter->rowGPGGA->lng)*weightB) / (weightA + weightB);
	double weightedHeightWGS84 = (atof(rowBefore->rowGPGGA->heightWGS84)*weightA + atof(rowAfter->rowGPGGA->heightWGS84)*weightB) / (weightA + weightB);
	double weightedHeading = (atof(rowBefore->rowPASHR->heading)*weightA + atof(rowAfter->rowPASHR->heading)*weightB) / (weightA + weightB);
	double weightedRoll = (atof(rowBefore->rowPASHR->roll)*weightA + atof(rowAfter->rowPASHR->roll)*weightB) / (weightA + weightB);
	double weightedPitch = (atof(rowBefore->rowPASHR->pitch)*weightA + atof(rowAfter->rowPASHR->pitch)*weightB) / (weightA + weightB);

	rowInterpolated = new RowInterpolated(weightedLng, weightedLat, weightedHeightWGS84, weightedHeading, weightedRoll, weightedPitch);

	cout << "OK" << endl << endl;

	f.close();
}

void ApxModifier::writeNewFile(char* txt_filename)
{
	filename = txt_filename;

	fnew.open(filename);
	fnew.precision(8);
	fnew.setf(ios::fixed);
	fnew.setf(ios::showpoint);

	cout << "Writing a new file (" << filename << ") ...";

	if (fnew.is_open())
	{
		fnew << rowInterpolated->X << '\t' << rowInterpolated->Y << '\t' << rowInterpolated->Z << '\t' << rowInterpolated->heading << '\t' << rowInterpolated->roll << '\t' << rowInterpolated->pitch;
	}

	fnew.close();

	cout << "OK" << endl;
}

void ApxModifier::convertRawInterpolated2TM()
{
}
