/**
 * @file traffic_density.cpp
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
#include <fstream> 
#include <bits/stdc++.h>
#include <boost/program_options.hpp>
#include <experimental/filesystem>

using namespace cv;
using namespace std;
namespace po = boost::program_options;

int QueueDensity(const string& location, int nth_frame);
Mat CropImage(Mat frame);
void findDensity(vector<int> y, string filename);
int DynamicDensity(const string& location, int nth_frame);
int writeOut();

static void on_low_H_thresh_trackbar(int, void *);
static void on_high_H_thresh_trackbar(int, void *);
static void on_low_S_thresh_trackbar(int, void *);
static void on_high_S_thresh_trackbar(int, void *);
static void on_low_V_thresh_trackbar(int, void *);
static void on_high_V_thresh_trackbar(int, void *);

// Used to find optimum values of inRange() function, line: 161
const int MAX_VALUE_H = 255;
const int MAX_VALUE = 255;
const int VIDEO_FRAME_RATE = 15;
const String WINDOW_CAPTURE_NAME = "Video Capture";
const String WINDOW_DETECTION_NAME = "Object Detection";
const String OUT_PATH = "out";
const char pathSeparator =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif

int low_H = 0, low_S = 0, low_V = 0;
int high_H = MAX_VALUE_H, high_S = MAX_VALUE, high_V = MAX_VALUE;

/**
 * Location of the input video file has to be given by command line argument
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    string vid_file;
    int nth_frame, parallel;
    desc.add_options()
            ("help,h", "Print help message")
            ("file,f", po::value(&vid_file)->required(),"Path to Video file")
            ("nframes,n", po::value<int>(&nth_frame)->default_value(3), "Decides FPS by processing "
                                                                        "every Nth frame of the video. Default Value: 3")
            ("threads,t", po::value<int>(&parallel)->default_value(0), "Set 1 for Parallel execution "
                                   "using OpenMP and 0 for Serial Execution. Default Value: 0. Type export OMP_NUM_THREADS=2 in terminal before compiling.")
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

    // Create directory for outfiles if it doesn't exist, else ignore
    experimental::filesystem::path dir(OUT_PATH);
    if(!(experimental::filesystem::exists(dir))){
        if (experimental::filesystem::create_directory(dir))
            cout << "Created out directory successfully!" << endl;
    }

    // start time
    auto t1 = chrono::high_resolution_clock::now();

    if(parallel == 0) { // Serial code execution
        QueueDensity(vid_file, nth_frame);

        auto t2 = chrono::high_resolution_clock::now();
        cout << "Time taken by Static queue density: " <<
             chrono::duration_cast<chrono::seconds>(t2 - t1).count() << " s\n";

        DynamicDensity(vid_file, nth_frame);
        auto t3 = chrono::high_resolution_clock::now();
        cout << "Time taken by Dynamic queue density: " <<
             chrono::duration_cast<chrono::seconds>(t3 - t2).count() << " s\n";
    }
    else { // OpenMP to Parallise function execution. Input export OMP_NUM_THREADS=2 to use
        #pragma omp parallel default(none) shared(vid_file, nth_frame)
        #pragma omp single
        {
            #pragma omp task
            QueueDensity(vid_file, nth_frame);

            #pragma omp task
            DynamicDensity(vid_file, nth_frame);
        }

        auto t2 = chrono::high_resolution_clock::now();
        cout << "Time taken for Parallelised Execution: " <<
             chrono::duration_cast<chrono::seconds>( t2 - t1 ).count()<<" s\n";
    }
    writeOut();

    return 0;
}

/**
 *
 * @param location : location where video file is located
 * @param nth_frame
 * @return Finds the density of moving objects
 */
int DynamicDensity(const string& location, int nth_frame){

    // Opens the video file
    VideoCapture capture(location);

    // If not able to open the video file, exit the program
    if (!capture.isOpened()){
        cout << "Unable to open the video file" << endl;
        cin.get();
        return -1;
    }

    // Create Background subtractor object
    Ptr<BackgroundSubtractor> backSub;
    backSub = createBackgroundSubtractorMOG2();

    // Initialize
    int i = 0;

    // frame_1 stores the previous frame
    Mat frame, frame_1;

    // read the previous frame
    capture >> frame;

    // Crop the previous frame
    frame = CropImage(frame);

    // Subtract the background
    backSub -> apply(frame, frame_1);

    // Stores the output values
    vector <int> y;

    while(true)
    {
        // frame_2 is the current frame
        // frame_threshold is the output frame which detects moving objects
        Mat frame2, frame_2, frame_threshold, out, magnitude, angle, magn_norm;

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        // Change the condition accordingly, i % 1 process every frame, i % 3 process every third frame
        if (i % nth_frame == 0){
            // Read the current frame
            capture >> frame2;
            // If frame is empty, exit the loop
            if (frame2.empty()){
                break;
            }
            // Crop the present frame
            frame2 = CropImage(frame2);

            // Uncomment the following lines to change the values in Line: 161; inRange() function

            /*
            namedWindow(WINDOW_CAPTURE_NAME);
            namedWindow(WINDOW_DETECTION_NAME);
            createTrackbar("Low H", WINDOW_DETECTION_NAME, &low_H, MAX_VALUE_H, on_low_H_thresh_trackbar);
            createTrackbar("High H", WINDOW_DETECTION_NAME, &high_H, MAX_VALUE_H, on_high_H_thresh_trackbar);
            createTrackbar("Low S", WINDOW_DETECTION_NAME, &low_S, MAX_VALUE, on_low_S_thresh_trackbar);
            createTrackbar("High S", WINDOW_DETECTION_NAME, &high_S, MAX_VALUE, on_high_S_thresh_trackbar);
            createTrackbar("Low V", WINDOW_DETECTION_NAME, &low_V, MAX_VALUE, on_low_V_thresh_trackbar);
            createTrackbar("High V", WINDOW_DETECTION_NAME, &high_V, MAX_VALUE, on_high_V_thresh_trackbar);
            */

            // flow stores
            Mat optflow(frame_1.size(), CV_32FC2);

            // Apply Background subtractor for current frame
            backSub -> apply(frame2, frame_2);

            // Computes the optical flow between frames
            calcOpticalFlowFarneback(frame_1, frame_2, optflow, 0.5, 3, 15, 3, 5, 1.2, 0);

            Mat flow_parts[2];

            // Splits optflow into two parts
            split(optflow, flow_parts);

            // Finds cartersian co-ordinates between two matrices
            cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);

            // Normalizes the magnitude
            normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);

            angle *= ((1.f / 360.f) * (180.f / 255.f));

            //builds hsv image
            Mat hsv[3], hsv_img, hsv_final, color_img;
            hsv[0] = angle;
            hsv[1] = Mat::ones(angle.size(), CV_32F);
            hsv[2] = magn_norm;
            merge(hsv, 3, hsv_img);
            hsv_img.convertTo(hsv_final, CV_8U, 255.0);

            // Convert hsv image to color image
            cvtColor(hsv_final, color_img, COLOR_HSV2BGR);

            // Convert color image to grayscale
            cvtColor(color_img, out, COLOR_BGR2GRAY);

            // Detects pixels whose pixel values lies in the given range
            // These values are found using trackbars. I found these values as optimum for finding moving  objects
            inRange(out, Scalar(40,45,50), Scalar(125,149,180), frame_threshold);

            // Detect edges of moving objects
            Canny( frame_threshold, out, 100, 200);

            // Finds contours of the detected edges
            findContours(out.clone(), contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

            // Add contours size to output vector
            y.push_back(contours.size());

            // Current slides becomes next slide for the next iteration
            frame_1 = frame_2;
        }
        else
            capture >> frame_2;
        i++;
    }
    // Find density of the output value
    findDensity(y, OUT_PATH + pathSeparator + "DynamicDensity.txt");
    return 0;
}

 /**
  *
  * @param location : location of the video file
  * @param nth_frame
  * @return
  */
int QueueDensity(const string& location, int nth_frame){

    // Opens the video file
    VideoCapture capture(location);

    // If not able to open the video file, exit the program
    if (!capture.isOpened()){
        cout << "Unable to open the video file" << endl;
        cin.get();
        return -1;
    }

    // Initialize matrices frame, fgMask
    int i = 1;

    Mat frame, fgMask;

    // threshold value used in edge detector
    int thresh = 100;

    // Stores the value of no of contours in a frame
    std::vector<int> y;

    while (true)
    {
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        // Change the condition accordingly, i % 1 process every frame, i % 3 process every third frame
        if (i % nth_frame == 0)
        {
            // If true, read the frame
            capture >> frame;

            // If the frame is empty, exit the loop
            if (frame.empty())
                break;

            // Finds the cropped image of the given frame
            frame = CropImage(frame);

            // Detects edges and applies Mask
            Canny( frame, fgMask, thresh, thresh*2 );

            // Find contours
            findContours(fgMask.clone(), contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
            
            // Add the contours size to y
            y.push_back(contours.size());
            // cout << contours.size() << endl;
        }
        else
            capture >> frame;
        
        i++;
    }

    // Calls Density function
     findDensity(y, OUT_PATH + pathSeparator + "QueueDensity.txt");

    return 0;
}

/**
 * Given a frame, this function crops and outputs the cropped frame
 * @param frame
 * @return : frame to be cropped
 */
Mat CropImage(Mat frame){

    // Stores the wrapped image
    Mat img_warp, img_crop;

    // These points are taken from Riju Ma'am's website
    vector<Point2f> dest_pts;
    dest_pts.emplace_back(Point2f(472, 52));
    dest_pts.emplace_back(Point2f(472, 830));
    dest_pts.emplace_back(Point2f(800, 830));
    dest_pts.emplace_back(Point2f(800, 52));

    // This part of the image is our area of interest. I approximated these points
    vector<Point2f> src_pts;
    src_pts.emplace_back(1000, 216);
    src_pts.emplace_back(286, 1060);
    src_pts.emplace_back(1552, 1070);
    src_pts.emplace_back(1264, 200);

    // Finding Homography between the points
    Mat homography = findHomography(src_pts, dest_pts);

    // Convert frame to Grayscale
    cvtColor(frame, frame, COLOR_BGR2GRAY);

    // Convert the frame into cropped image
    warpPerspective(frame, img_warp, homography, frame.size());
    img_crop = img_warp(Rect(472, 52, 328, 778));

    // returns the cropped image
    return img_crop;
}

/**
 * Finds the Density of the given values and stores it in the output file
 * @param y : integer Vector
 * @param filename : name of the file in which output is to be stored
 */
void findDensity(vector<int> y, string filename){
    // Stores the maximum value of the vector
    int max_val = *max_element(y.begin(), y.end());

    // Stores the minimum value of the vector
    int min_val = *min_element(y.begin(), y.end());

    double z;
    int range = max_val - min_val;

    // Creates file and stores output value in out.txt
    ofstream MyFile(filename);

    // Traverse the vector and find the normalized values
    for (int it: y) {
        z = (double)(it - min_val)/(double)range;
        MyFile << z << endl;
    }

    // Closes the output file
    MyFile.close();
}

/**
 * This methods writes the values to out.csv
 * @return
 */
int writeOut() {
    ofstream out_file(OUT_PATH + pathSeparator + "out.csv");
    ifstream queue(OUT_PATH + pathSeparator + "QueueDensity.txt");
    ifstream dynamic(OUT_PATH + pathSeparator + "DynamicDensity.txt");
    out_file << "framenum, queue density, dynamic density\n";
    cout << "framenum, queue density, dynamic density\n";
    int i=0;

    if (!queue || !dynamic)
    {
        std::cout << "Error opening file " << (queue ? "Dynamic": "Queue") << ": " << strerror(errno) << '\n';
        return 1;
    }

    do
    {
        string line1, line2;
        if(!getline(queue, line1))
            line1 = "0";
        if(!getline(dynamic, line2))
            line2 = "0";
        string res = to_string(i) + ", " + line1 + ", " + line2 + "\n";
        cout << res;
        out_file << res;
        i++;
    } while(queue || dynamic);

    out_file.close();
    queue.close();
    dynamic.close();
    return 0;
}

/*

NOTE : I copy pasted these functions from a website, just to find optimum values for inRange() function
       Should remove these functions before submitting

*/

static void on_low_H_thresh_trackbar(int, void *)
{
    low_H = min(high_H-1, low_H);
    setTrackbarPos("Low H", WINDOW_DETECTION_NAME, low_H);
}
static void on_high_H_thresh_trackbar(int, void *)
{
    high_H = max(high_H, low_H+1);
    setTrackbarPos("High H", WINDOW_DETECTION_NAME, high_H);
}
static void on_low_S_thresh_trackbar(int, void *)
{
    low_S = min(high_S-1, low_S);
    setTrackbarPos("Low S", WINDOW_DETECTION_NAME, low_S);
}
static void on_high_S_thresh_trackbar(int, void *)
{
    high_S = max(high_S, low_S+1);
    setTrackbarPos("High S", WINDOW_DETECTION_NAME, high_S);
}
static void on_low_V_thresh_trackbar(int, void *)
{
    low_V = min(high_V-1, low_V);
    setTrackbarPos("Low V", WINDOW_DETECTION_NAME, low_V);
}
static void on_high_V_thresh_trackbar(int, void *)
{
    high_V = max(high_V, low_V+1);
    setTrackbarPos("High V", WINDOW_DETECTION_NAME, high_V);
}