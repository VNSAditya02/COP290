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
void writeInFile(vector<double> y, string filename);
int DynamicDensity(const string& location, int nth_frame);
double count (Mat frame, int rows, int cols, int threshold);
int writeOut();

const String OUT_PATH = "out";
const char pathSeparator =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif

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

    // Initialize
    int i = 0;

    // frame_1 stores the previous frame
    Mat frame;

    // read the previous frame
    capture >> frame;

    // Crop the previous frame
    frame = CropImage(frame);

    // Stores the output values
    vector <double> y;

    while(true)
    {
        // frame_2 is the current frame
        // out is the output frame which detects moving objects
        Mat frame2, out;

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

            // Finds the difference between the current and previous frames
            absdiff(frame2, frame, out);

            // rows denotes no of rows in output frame, col denotes no of columns in output frame
            int rows = out.rows;
            int cols = out.cols;

            // Counts no of pixels whose pixel value is greater than 10
            double count_pix = count(out, rows, cols, 10);

            // Add the output to the output vector
            y.push_back(count_pix);

            // Changes current frame to previous frame
            frame = frame2;
        }

        else{
            capture >> frame2;
        }

        i++;
    }
    // Find density of the output value
    writeInFile(y, OUT_PATH + pathSeparator + "DynamicDensity.txt");

    return 0;
}

 /**
  *
  * @param location : location of the video file
  * @param nth_frame
  * @return
  */
int QueueDensity(const string& location, int nth_frame){

    // Opens the empty frame
    Mat src_colour = imread("../COP290/assets/empty_final.jpg");

    // Finds the cropped image of the input image
    Mat src_crop = CropImage(src_colour);

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

    // Stores the value of no of contours in a frame
    std::vector<double> y;

    while (true)
    {
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

            // Finds the difference between the present frame and empty frame
            absdiff(frame, src_crop, fgMask);

            // rows denotes no of rows in output frame, col denotes no of columns in output frame
            int row = fgMask.rows;
            int col = fgMask.cols;

            // Counts no of pixels whose pixel value is greater than 15
            double x =count(fgMask, row, col, 15);

            // Add the output to the output vector 
            y.push_back(x);
            
        }
        else
            capture >> frame;
        
        i++;
    }

    // Calls Density function
     writeInFile(y, OUT_PATH + pathSeparator + "QueueDensity.txt");

    return 0;
}

/**
 * Given a frame, this frame finds the no of pixels whose pixel value is greater than a threshold
 * @param frame
 * @param row: no of rows in the frame
 * @param cols: no of columns in the frame
 */
double count (Mat frame, int rows, int cols, int threshold){

    int count = 0;
    for (int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if(frame.at<uchar>(j, i) > threshold){
                count++;
            }
        }
    }
    return (double)count/double(rows*cols);
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
void writeInFile(vector<double> y, string filename){

    // Creates file and stores output value in out.txt
    ofstream MyFile(filename);

    // Traverse the vector and adds the values to the output file
    for (double it: y) {
        MyFile << it << endl;
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