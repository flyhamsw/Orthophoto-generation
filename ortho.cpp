//#include <direct.h>
//#include <fstream>
//#include <vector>
#include <string>
//#include <conio.h>
#include "ortho.h"
#include <stdio.h>
#include <Windows.h>
#include <WinInet.h>
#include <opencv2\highgui.hpp>


using namespace std;
using namespace cv;


bool ortho(char* EOname)
{		
		cout << endl;
		cout << "*************************** " << endl;
		cout << "Orthoimage Generation Start " << endl;
		cout << "*************************** " << endl;

		_mkdir("C:\\uav_image\\Result\\Temp");

		Mat srcImage;
		Mat sub_srcImage;

		double pixel_size = 6E-6;		// unit : m
		double focal_length = 0.024;	// unit : m
		double gsd = 0.0;

		//string input_foldername = "Input_Image_icheon\\";
		string input_foldername = "C:\\uav_image\\";
		string output_foldername = "C:\\uav_image\\Result\\";
		string img_input, img_output, img_name;

		int pixel_cnt[2];
		double ground_height = 100.0;

		/*ifstream roi("Input\\roi.txt");
		vector<Roi> roiData;
		ReadROIFile(roi, roiData);*/

		/*ifstream in("Results\\AT_Results\\Estimated_EO_이천300m.txt");*/
		ifstream in(EOname);
		
		/*ifstream in(EO);*/
		vector<Data> vec;

		if (!in.is_open())
		{		
			/*cout << "EO 파일을 확인할 수 없습니다" << endl;
			cout << "프로그램을 종료하겠습니다. 아무키나 누르세요" << endl;
			_getch();*/
			return false;
		}
		ReadEOFile(in, vec);//photo file read

		//ofstream eo_ortho("Results\\Orthoimage_Results\\eo_ortho.txt");
		//eo_ortho << "%%Image Name\tGSD(m)\tX\tY\trow\tcol\n" << endl;

		int siz = 0;
		siz = int(vec.size());		// number of photos
		for (int k = 0; k < siz; k++)
		{
			img_name = vec[k].imagename;
			img_input = input_foldername + img_name;
			cout << endl;
			cout << img_name << " 처리 시작" << endl;

			srcImage = imread(img_input.c_str());

			pixel_cnt[0] = srcImage.rows;	// row
			pixel_cnt[1] = srcImage.cols;	// column/				
exit(0);*/
				return false;
			}

			if (vec[k].eo[2] < 0)
			{
				cout << "Z 값 : " << vec[k].eo[2] << endl;
				cout << k << "번째의 Estimate EO의 Z값이 0보다 작으니 확인하시기 바랍니다" << endl;
				cout << "프로그램을 종료하겠습니다. 아무키나 누르세요" << endl;
				_getch();
				exit(0);
			}

			double EO[6] = { vec[k].eo[0], vec[k].eo[1], vec[k].eo[2], vec[k].eo[3], vec[k].eo[4], vec[k].eo[5] };
			//gsd = (pixel_size * (EO[2] - ground_height)) / focal_length;
			gsd = 0.3;

			//Mat dstImage;
			//dstImage = orthophoto_m(srcImage, pixel_cnt, pixel_size, focal_length, EO, gsd, ground_height);

			// rotation matrix
			Mat R = rot3D(EO);

			// ground coverage
			cout << "vertex_g" << endl;
			double g_c[4];
			vertex_g(pixel_cnt, pixel_size, focal_length, EO, R, ground_height, g_c);

			double col_s = (g_c[1] - g_c[0]) / gsd;
			double row_s = (g_c[3] - g_c[2]) / gsd;

			// construct grid point
			//dem_m(g_c, floor(row_s), floor(col_s), gsd, ground_height, dem_x, dem_y, dem_z);

			double* x_fcs = new double[int(floor(row_s)*floor(col_s))];//x,y 지정			
double* y_fcs = new double[int(floor(row_s)*floor(col_s))];

			// construct grid point & re-calculate image coordinate
			cout << "image_coordinate" << endl;
			image_coordinate(g_c, gsd, ground_height, int(floor(row_s)), int(floor(col_s)), R, EO, focal_length, pixel_size, pixel_cnt, x_fcs, y_fcs);
			
			// save image pixel value
			cout << "pixel_color" << endl;
			Mat dstImage = pixel_color(pixel_cnt, x_fcs, y_fcs, int(floor(row_s)), int(floor(col_s)), srcImage);
			cvDestroyAllWindows();

			//char *windowname = "Output Image";
			//cvLoadWindowParameters(windowname);		
			
			//Size size(100, 605);
			/*Mat resizeImg;
			resize(dstImage, resizeImg, Size(), 0.5, 0.5);*/

			namedWindow("Output Image", CV_WINDOW_NORMAL);
			moveWindow("Output Image", 10, 0);
			resizeWindow("Output Image", 1050, 650);
			imshow("Output Image", dstImage);
			//imshow("Output Image", resizeImg);
			cvWaitKey(1);

			img_name.erase(img_name.end() - 4, img_name.end());
			img_output = output_foldername + img_name;
			//imwrite(img_output.c_str(), dstImage);
			imwrite(img_output.substr(0, 51) + ".png", dstImage);
			cout << img_name << " 저장 완료" << endl;

			cout << "writing .pgw" << endl;
			ofstream jgw(img_output.substr(0, 51) + ".pgw");
			jgw << gsd << endl << 0 << endl << 0 << endl << -gsd << endl << g_c[0] << endl << g_c[3];
			jgw.close();
			cout << img_name << " world file 저장 완료" << endl;

			//영상 저장 경로에서 영상을 다른 폴더로 이동
			string Oname = "C://uav_image/Result/"+ img_name + ".png";
			string Opath = "C://uav_image/Result/Temp/"+img_name+".png";
			const char *Fname = Oname.c_str();
			const char *Fpath = Opath.c_str();			
			//printf("%s \n", Fname);
			rename(Fname, Fpath);

			string Oname2 = "C://uav_image/Result/" + img_name + ".pgw";
			string Opath2 = "C://uav_image/Result/Temp/" + img_name + ".pgw";
			const char *Fname2 = Oname2.c_str();
			const char *Fpath2 = Opath2.c_str();
			//printf("%s \n", Fname2);
			rename(Fname2, Fpath2);
			
			/*const char *vis_img1 = Opath.c_str();*/


			/*cvDestroyWindow(vis_img1);
			IplImage* image = cvLoadImage(vis_img1, 1);
			IplImage* image2 = cvCreateImage(cvSize(image->width*0.4, image->height*0.4), IPL_DEPTH_8U, 3);
			cvResize(image, image2, CV_INTER_CUBIC);

			cvNamedWindow(vis_img1, CV_WINDOW_AUTOSIZE);
			cvMoveWindow(vis_img1, 600, 0);
			cvShowImage(vis_img1, image2);
			cvWaitKey(1);*/
		}
		return true;
	}

//Mat orthophoto_m(Mat srcImage, int* pixel_cnt, double pixel_size,
//	double focal_length, double* EO, double gsd, double ground_height)
//{
//	// rotation matrix
//	Mat R = rot3D(EO);
//		
//	// ground coverage
//	cout << "vertex_g" << endl;
//	double g_c[4];
//	vertex_g(pixel_cnt, pixel_size, focal_length, EO, R, ground_height, g_c);
//
//	double col_s = (g_c[1] - g_c[0]) / gsd;
//	double row_s = (g_c[3] - g_c[2]) / gsd;
//
//	// construct grid point
//	//dem_m(g_c, floor(row_s), floor(col_s), gsd, ground_height, dem_x, dem_y, dem_z);
//
//	double* x_fcs = new double[floor(row_s)*floor(col_s)];
//	double* y_fcs = new double[floor(row_s)*floor(col_s)];
//
//	// construct grid point & re-calculate image coordinate
//	cout << "image_coordinate" << endl;
//	image_coordinate(g_c, gsd, ground_height, floor(row_s), floor(col_s), R, EO, focal_length, pixel_size, pixel_cnt, x_fcs, y_fcs);
//
//	// save image pixel value
//	cout << "pixel_color" << endl;
//	Mat dstImage = pixel_color(pixel_cnt, x_fcs, y_fcs, floor(row_s), floor(col_s), srcImage);
//
//	return dstImage;
//}

Mat rot3D(double* EO)
{
	double	x;				// Angle		
	double	cos_x, sin_x;	// cos(Angle), sin(Angle)
	Mat	Rx, Ry, Rz, R;	// Rotational matrix

	// 1. Initialize the rotational matrix.
	Rx = Mat(3, 3, CV_64FC1);
	Ry = Mat(3, 3, CV_64FC1);
	Rz = Mat(3, 3, CV_64FC1);

	// 2. Manage R rotaion matrix
	R = Mat(3, 3, CV_64FC1);

	// Construct rotational matrix for Image#1
	
	//		|	1			0		0		|
	// Rx =	|	0		 cos(Om)	sin(Om)	|
	//		|	0		 -sin(Om)	cos(Om)	|		

	x = EO[3];
	cos_x = cos(x);
	sin_x = sin(x);
	
	double* rotX = (double*)Rx.data;
	rotX[0 * Rx.cols + 0] = 1.0;
	rotX[0 * Rx.cols + 1] = 0.0;
	rotX[0 * Rx.cols + 2] = 0.0;
	rotX[1 * Rx.cols + 0] = 0.0;
	rotX[1 * Rx.cols + 1] = cos_x;
	rotX[1 * Rx.cols + 2] = sin_x;
	rotX[2 * Rx.cols + 0] = 0.0;
	rotX[2 * Rx.cols + 1] = -sin_x;
	rotX[2 * Rx.cols + 2] = cos_x;

	/*Rx.at<double>(0, 0) = 1.0;
	Rx.at<double>(0, 1) = 0.0;
	Rx.at<double>(0, 2) = 0.0;
	Rx.at<double>(1, 0) = 0.0;
	Rx.at<double>(1, 1) = cos_x;
	Rx.at<double>(1, 2) = sin_x;
	Rx.at<double>(2, 0) = 0.0;
	Rx.at<double>(2, 1) = -sin_x;
	Rx.at<double>(2, 2) = cos_x;*/

	//		|	cos(Ph)		0		-sin(Ph)|
	// Ry =	|	  0			1		   0	|
	//		|	sin(Ph)		0		cos(Ph)	|

	x = EO[4];
	cos_x = cos(x);
	sin_x = sin(x);

	double* rotY = (double*)Ry.data;
	rotY[0 * Ry.cols + 0] = cos_x;
	rotY[0 * Ry.cols + 1] = 0.0;
	rotY[0 * Ry.cols + 2] = -sin_x;
	rotY[1 * Ry.cols + 0] = 0.0;
	rotY[1 * Ry.cols + 1] = 1.0;
	rotY[1 * Ry.cols + 2] = 0.0;
	rotY[2 * Ry.cols + 0] = sin_x;
	rotY[2 * Ry.cols + 1] = 0.0;
	rotY[2 * Ry.cols + 2] = cos_x;

	//		|	cos(Ka)		sin(Ka)		0	|
	// Rz =	|	-sin(Ka)	cos(Ka)		0	|
	//		|	  0			   0		1	|

	x = EO[5];	// Kappa
	cos_x = cos(x);
	sin_x = sin(x);

	double* rotZ = (double*)Rz.data;
	rotZ[0 * Rz.cols + 0] = cos_x;
	rotZ[0 * Rz.cols + 1] = sin_x;
	rotZ[0 * Rz.cols + 2] = 0.0;
	rotZ[1 * Rz.cols + 0] = -sin_x;
	rotZ[1 * Rz.cols + 1] = cos_x;
	rotZ[1 * Rz.cols + 2] = 0.0;
	rotZ[2 * Rz.cols + 0] = 0.0;
	rotZ[2 * Rz.cols + 1] = 0.0;
	rotZ[2 * Rz.cols + 2] = 1.0;

	R = Rz * Ry * Rx;

	return R;
}

void vertex_g(int* pixel_cnt, double pixel_size, double focal_length, double* EO, Mat R, double ground_height, double* g_c)
{
	// input
	//	int* pixel_cnt
	//	double pixel_size
	//	double focal_length
	//	double* EO
	//	double ground_height
	//	Mat R	
	// output
	//	double* g_c

	Mat image_xy;
	image_xy = Mat(4, 3, CV_64FC1);

	double* image_xy_data = (double*)image_xy.data;
	image_xy_data[0 * image_xy.cols + 0] = pixel_cnt[1] * pixel_size / 2;
	image_xy_data[0 * image_xy.cols + 1] = pixel_cnt[0] * pixel_size / 2;
	image_xy_data[0 * image_xy.cols + 2] = - focal_length;
	image_xy_data[1 * image_xy.cols + 0] = pixel_cnt[1] * pixel_size / 2;
	image_xy_data[1 * image_xy.cols + 1] = - pixel_cnt[0] * pixel_size / 2;
	image_xy_data[1 * image_xy.cols + 2] = - focal_length;
	image_xy_data[2 * image_xy.cols + 0] = - pixel_cnt[1] * pixel_size / 2;
	image_xy_data[2 * image_xy.cols + 1] = - pixel_cnt[0] * pixel_size / 2;
	image_xy_data[2 * image_xy.cols + 2] = - focal_length;
	image_xy_data[3 * image_xy.cols + 0] = - pixel_cnt[1] * pixel_size / 2;
	image_xy_data[3 * image_xy.cols + 1] = pixel_cnt[0] * pixel_size / 2;
	image_xy_data[3 * image_xy.cols + 2] = - focal_length;

	//cout << image_xy.at<double>(0, 0) << " " << image_xy.at<double>(0, 1) << " " << image_xy.at<double>(0, 2) << endl;
	//cout << image_xy.row(0).at<double>(0, 0) << " " << image_xy.row(0).at<double>(0, 1) << " " << image_xy.row(0).at<double>(0, 2) << " " << endl;

	Mat xy_ground;
	xy_ground = Mat(4, 2, CV_64FC1);

	double* xy_ground_data = (double*)xy_ground.data;

	// each row matrix of image coordinate matrix
	/*Mat image_xy_row;
	image_xy_row = Mat(1, 3, CV_64FC1);*/

	// ground corner coordinates from image corner coorinates
	Mat xy_g_coord;
	xy_g_coord = Mat(1, 2, CV_64FC1);
	
	double* xy_g_coord_data = (double*)xy_g_coord.data;
	
	for (int i = 0; i < 4; i++) {
		xy_g_coord = xy_g_min(EO, R, image_xy.row(i), ground_height);
		//cout << xy_g_coord.at<double>(0, 0) << " " << xy_g_coord.at<double>(0, 1) << endl;
		//cout << xy_g_coord_data[0] << " " << xy_g_coord_data[1] << endl;

		xy_g_coord_data[0] = xy_g_coord.at<double>(0, 0);
		xy_g_coord_data[1] = xy_g_coord.at<double>(0, 1);

		//cout << xy_g_coord_data[0] << " " << xy_g_coord_data[1] << endl;

		for (int j = 0; j < 2; j++) {
			xy_ground_data[i*xy_ground.cols + j] = xy_g_coord_data[0 * xy_g_coord.cols + j];
			//xy_ground_data[i*xy_ground.cols + j] = xy_g_coord.at<double>(0, j);
		}
	}
	//cout << xy_ground.at<double>(0, 0) << " " << xy_ground.at<double>(0, 1) << endl;
	//cout << xy_ground_data[0] << " " << xy_ground_data[1] << endl;

	double edge_x[4] = { xy_ground_data[0 * xy_ground.cols + 0], xy_ground_data[1 * xy_ground.cols + 0], 
		xy_ground_data[2 * xy_ground.cols + 0], xy_ground_data[3 * xy_ground.cols + 0] };
	double edge_y[4] = { xy_ground_data[0 * xy_ground.cols + 1], xy_ground_data[1 * xy_ground.cols + 1], 
		xy_ground_data[2 * xy_ground.cols + 1], xy_ground_data[3 * xy_ground.cols + 1] };

	// ground coverage
	g_c[0] = getMin(edge_x, 4);
	g_c[1] = getMax(edge_x, 4);
	g_c[2] = getMin(edge_y, 4);
	g_c[3] = getMax(edge_y, 4);

	//cout << g_c[0] << " " <<  g_c[1] << " " << g_c[2] << " " << g_c[3] << " " << endl;
}

Mat xy_g_min(double* EO, Mat R, Mat xy_image, double ground_height)
{	
	// inverse matrix
	Mat inv_R;
	inv_R = Mat(3, 3, CV_64FC1);
	inv_R = R.t();

	double* inv_R_data = (double*)inv_R.data;
	double* xy_image_data = (double*)xy_image.data;

	double scale = (ground_height - EO[2]) / (inv_R_data[2 * inv_R.cols + 0] * xy_image_data[0 * xy_image.cols + 0] +
		inv_R_data[2 * inv_R.cols + 1] * xy_image_data[0 * xy_image.cols + 1] +
		inv_R_data[2 * inv_R.cols + 2] * xy_image_data[0 * xy_image.cols + 2]);

	Mat sc;
	sc = Mat(3, 1, CV_64FC1);

	// transpose matrix
	Mat xy_image_t;
	xy_image_t = Mat(3, 1, CV_64FC1);
	xy_image_t = xy_image.t();

	// sc = inv_R * xy_image' 3 by 1
	sc = inv_R*xy_image_t;

	double* sc_data = (double*)sc.data;

	Mat xy_ground;
	xy_ground = Mat(1, 2, CV_64FC1);

	double* xy_ground_data = (double*)xy_ground.data;

	xy_ground_data[0] = EO[0] + scale*sc_data[0];
	xy_ground_data[1] = EO[1] + scale*sc_data[1];
	
	//cvmSet(xy_ground, 0, 0, EO[0] + scale*cvmGet(sc, 0, 0));
	//cvmSet(xy_ground, 0, 1, EO[1] + scale*cvmGet(sc, 1, 0));

	return xy_ground;
}

//MatND dem_m(double* g_c, int row_s, int col_s, double gsd, double ground_height)
//{
//	// grid point
//	int size[] = { row_s, col_s, 3 };
//	MatND dem;
//	dem = MatND(3, size, CV_64FC1);
//	
//	for (int i = 0; i < dem.size[0]; i++) {
//		for (int j = 0; j < dem.size[1]; j++) {
//			dem.at<double>(i, j, 0) = g_c[3] - i*gsd;
//			dem.at<double>(i, j, 1) = g_c[0] + j*gsd;
//			dem.at<double>(i, j, 2) = ground_height;
//
//			//cvSetReal3D(dem, i, j, 0, g_c[3] - i*gsd);
//			//cvSetReal3D(dem, i, j, 1, g_c[0] + j*gsd);
//			//cvSetReal3D(dem, i, j, 2, ground_height);
//		}
//	}
//
//	return dem;
//}

//void dem_m(double* g_c, int row_s, int col_s, double gsd, double ground_height, double* dem_x, double* dem_y, double* dem_z)
//{
//	// input
//	//	double* g_c - ground coverage
//	//	int row_s	- the number of rows
//	//	int col_s	- the number of cols
//	//	double gsd
//	//	double ground_height
//	// output
//	//	double* dem_x
//	//	double* dem_y
//	//	double* dem_z
//
//	for (int i = 0; i < row_s; i++) {
//		for (int j = 0; j < col_s; j++) {
//			dem_x[i*col_s + j] = g_c[3] - i*gsd;
//			dem_y[i*col_s + j] = g_c[0] + j*gsd;
//			dem_z[i*col_s + j] = ground_height;
//		}
//	}
//}

//MatND image_coordinate(MatND dem, Mat R, double* EO, double focal_length, double pixel_size, int* pixel_cnt)
//{
//	// fiducial point
//	int size[] = { dem.size[0], dem.size[1], dem.size[2] };
//	MatND xy_fcs;
//	xy_fcs = MatND(3, size, CV_64FC1);
//
//	double x_fcs = 0, y_fcs = 0;
//	
//	// ground point of each grid point
//	Mat xyz_ground;
//	xyz_ground = Mat(1, 3, CV_64FC1);
//
//	double x_ground, y_ground, z_ground;
//
//	// transpose matrix
//	Mat xyz_t;
//	xyz_t = Mat(3, 1, CV_64FC1);
//
//	// multiply matrix
//	Mat coll_pcs;
//	coll_pcs = Mat(3, 1, CV_64FC1);
//
//	Mat xy_image;
//	xy_image = Mat(3, 1, CV_64FC1);
//
//	for (int i = 0; i < xy_fcs.size[0]; i++) {
//		for (int j = 0; j < xy_fcs.size[1]; j++) {
//			x_ground = dem.at<double>(i, j, 1);
//			y_ground = dem.at<double>(i, j, 0);
//			z_ground = dem.at<double>(i, j, 2);
//			
//			/*x_ground = cvGetReal3D(dem, i, j, 1);
//			y_ground = cvGetReal3D(dem, i, j, 0);
//			z_ground = cvGetReal3D(dem, i, j, 2);*/
//
//			xyz_ground.at<double>(0, 0) = x_ground - EO[0];
//			xyz_ground.at<double>(0, 1) = y_ground - EO[1];
//			xyz_ground.at<double>(0, 2) = z_ground - EO[2];
//
//			/*cvmSet(xyz_ground, 0, 0, x_ground - EO[0]);
//			cvmSet(xyz_ground, 0, 1, y_ground - EO[1]);
//			cvmSet(xyz_ground, 0, 2, z_ground - EO[2]);*/
//				
//			//cvTranspose(xyz_ground, xyz_t);
//			xyz_t = xyz_ground.t();
//
//			// coll_pcs = R * xyz_t' 3 by 1
//			coll_pcs = R * xyz_t;
//
//			double scale = - coll_pcs.at<double>(2, 0) / focal_length;
//
//			xy_image.at<double>(0, 0) = coll_pcs.at<double>(0, 0) / scale / pixel_size;
//			xy_image.at<double>(1, 0) = coll_pcs.at<double>(1, 0) / scale / pixel_size;
//			xy_image.at<double>(2, 0) = coll_pcs.at<double>(2, 0) / scale / pixel_size;
//
//			/*cvmSet(xy_image, 0, 0, cvmGet(coll_pcs, 0, 0) / scale / pixel_size);
//			cvmSet(xy_image, 1, 0, cvmGet(coll_pcs, 1, 0) / scale / pixel_size);
//			cvmSet(xy_image, 2, 0, cvmGet(coll_pcs, 2, 0) / scale / pixel_size);*/
//
//			x_fcs = pixel_cnt[0] / 2 + xy_image.at<double>(1, 0);
//			y_fcs = pixel_cnt[1] / 2 - xy_image.at<double>(0, 0);
//
//			xy_fcs.at<double>(i, j, 0) = x_fcs;
//			xy_fcs.at<double>(i, j, 1) = y_fcs;
//
//			/*cvSetReal3D(xy_fcs, i, j, 0, x_fcs);
//			cvSetReal3D(xy_fcs, i, j, 1, y_fcs);*/
//		}
//	}
//
//	return xy_fcs;
//}

void image_coordinate(double* g_c, double gsd, double ground_height, int row_s, int col_s, Mat R, double* EO, double focal_length, double pixel_size, int* pixel_cnt, double* x_fcs, double* y_fcs)
{
	// input
	//	double* g_c
	//	double gsd
	//	double ground_height
	//	int row_s
	//	int col_s
	//	Mat R
	//	double* EO
	//	double focal_length
	//	double pixel_size
	//	int* pixel_cnt
	// output
	//	double* x_fcs
	//	double* y_fcs

	double* R_data = (double*)R.data;
	//cout << R_data[0] << " " << R_data[1] << " " << R_data[2] << endl;
	
	// ground point of each grid point
	/*Mat xyz_ground;
	xyz_ground = Mat(1, 3, CV_64FC1);

	double* xyz_ground_data = (double*)xyz_ground.data;*/
	double* xyz_ground = new double[3];

	double x_ground, y_ground, z_ground;

	// multiply matrix
	Mat coll_pcs;
	coll_pcs = Mat(3, 1, CV_64FC1);

	double* coll_pcs_data = (double*)coll_pcs.data;

	//Mat xy_image;
	//xy_image = Mat(3, 1, CV_64FC1);

	double* xy_image = new double[3];

	for (int i = 0; i < row_s; i++) {
		for (int j = 0; j < col_s; j++) {
			x_ground = g_c[0] + j*gsd;
			y_ground = g_c[3] - i*gsd;
			z_ground = ground_height;

			/*x_ground = cvGetReal3D(dem, i, j, 1);
			y_ground = cvGetReal3D(dem, i, j, 0);
			z_ground = cvGetReal3D(dem, i, j, 2);*/

			xyz_ground[0] = x_ground - EO[0];
			xyz_ground[1] = y_ground - EO[1];
			xyz_ground[2] = z_ground - EO[2];

			//cvTranspose(xyz_ground, xyz_t);
			//xyz_t = xyz_ground.t();

			// coll_pcs = R * xyz_t' 3 by 1
			//coll_pcs = R * xyz_t;
			for (int k = 0; k < 3; k++) {
				coll_pcs_data[k] = 0.0;
				for (int l = 0; l < 3; l++) {
					coll_pcs_data[k] += R_data[k * 3 + l] * xyz_ground[l];
				}
			}

			double scale = - coll_pcs_data[2] / focal_length;

			xy_image[0] = coll_pcs_data[0] / scale / pixel_size;
			xy_image[1] = coll_pcs_data[1] / scale / pixel_size;
			xy_image[2] = coll_pcs_data[2] / scale / pixel_size;

			/*cvmSet(xy_image, 0, 0, cvmGet(coll_pcs, 0, 0) / scale / pixel_size);
			cvmSet(xy_image, 1, 0, cvmGet(coll_pcs, 1, 0) / scale / pixel_size);
			cvmSet(xy_image, 2, 0, cvmGet(coll_pcs, 2, 0) / scale / pixel_size);*/

			x_fcs[i*col_s + j] = pixel_cnt[0] / 2 + xy_image[1];
			y_fcs[i*col_s + j] = pixel_cnt[1] / 2 - xy_image[0];
		}
	}
}

//Mat pixel_color(int* pixel_cnt, MatND xy_fcs, Mat srcImage)
//{
//	int row = xy_fcs.size[0];
//	int col = xy_fcs.size[1];
//
//	//Mat em_b, em_g, em_r;
//	Mat FB, FG, FR, dstImage;
//	//Mat bgr[3];
//	vector<Mat> bgr;
//	vector<Mat> channels;
//	
//	// split the image into channel bgr
//	split(srcImage, bgr);
//	/*Mat test;
//	merge(bgr, test);
//	imwrite("merge.jpg", test);
//	imwrite("o.jpg", srcImage);
//	imwrite("b.jpg", bgr[0]);
//	imwrite("g.jpg", bgr[1]);
//	imwrite("r.jpg", bgr[2]);*/
//
//	FB = Mat(row, col, CV_8UC1);
//	FG = Mat(row, col, CV_8UC1);
//	FR = Mat(row, col, CV_8UC1);
//
//	double outOfBoundsRow, outOfBoundsCol;
//	for (int i = 0; i < row; i++) {
//		for (int j = 0; j < col; j++) {
//			outOfBoundsRow = xy_fcs.at<double>(row - (i + 1), col - (j + 1), 0);
//			outOfBoundsCol = xy_fcs.at<double>(row - (i + 1), col - (j + 1), 1);
//
//			if (outOfBoundsRow < 0 || outOfBoundsRow > pixel_cnt[0])
//				continue;
//			else if (outOfBoundsCol < 0 || outOfBoundsCol > pixel_cnt[1])
//				continue;
//			else {
//				FB.at<uchar>(i, j) = *bgr[0].ptr<uchar>(int(outOfBoundsRow), int(outOfBoundsCol));
//				FG.at<uchar>(i, j) = *bgr[1].ptr<uchar>(int(outOfBoundsRow), int(outOfBoundsCol));
//				FR.at<uchar>(i, j) = *bgr[2].ptr<uchar>(int(outOfBoundsRow), int(outOfBoundsCol));
//			}
//		}
//	}
//
//	channels.push_back(FB);
//	channels.push_back(FG);
//	channels.push_back(FR);
//
//	dstImage = Mat(row, col, CV_8UC3);
//	merge(channels, dstImage);
//
//	return dstImage;
//}

Mat pixel_color(int* pixel_cnt, double* x_fcs, double* y_fcs, int row_s, int col_s, Mat srcImage)
{
	//Mat em_b, em_g, em_r;
	Mat FB, FG, FR, dstImage, FA;
	//Mat bgr[3];
	vector<Mat> bgr;
	vector<Mat> channels;

	// split the image into channel bgr
	split(srcImage, bgr);
	uchar* b_data = (uchar*)bgr[0].data;
	uchar* g_data = (uchar*)bgr[1].data;
	uchar* r_data = (uchar*)bgr[2].data;

	/*FB = Mat(row_s, col_s, CV_8UC1);
	FG = Mat(row_s, col_s, CV_8UC1);
	FR = Mat(row_s, col_s, CV_8UC1);
	FA = Mat(row_s, col_s, CV_8UC1);*/

	FB = Mat::zeros(row_s, col_s, CV_8UC1);
	FG = Mat::zeros(row_s, col_s, CV_8UC1);
	FR = Mat::zeros(row_s, col_s, CV_8UC1);
	FA = Mat::zeros(row_s, col_s, CV_8UC1);

	uchar* FB_data = (uchar*)FB.data;
	uchar* FG_data = (uchar*)FG.data;
	uchar* FR_data = (uchar*)FR.data;
	uchar* FA_data = (uchar*)FA.data;

	double outOfBoundsRow, outOfBoundsCol;
	for (int i = 0; i < row_s; i++) {
		for (int j = 0; j < col_s; j++) {
			outOfBoundsRow = x_fcs[(int)(row_s*col_s - j - i*col_s)];
			outOfBoundsCol = y_fcs[(int)(row_s*col_s - j - i*col_s)];

			if (outOfBoundsRow < 0 || outOfBoundsRow > pixel_cnt[0])
				continue;
			else if (outOfBoundsCol < 0 || outOfBoundsCol > pixel_cnt[1])
				continue;
			else {
				//cout << int(outOfBoundsRow) << "\t" << int(outOfBoundsCol) << endl;
				FB_data[i*col_s + j] = *bgr[0].ptr<uchar>(int(outOfBoundsRow), int(outOfBoundsCol));
				FG_data[i*col_s + j] = *bgr[1].ptr<uchar>(int(outOfBoundsRow), int(outOfBoundsCol));
				FR_data[i*col_s + j] = *bgr[2].ptr<uchar>(int(outOfBoundsRow), int(outOfBoundsCol));
				FA_data[i*col_s + j] = 255;
				
				/*FB_data[i*col_s + j] = b_data[int(outOfBoundsRow)*int(outOfBoundsCol) + int(outOfBoundsCol)];
				FG_data[i*col_s + j] = g_data[int(outOfBoundsRow)*int(outOfBoundsCol) + int(outOfBoundsCol)];
				FR_data[i*col_s + j] = r_data[int(outOfBoundsRow)*int(outOfBoundsCol) + int(outOfBoundsCol)];*/
			}
		}
	}	

	channels.push_back(FB);
	channels.push_back(FG);
	channels.push_back(FR);
	channels.push_back(FA);

	dstImage = Mat(row_s, col_s, CV_8UC4);
	merge(channels, dstImage);

	return dstImage;
}

double getMax(double* n, int size) {
	double max = n[0];

	for (int i = 1; i < size; i++)
		if (n[i] > max) max = n[i];

	return max;
}

double getMin(double* n, int size) {
	double min = n[0];

	for (int i = 1; i < size; i++)
		if (n[i] < min) min = n[i];

	return min;
}

istream& ReadEOFile(istream& in, vector<Data>& vec)
{
	if (in) {
		vec.clear();
		Data d;
		while (in >> d.imagename >> d.eo[0] >> d.eo[1] >> d.eo[2] >> d.eo[3] >> d.eo[4] >> d.eo[5])
			vec.push_back(d);
		in.clear();
	}
	return in;
}

//istream& ReadROIFile(istream& in, vector<Roi>& vec)
//{
//	if (in) {
//		vec.clear();
//		Roi d;
//		while (in >> d.imagename >> d.pixelXY[0] >> d.pixelXY[1] >> d.pixelXY[2] >> d.pixelXY[3])
//			vec.push_back(d);
//		in.clear();
//	}
//	return in;
//}
