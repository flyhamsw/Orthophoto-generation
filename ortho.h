#pragma once

//#include "../KLT/AerialSimulator.h"	// To use the CAerialSimulator object
//#include "../KLT/KLTTracker.h"			// To use the CKLTTracker object
#include <iostream>
#include <fstream>
#include <direct.h>		// To make a directory
#include <vector>		// STL
#include <conio.h>		// To use _getch()
//#include <string>
//#include <algorithm>
//#include <cv.h>
//#include <cxcore.h>
//#include <highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <Windows.h>
#include "ortho.h"
#include <stdio.h>
#include <Windows.h>
#include <WinInet.h>

using namespace std;
using namespace cv;

#define M_PI 3.14159265358979323846  /* pi */

typedef struct tagData
{
	string imagename;
	double eo[6];
}Data;

//typedef struct ROIData
//{
//	string imagename;
//	double pixelXY[4];
//}Roi;

//void ortho();
bool ortho(char* EOname);
//istream& ReadROIFile(istream& in, vector<Roi>& roiData);
istream& ReadEOFile(istream& in, vector<Data>& vec);
Mat orthophoto_m(Mat srcImage, int* pixel_cnt, double pixel_size,
	double focal_length, double* EO, double gsd, double ground_height);
Mat rot3D(double* EO);
void vertex_g(int* pixel_cnt, double pixel_size, double focal_length, double* EO, Mat R, double ground_height, double* g_c);
Mat xy_g_min(double* EO, Mat R, Mat xy_image, double ground_height);
//void dem_m(double* g_c, int row_s, int col_s, double gsd, double ground_height, double* dem_x, double* dem_y, double* dem_z);
void image_coordinate(double* g_c, double gsd, double ground_height, int row_s, int col_s, Mat R, double* EO, double focal_length, double pixel_size, int* pixel_cnt, double* x_fcs, double* y_fcs);
Mat pixel_color(int* pixel_cnt, double* x_fcs, double* y_fcs, int row_s, int col_s, Mat srcImage);
double getMax(double* n, int size);
double getMin(double* n, int size);

