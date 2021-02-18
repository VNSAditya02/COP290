#include <opencv2/opencv.hpp>
#include <iostream>
#include <boost/program_options.hpp>

using namespace cv;
using namespace std;
namespace po = boost::program_options;

void project_crop(const Mat& img_src_1, const Mat& img_src_2, const vector<Point2f>& src_pts, const vector<Point2f>& dest_pts);

/**
 * Serves as an entry point for the code execution
 * @param argc
 * @param argv
 * @return
 */
int  main(int argc, char* argv[]){

    // loads image (empty road)
	Mat img_src_1 = imread("./empty1.jpg");

    // loads image (road with vehicles)
	Mat img_src_2 = imread("./traffic1.png");

    //Approximated points (Should be selected by mouse)
	vector<Point2f> src_pts;

	//These co-ordinates should be selected by mouse. For verification, I approximated these points
	src_pts.emplace_back(979, 215);
	src_pts.emplace_back(258, 1074);
	src_pts.emplace_back(1552, 1070);
	src_pts.emplace_back(1264, 224);

	//These points are taken from Riju Ma'am's website
	vector<Point2f> dest_pts;
	dest_pts.emplace_back(472, 52);
	dest_pts.emplace_back(472, 830);
	dest_pts.emplace_back(800, 830);
	dest_pts.emplace_back(800, 52);

	// Calls the function project_crop which projects and crops the given image as desired.
	project_crop(img_src_1, img_src_2, src_pts, dest_pts);
	waitKey(0);
}

/**
 * This function takes the two images and produces the projected and cropped images
 * @param img_src_1 empty road image file
 * @param img_src_2 road with vehicles image file
 * @param src_pts points selected by mouse
 * @param dest_pts points given in Riju Ma'am website
 */
void project_crop(const Mat& img_src_1, const Mat& img_src_2, const vector<Point2f>& src_pts, const vector<Point2f>& dest_pts){
    // Finding Homography between the points
    Mat homography = findHomography(src_pts, dest_pts);

	//Initializing matrices.
	//img_wrap_1 is projected image for empty road, img_wrap_2 is projected image for road with vehicles.
	//img_crop_1 is cropped image for empty road, img_crop_2 is cropped image for road with vehicles
	Mat img_warp_1, img_warp_2, img_crop_1, img_crop_2;

	//Finding projected images using "wrapPerspective" function. 
	//For projected image, size of src_image and projected image should be same 
	warpPerspective(img_src_1, img_warp_1, homography, img_src_1.size());
	warpPerspective(img_src_2, img_warp_2, homography, img_src_2.size());

	// Finding cropped image. Function: Rect(x_min, y_min, x_max - x_min, y_max - y_min)
	// These values (x_min,.....) are calculated from the values given on Riju Ma'am's website.
	// We Can calculate these values from dest_pts but these values are constant as far as I know!
	img_crop_1 = img_warp_1(Rect(472, 52, 328, 778)); 
	img_crop_2 = img_warp_2(Rect(472, 52, 328, 778));

	//Displays images
	imshow("warp_img_1: ", img_warp_1);
	imshow("warp img_2: ", img_warp_2);
	imshow("crop img_1: ", img_crop_1);
	imshow("crop img_2: ", img_crop_2);
}