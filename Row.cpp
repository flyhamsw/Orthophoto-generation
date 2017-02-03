#include "stdafx.h"
#include "Row.h"


using namespace std;

RowGPGGA::RowGPGGA(char* txt_time, char* txt_lat, char* txt_latHeading, char* txt_lng, char* txt_lngHeading, char* txt_gpsQuality, char* txt_numOfSV, char* txt_HDOP, char* txt_attHeight, char* txt_altUnit, char* txt_heightWGS84, char* txt_heightWGS84Unit, char* txt_DGPS, char* txt_checksum)
{
	time = txt_time;
	lat = txt_lat;
	latHeading = txt_latHeading;
	lng = txt_lng;
	lngHeading = txt_latHeading;
	gpsQuality = txt_gpsQuality;
	numOfSV = txt_numOfSV;
	HDOP = txt_HDOP;
	alt = txt_attHeight;
	altUnit = txt_altUnit;
	heightWGS84 = txt_heightWGS84;
	heightWGS84Unit = txt_heightWGS84Unit;
	DGPS = txt_DGPS;
	checksum = txt_checksum;
}

RowPASHR::RowPASHR(char* txt_time, char* txt_heading, char* txt_headingTrue, char* txt_roll, char* txt_pitch, char* txt_heave, char* txt_rollAccuracy, char* txt_pitchAccuracy, char* txt_headingAccuracy, char* txt_aidingStatus, char* txt_IMUStatus)
{
	time = txt_time;
	heading = txt_heading;
	headingTrue = txt_headingTrue;
	roll = txt_roll;
	pitch = txt_pitch;
	heave = txt_heave;
	rollAccuracy = txt_rollAccuracy;
	pitchAccuracy = txt_pitchAccuracy;
	headingAccuracy = txt_headingAccuracy;
	aidingStatus = txt_aidingStatus;
	IMUStatus = txt_IMUStatus;
}

RowPTNL::RowPTNL(char* txt_type, char* txt_time, char* txt_field4, char* txt_field5, char* txt_field6, char* txt_field7, char* txt_field8)
{
	type = txt_type;
	time = txt_time;
	field4 = txt_field4;
	field5 = txt_field5;
	field6 = txt_field6;
	field7 = txt_field7;
	field8 = txt_field8;
}

Row::Row(RowGPGGA* matchedRowGPGGA, RowPASHR* matchedRowPASHR)
{
	rowGPGGA = matchedRowGPGGA;
	rowPASHR = matchedRowPASHR;
}

RowInterpolated::RowInterpolated(double intpl_X, double intpl_Y, double intpl_Z, double intpl_heading, double intpl_roll, double intpl_pitch)
{
	X = intpl_X;
	Y = intpl_Y;
	Z = intpl_Z;
	heading = intpl_heading;
	roll = intpl_roll;
	pitch = intpl_pitch;
}

