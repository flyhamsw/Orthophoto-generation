#include "stdafx.h"
#include "Row.h"
#include "ApxModifier.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <stdlib.h>
#include <math.h>
#include <string>

using namespace std;

//Constant value for pi
const double pi = 3.141592653589793238463;

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
		//Split data and store to RowGPGGA, RowPASHR, RowPTNL
		char line[300];

		while (!f.eof())
		{
			//Get a line and store to a stringstream
			f.getline(line, sizeof(line));
			stringstream line_stream(line);

			while (!line_stream.eof())
			{
				//In a stringstream, get a comma separated word
				//Determine whether it is GPS data or INS Data
				char* lineHeader = new char[20];
				line_stream.getline(lineHeader, 20, ',');

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
	//Find GPS data and INS data with same time, and conjugate the GPS-INS data
	cout << "Matching Data...";

	while (!(qGPGGA.empty()) && !(qPASHR.empty()))
	{
		double time_GPGGA;
		double time_PASHR;

		RowGPGGA* currRowGPGGA = qGPGGA.front();
		RowPASHR* currRowPASHR = qPASHR.front();

		time_GPGGA = atof(currRowGPGGA->time);
		time_PASHR = atof(currRowPASHR->time);

		//Pop queue with lower time value
		if (time_GPGGA < time_PASHR)
		{
			qGPGGA.pop();
		}
		else if (time_GPGGA > time_PASHR)
		{
			qPASHR.pop();
		}
		//If time data of each queue are same, store them, and pop both queues
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

//Copyright: http://m.blog.naver.com/tacma/20108668315
void convertWGS84_to_TM(double *plon, double *plat)
{
	double lon, lat;
	lon = *plon;
	lat = *plat;
	double m_arScaleFactor = 1;
	double m_arLonCenter = 127 * pi / 180; //Projection center of Korean central belt 2010
	double m_arLatCenter = 38 * pi / 180; //Projection center of Korean central belt 2010
	double m_arFalseNorthing = 600000.0;
	double m_arFalseEasting = 200000.0;
	double x, y;
	double m_dDstInd;
	double m_arMajor = 6378137.0;
	double m_arMinor = 6356752.3142;
	double delta_lon; // Delta longitude (Given longitude - center longitude)
	double sin_phi, cos_phi; // sin and cos value
	double al, als; // temporary values
	double b, c, t, tq; // temporary values
	double con, n, ml; // cone constant, small m
	double temp = m_arMinor / m_arMajor;
	double m_ecc = 1.0 - temp * temp;
	double m_eccp = (1 / temp)*(1 / temp) - 1;
	double m_dDstE0 = 1.0 - 0.25 * m_ecc * (1.0 + m_ecc / 16.0 * (3.0 + 1.25 * m_ecc));
	double m_dDstE1 = 0.375 * m_ecc * (1.0 + 0.25 * m_ecc * (1.0 + 0.46875 * m_ecc));
	double m_dDstE2 = 0.05859375 * m_ecc * m_ecc * (1.0 + 0.75 * m_ecc);
	double m_dDstE3 = m_ecc * m_ecc * m_ecc * (35.0 / 3072.0);
	double m_dDstMl0 = m_arMajor * (m_dDstE0 * m_arLatCenter - m_dDstE1 * sin(2.0 * m_arLatCenter) + m_dDstE2 * sin(4.0 * m_arLatCenter) - m_dDstE3 * sin(6.0 * m_arLatCenter));
	m_dDstInd = 0.0;
	delta_lon = lon - m_arLonCenter;
	sin_phi = sin(lat);
	cos_phi = cos(lat);
	b = 0;
	x = 0.5 * m_arMajor * m_arScaleFactor * log((1.0 + b) / (1.0 - b));
	con = acos(cos_phi * cos(delta_lon) / sqrt(1.0 - b * b));
	al = cos_phi * delta_lon;
	als = al * al;
	c = m_eccp * cos_phi * cos_phi;
	tq = tan(lat);
	t = tq * tq;
	con = 1.0 - m_ecc * sin_phi * sin_phi;
	n = m_arMajor / sqrt(con);
	ml = m_arMajor * (m_dDstE0 * lat - m_dDstE1 * sin(2.0 * lat) + m_dDstE2 * sin(4.0 * lat) - m_dDstE3 * sin(6.0 * lat));
	x = m_arScaleFactor * n * al * (1.0 + als / 6.0 * (1.0 - t + c + als / 20.0 * (5.0 - 18.0 * t + t * t + 72.0 * c - 58.0 * m_eccp))) + m_arFalseEasting;
	y = m_arScaleFactor * (ml - m_dDstMl0 + n * tq * (als * (0.5 + als / 24.0 * (5.0 - t + 9.0 * c + 4.0 * c * c + als / 30.0 * (61.0 - 58.0 * t + t * t + 600.0 * c - 330.0 * m_eccp))))) + m_arFalseNorthing;
	*plon = x;
	*plat = y;
}

void ApxModifier::interpolateData()
{
	cout << "Finding adjacent data...";

	if (qPTNL.size() == 0)
	{
		cout << "No event data" << endl << endl;
	}
	else
	{
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

			//if diffA is negative and diffB is positive, the photo was taken between A and B
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

		//Interpolate by time difference
		double weightA = 1 / diffA;
		double weightB = 1 / diffB;

		double weightedLat = (atof(rowBefore->rowGPGGA->lat)*weightA + atof(rowAfter->rowGPGGA->lat)*weightB) / (weightA + weightB);
		double weightedLng = (atof(rowBefore->rowGPGGA->lng)*weightA + atof(rowAfter->rowGPGGA->lng)*weightB) / (weightA + weightB);
		double weightedHeightWGS84 = (atof(rowBefore->rowGPGGA->heightWGS84)*weightA + atof(rowAfter->rowGPGGA->heightWGS84)*weightB) / (weightA + weightB);
		double weightedHeading = (atof(rowBefore->rowPASHR->heading)*weightA + atof(rowAfter->rowPASHR->heading)*weightB) / (weightA + weightB);
		double weightedRoll = (atof(rowBefore->rowPASHR->roll)*weightA + atof(rowAfter->rowPASHR->roll)*weightB) / (weightA + weightB);
		double weightedPitch = (atof(rowBefore->rowPASHR->pitch)*weightA + atof(rowAfter->rowPASHR->pitch)*weightB) / (weightA + weightB);

		//Split weighted values into Degree and Minute
		double wLat_min = fmod(weightedLat, 100);
		double wLat_deg = ((weightedLat - wLat_min) / 100);

		double wLng_min = fmod(weightedLng, 100);
		double wLng_deg = ((weightedLng - wLng_min) / 100);

		//Convert Deg, Min values to Radian
		weightedLat = (wLat_deg + wLat_min / 60) * pi / 180;
		weightedLng = (wLng_deg + wLng_min / 60) * pi / 180;

		convertWGS84_to_TM(&weightedLng, &weightedLat);

		//Store interpolated data
		rowInterpolated = new RowInterpolated(weightedLng, weightedLat, weightedHeightWGS84, weightedHeading, weightedRoll, weightedPitch);

		cout << "OK" << endl << endl;

		f.close();
	}

}


void ApxModifier::writeNewFile(char* txt_filename)
{
	if (rowInterpolated == NULL)
	{
		cout << "Data was not interpolated" << endl;
	}
	else
	{
		filename = txt_filename;

		int dotIdx;
		int slIdx;

		for (int i = strlen(filename); i >= 0; i--)
		{
			if (filename[i] == '.')
			{
				dotIdx = i;
			}
			else if (filename[i] == '/')
			{
				slIdx = i;
				break;
			}
		}

		string jpgName = filename;
		jpgName = jpgName.substr(slIdx + 1, dotIdx - slIdx - 1);

		fnew.open(filename);
		fnew.precision(8);
		fnew.setf(ios::fixed);
		fnew.setf(ios::showpoint);

		cout << "Writing a new file (" << filename << ") ...";

		if (fnew.is_open())
		{
			fnew << jpgName << ".jpg" << '\t' << rowInterpolated->X << '\t' << rowInterpolated->Y << '\t' << rowInterpolated->Z << '\t' << rowInterpolated->heading << '\t' << rowInterpolated->roll << '\t' << rowInterpolated->pitch;
		}

		fnew.close();

		cout << "OK" << endl;

	}

}
