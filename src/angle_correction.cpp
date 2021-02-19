/**
 * @file angle_correction.cpp
 *
 * @brief This file outputs a warped and a cropped Homomorphised and greyscale version of the input RGB Image
 *
 * @ingroup COP290
 *
 * @authors Pratyush Pandey, VNS Aditya
 * Contact: ee1170938@iitd.ac.in, cs5190471@iitd.ac.in
 *
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/program_options.hpp>

using namespace cv;
using namespace std;
namespace po = boost::program_options;

void mouseHandler(int event,int x,int y, int flags,void* param);
void project_crop(const Mat& img_src, const vector<Point2f>& src_pts, const vector<Point2f>& dest_pts);

struct clickData {
    Mat img;
    vector<Point2f> points;
};

/**
 *
 * setMouseCallback function for openCV - this offers a visual representation of your clicks on the image file
 *
 * @param event Mouse Event
 * @param x Positional Co-ordinate
 * @param y Positional Co-ordinate
 * @param flags
 * @param param
 */
void mouseHandler(int event, int x, int y, int flags, void* param)
{
    if( event == EVENT_LBUTTONDOWN )
    {
        clickData *data = ((clickData *) param);
        circle(data->img, Point(x, y), 3, Scalar(0,255,255), 5, cv::LINE_AA);
        imshow("Image", data->img);
        if (data->points.size() < 4)
        {
            data->points.emplace_back(Point2f(x,y));
        }
    }
}

/**
 *
 * Serves as an entry point for the code execution
 *
 * @param argc
 * @param argv
 * @return
 */
int  main(int argc, char* argv[]){

    string img_file;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "print usage message")
            ("file,f", po::value(&img_file),"Path to Image file")
            ;
    po::variables_map vm;
    try {
        po::store(parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch(exception& e) {
        cout << desc << "\n";
        return 0;
    }

    if (vm.count("help")) {
        cout << desc << "\n";
        return 0;
    }

    // load the image
	Mat img_src_colour = imread(img_file);
    Mat img_src;

    // In case of a wrong image source or wrong use of command line arguments
    if (!img_src_colour.data) {
        cout << "Could not open the image file." << endl;
        cout << desc << "\n";
        return -1;
    }

    // convert RGB Image to grayscale
    cvtColor(img_src_colour, img_src, cv::COLOR_BGR2GRAY);

    Mat img_temp = img_src.clone();

    // Source Points: selected by mouse
    clickData data;
    data.img = img_temp;

    string window_name = "Image";
    namedWindow(window_name, WINDOW_AUTOSIZE);
    setMouseCallback(window_name, mouseHandler, &data);
    imshow(window_name, img_temp);
    waitKey(0);

	// These points are taken from Riju Ma'am's website
	vector<Point2f> dest_pts;
	dest_pts.emplace_back(Point2f(472, 52));
	dest_pts.emplace_back(Point2f(472, 830));
	dest_pts.emplace_back(Point2f(800, 830));
	dest_pts.emplace_back(Point2f(800, 52));

	// Calls the function project_crop which projects and crops the given image as desired.
    project_crop(img_src, data.points, dest_pts);
	waitKey(0);

	// Clean Up
    img_src_colour.release();
	img_src.release();
	img_temp.release();

	return 0;
}

/**
 *
 * This function takes the two images and produces the projected and cropped images
 *
 * @param img_src empty road image file
 * @param src_pts points selected by mouse
 * @param dest_pts points given in Riju Ma'am website
 */
void project_crop(const Mat& img_src, const vector<Point2f>& src_pts, const vector<Point2f>& dest_pts){
    // Finding Homography between the points
    Mat homography = findHomography(src_pts, dest_pts);

	// Initializing matrices.
	// img_wrap_1 is projected image for empty road, img_wrap_2 is projected image for road with vehicles.
	// img_crop is cropped image for empty road, img_crop_2 is cropped image for road with vehicles
	Mat img_warp, img_crop;

	// Finding projected images using "wrapPerspective" function.
	// For projected image, size of src_image and projected image should be same
	warpPerspective(img_src, img_warp, homography, img_src.size());

	// Finding cropped image. Function: Rect(x_min, y_min, x_max - x_min, y_max - y_min)
	// These values (x_min,.....) are calculated from the values given on Riju Ma'am's website.
	// We Can calculate these values from dest_pts but these values are constant as far as I know!
	img_crop = img_warp(Rect(472, 52, 328, 778));

	//Displays images
	imshow("Warped Image: ", img_warp);
	imshow("Cropped Image: ", img_crop);
    imwrite("../assets/Wrapped Image.jpg", img_warp);
}