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
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

using namespace cv;
using namespace std;
namespace po = boost::program_options;

int QueueDensity(const string& location, int nth_frame, int num_threads);
Mat CropImage(Mat frame, int c1, int c2, int c3, int c4, int r1, int r2, int r3, int r4, int i, int j);
void writeInFile(vector<double> y, string filename);
int DynamicDensity(const string& location, int nth_frame, int num_threads);
int count (Mat frame,  int start,int rows, int cols, int threshold);
int writeOut();
void *ProcessFrame(void *thrd);

// Opens the empty frame
Mat src_colour = imread("../COP290/assets/empty_final.jpg");
//Mat src_crop_1, src_crop_2, src_crop_3;
Mat src_crop[100];
// Finds the cropped image of the input image
//Mat src_crop = CropImage(src_colour, 1000, 286, 1552, 1264, 216, 1060, 1070, 200, 52, 830);

int A[100];
Mat sources[100];
int B[100];
struct thread_data{
    Mat frame;
    Mat source;
    //Mat dest_frame;
    int threads;
    int cur_thread;
};

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
    int nth_frame, parallel, num_threads;
    desc.add_options()
            ("help,h", "Print help message")
            ("file,f", po::value(&vid_file)->required(),"Path to Video file")
            ("nframes,n", po::value<int>(&nth_frame)->default_value(3), "Decides FPS by processing "
                                                                        "every Nth frame of the video. Default Value: 3")
            ("threads,t", po::value<int>(&parallel)->default_value(0), "Set 1 for Parallel execution "
                                   "using OpenMP and 0 for Serial Execution. Default Value: 0. Type export OMP_NUM_THREADS=2 in terminal before compiling.")
            ("numThreads,x", po::value<int>(&num_threads)->default_value(1), "Decides no of threads "
                                   "using pthreads for parallel execution. Default Value: 1.")
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
        QueueDensity(vid_file, nth_frame, num_threads);

        auto t2 = chrono::high_resolution_clock::now();
        cout << "Time taken by Static queue density: " <<
             chrono::duration_cast<chrono::seconds>(t2 - t1).count() << " s\n";

        //DynamicDensity(vid_file, nth_frame, num_threads);
        //auto t3 = chrono::high_resolution_clock::now();
        //cout << "Time taken by Dynamic queue density: " <<
            // chrono::duration_cast<chrono::seconds>(t3 - t2).count() << " s\n";
    }
    else { // OpenMP to Parallise function execution. Input export OMP_NUM_THREADS=2 to use
        #pragma omp parallel default(none) shared(vid_file, nth_frame, num_threads)
        #pragma omp single
        {
            #pragma omp task
            QueueDensity(vid_file, nth_frame, num_threads);

           // #pragma omp task
            //DynamicDensity(vid_file, nth_frame, num_threads);
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
/*int DynamicDensity(const string& location, int nth_frame, int num_threads){

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
    //frame = CropImage(frame);

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
            //frame2 = CropImage(frame2);

            // Finds the difference between the current and previous frames
            absdiff(frame2, frame, out);

            // rows denotes no of rows in output frame, col denotes no of columns in output frame
            int rows = out.rows;
            int cols = out.cols;

            // Counts no of pixels whose pixel value is greater than 20
            double count_pix = count(out, 0, rows, cols, 20);

            // Add the output to the output vector
            y.push_back(count_pix*1.5);

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
}*/

 /**
  *
  * @param location : location of the video file
  * @param nth_frame
  * @param num_threads
  * @return
  */
int QueueDensity(const string& location, int nth_frame, int num_threads){

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
    int yes = 0;
    int rows = 0;
    Mat frame, fgMask;
    int is_complete[num_threads];

    pthread_t threads[num_threads];
    int rc;
    int j;
    void *x;

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

            // Create threads
            struct thread_data thrd[num_threads];
            vector<long> thrd_output;
            
            // Give each thread, the current frame, destination frame and thrd id as arguments
            for(int j = 0; j < num_threads; j++){
                Mat fgMask, src_temp;
                thrd[j].frame = frame;
                thrd[j].source = src_colour;
                thrd[j].threads = num_threads;
                thrd[j].cur_thread = j;
                int rc = pthread_create(&threads[j], NULL, ProcessFrame, (void *)&thrd[j]);
                if (rc) {
                    cout << "Error:unable to create thread," << rc << endl;
                    exit(-1);
                }
            }
            
            
            // Join all the threads and place the output values in a vector
            for(int j = 0; j < num_threads; j++){

                int rc = pthread_join( threads[j], &x);
                if (rc) {
                    cout << "Error:unable to join," << rc << endl;
                    exit(-1);
                }
                thrd_output.push_back((long)x);
            }

            // Find the total number of rows in the output image
            if(yes == 0){
                for(int k = 0; k < num_threads; k++){
                    if(!src_crop[k].empty()){
                        rows = rows + src_crop[k].rows;
                    }
                    else{
                        continue;
                    }
                }
                yes = 1;
            }

            // Sum all the outputs of different threads
            double x = (double)accumulate(thrd_output.begin(), thrd_output.end(), 0)/(double)(rows*328);

            // Store the final output in a vector
            y.push_back(x);

        }

        else{
            capture >> frame;
        }

        i++;
    }

    writeInFile(y, OUT_PATH + pathSeparator + "QueueDensity.txt");

    return 0;
}
double line1(int y){
    return 1000 - (((double)1/(double)1.2)*(double)(y - 215));
}
double line2(int y){
    return 1552 + (((double)1/(double)3)*(double)(y - 1070));
}
/*
 * Process the selected rows in the given frame
 * @param thrd: contains the data of cur frame, dest frame and thread id
*/
void *ProcessFrame(void *thrd){

    struct thread_data *thrd_data = (struct thread_data *) thrd;
    
    // Current frame, Source Frame
    Mat frame_2;
    Mat frame = thrd_data->frame;
    Mat source = thrd_data ->source;
    
    // Find the rows that current thread has to process
    int cur_thread = thrd_data->cur_thread;
    int temp = (frame.rows)/(thrd_data->threads);
    int start_row = cur_thread*temp;
    int end_row = (cur_thread + 1)*temp;
    int col = 328;

    if(cur_thread == (thrd_data->threads) - 1){
        end_row = frame.rows;
    }

    // Crop the original frame as required
    frame = frame(Rect(0, start_row, frame.cols, end_row - start_row));
    source = source(Rect(0, start_row, src_colour.cols, end_row - start_row));
    
    int count = 0;

    // If the cropped part lies out of the area of interest, return 0
    if(end_row <= 215){
        pthread_exit((void *)(unsigned long long)count);
    }
    else if(start_row >= 1070){
        pthread_exit((void *)(unsigned long long)count);
    }

    // Else find the angle correction of the required part
    else if(start_row > 215 && end_row > 1070){
        
        int contrib = ((1070 - start_row)*778/855) + 1;
        frame_2 = CropImage(frame, (int)line1(start_row), (int)line1(1070), (int)line2(1070), (int)line2(start_row), 0, 1069 - start_row, 1069 - start_row, 0, 0, contrib);
        if(A[cur_thread] == 0){
            src_crop[cur_thread] = CropImage(source, (int)line1(start_row), (int)line1(1070), (int)line2(1070), (int)line2(start_row), 0, 1069 - start_row, 1069 - start_row, 0, 0, contrib);
            A[cur_thread] = 1;
        }
    }
    else if(start_row < 215 && end_row < 1070){
        
        int contrib = ((end_row - 215)*778/855) + 1;
        int x = ((end_row - 215)*52/855) + 1;
        frame_2 = CropImage(frame, (int)line1(215), (int)line1(end_row - 1), (int)line2(end_row - 1), (int)line2(215), 214 - start_row, end_row - start_row - 1, end_row - start_row - 1, 214 - start_row, x, x + contrib);
        if(A[cur_thread] == 0){ 
            src_crop[cur_thread] = CropImage(source, (int)line1(215), (int)line1(end_row - 1), (int)line2(end_row - 1), (int)line2(215), 214 - start_row, end_row - start_row - 1, end_row - start_row - 1, 214 - start_row, x, x + contrib);
            A[cur_thread] == 1;
        }
    }
    else if(start_row < 215 && end_row > 1070){
        int contrib = ((end_row - start_row)*778/855) + 1;
        frame_2 = CropImage(frame, (int)line1(215), (int)line1(1070), (int)line2(1070), (int)line2(215), 214 - start_row, 1069 - start_row, 1069 - start_row, 214 - start_row, 52, 830);
        if(A[cur_thread] == 0){
            src_crop[cur_thread] = CropImage(source, (int)line1(215), (int)line1(1070), (int)line2(1070), (int)line2(215), 214 - start_row, 1069 - start_row, 1069 - start_row, 214 - start_row, 52, 830);
            A[cur_thread] = 1;
        }
    }
    else{
        
        int contrib = ((end_row - start_row)*778/855) + 1;
        frame_2 = CropImage(frame, (int)line1(start_row), (int)line1(end_row - 1), (int)line2(end_row - 1), (int)line2(start_row), 0, end_row - start_row - 1, end_row - start_row - 1, 0, 0, contrib);
        if(A[cur_thread] == 0){
            src_crop[cur_thread] = CropImage(source, (int)line1(start_row), (int)line1(end_row - 1), (int)line2(end_row - 1), (int)line2(start_row), 0, end_row - start_row - 1, end_row - start_row - 1, 0, 0, contrib);
            A[cur_thread] = 1;
        }
    }

    // Find number of pixels whose value is greater than the threshold 15
    for(int p = 0; p < frame.rows; p++){
        for(int j = 0; j < col; j++){
            if (abs(frame_2.at<uchar>(p, j) - src_crop[cur_thread].at<uchar>(p, j)) > 15){
               count ++;
            }   
        }
    }

    // Exit the thread
    pthread_exit((void *)(unsigned long long)count);
    
}

/**
 * Given a frame, this frame finds the no of pixels whose pixel value is greater than a threshold
 * @param frame
 * @param row: no of rows in the frame
 * @param cols: no of columns in the frame
 */
int count (Mat frame, int start, int rows, int cols, int threshold){

    int count = 0;
    for (int i = start; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if(frame.at<uchar>(i, j) > threshold){
                count++;
            }
        }
    }
    return count;
}

/**
 * Given a frame, this function crops and outputs the cropped frame
 * @param frame
 * @param c1, c2, c3, c4: Column values for area of interest
 * @param r1, r2, r3, r4: Row values for area of interst
 * @param i, j: Row values used in homography
 * @return : frame to be cropped
 */
Mat CropImage(Mat frame, int c1, int c2, int c3, int c4, int r1, int r2, int r3, int r4, int i, int j){

    // Stores the wrapped image
    Mat img_warp, img_crop;

    // These points are taken from Riju Ma'am's website
    vector<Point2f> dest_pts;
    dest_pts.emplace_back(Point2f(472, i));
    dest_pts.emplace_back(Point2f(472, j));
    dest_pts.emplace_back(Point2f(800, j));
    dest_pts.emplace_back(Point2f(800, i));

    // This part of the image is our area of interest. I approximated these points
    vector<Point2f> src_pts;
    src_pts.emplace_back(c1, r1);
    src_pts.emplace_back(c2, r2);
    src_pts.emplace_back(c3, r3);
    src_pts.emplace_back(c4, r4);

    // Finding Homography between the points
    Mat homography = findHomography(src_pts, dest_pts);
    
    // Convert frame to Grayscale
    cvtColor(frame, frame, COLOR_BGR2GRAY);

    warpPerspective(frame, img_warp, homography, frame.size());

    img_crop = img_warp(Rect(472, i, 328, j - i));

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
    ifstream baseline("../COP290/assets/baseline.txt");
    //ifstream dynamic(OUT_PATH + pathSeparator + "DynamicDensity.txt");
    out_file << "Time(in sec), queue density\n";
    //cout << "framenum, queue density\n";
    int i=0;
    double error = 0;
    double base, out;
    if (!queue)
    {
        std::cout << "Error opening file Queue" << ": " << strerror(errno) << '\n';
        return 1;
    }

    do
    {
        string res = to_string((double)i/(double)15) + ", " + to_string(out) + "\n";
        error = error + pow(base - out, 2.0);
        out_file << res;
        i++;
    } while(queue >> out && baseline >> base);

    error = error / (double)5737;
    error = pow(error, 0.5);
    cout << "Error :" << error << endl;
    out_file.close();
    queue.close();
    baseline.close();
    //dynamic.close();
    return 0;
}