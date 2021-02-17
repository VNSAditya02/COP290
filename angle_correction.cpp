#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void project_crop(Mat img_src_1, Mat img_src_2, vector<Point2f> src_pts, vector<Point2f> dest_pts);

int  main(int argc, char* argv[]){
	Mat img_src_1 = imread("/home/aditya/Downloads/empty1.jpg"); //loads image (empty road)
	Mat img_src_2 = imread("/home/aditya/Downloads/traffic1.png"); //loads image (road with vehicles)
	vector<Point2f> src_pts; //Approximated points (Should be selected by mouse)
	//These co-ordinates should be selected by mouse. For verification, I approximated these points
	src_pts.push_back(Point2f(979, 215));
	src_pts.push_back(Point2f(258, 1074));
	src_pts.push_back(Point2f(1552, 1070));
	src_pts.push_back(Point2f(1264, 224));
	//These points are given in Riju Ma'am's website (xD)
	vector<Point2f> dest_pts;
	dest_pts.push_back(Point2f(472, 52));
	dest_pts.push_back(Point2f(472, 830));
	dest_pts.push_back(Point2f(800, 830));
	dest_pts.push_back(Point2f(800, 52));
	//Calls the function project_crop which projects and crops the given image as desired.
	project_crop(img_src_1, img_src_2, src_pts, dest_pts);
	waitKey(0);
}
//The below function takes the two images and produces the projected and cropped images
//img_src_1 is empty road, img_src_2 is road with vehicles
//src_pts are points selected by mouse
//dest_pts are points given in Riju Ma'am website
void project_crop(Mat img_src_1, Mat img_src_2, vector<Point2f> src_pts, vector<Point2f> dest_pts){
	Mat homography = findHomography(src_pts, dest_pts); //Finding Homography between the points
	//Initializing matrices.
	//img_wrap_1 is projected image for empty road, img_wrap_2 is projected image for road with vehicles.
	//img_crop_1 is cropped image for empty road, img_crop_2 is cropped image for road with vehicles
	Mat img_warp_1, img_warp_2, img_crop_1, img_crop_2; 
	//Finding projected images using "wrapPerspective" function. 
	//For projected image, size of src_image and projected image should be same 
	warpPerspective(img_src_1, img_warp_1, homography, img_src_1.size());
	warpPerspective(img_src_2, img_warp_2, homography, img_src_2.size());
	//Finding cropped image. Function: Rect(x_min, y_min, x_max - x_min, y_max - y_min)
	//These values (x_min,.....) are calculated from the values given on Riju Ma'am's website.
	//We Can calculate these values from dest_pts but these values are constant as far as I know! 
	img_crop_1 = img_warp_1(Rect(472, 52, 328, 778)); 
	img_crop_2 = img_warp_2(Rect(472, 52, 328, 778));
	//Displays images
	imshow("warp_img_1: ", img_warp_1);
	imshow("warp img_2: ", img_warp_2);
	imshow("crop img_1: ", img_crop_1);
	imshow("crop img_2: ", img_crop_2);
}