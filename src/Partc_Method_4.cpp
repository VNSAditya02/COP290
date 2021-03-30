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
Mat CropImage(Mat frame);
void writeInFile(vector<double> y, string filename);
int DynamicDensity(const string& location, int nth_frame, int num_threads);
int count (Mat frame,  int start,int rows, int cols, int threshold);
int writeOut();
void *ProcessFrame(void *thrd);

// Opens the empty frame
Mat src_colour = imread("../COP290/assets/empty_final.jpg");
// Finds the cropped image of the input image
Mat src_crop = CropImage(src_colour);

// Used to store the data that should be passed to threads
struct thread_data{
    Mat frame;
    /*int start_row;
    int end_row;
    int cols;*/
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
int DynamicDensity(const string& location, int nth_frame, int num_threads){

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
}

 /**
  *
  * @param location : location of the video file
  * @param nth_frame
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
    Mat frame, fgMask;;

    pthread_t threads[num_threads];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    int rc;
    int j;
    void *x;


    // Stores the value of no of contours in a frame
    std::vector<double> y;

    while (true)
    {
        // threads_created variable stores the count of no of threads created because at the end of video
        // NUM_THREADS may not be equal to threads_created
        struct thread_data thrd[num_threads];
        int flag = 0;
        int threads_created = 0;
        int p = 0;
        while(p < num_threads){
            // Change the condition accordingly, i % 1 process every frame, i % 3 process every third frame
            if (i % nth_frame == 0)
            {
                // If true, read the frame
                capture >> frame;

               // If the frame is empty, exit the loop
                if (frame.empty()){
                    flag = 1;
                    break;
                }

                thrd[p].frame = frame;

                // Create threads
                int rc = pthread_create(&threads[p], &attr, ProcessFrame, (void *)&thrd[p]);
                
                if (rc) {
                    cout << "Error:unable to create thread," << rc << endl;
                    exit(-1);
                }
                p++;
                threads_created++;
            }
            
            else{
                capture >> frame;
            }

            i++;
        }
        
        // Join the created threads
        pthread_attr_destroy(&attr);
        for(int p = 0; p < threads_created; p++){
            int rc = pthread_join( threads[p], &x);
            if (rc) {
                cout << "Error:unable to join," << rc << endl;
                exit(-1);
            }

            double output = (long) x;
            y.push_back(output/double(778*328));
        }

        if(flag == 1){
            break;
        }

        else{
            continue;
        }

        
    }

    writeInFile(y, OUT_PATH + pathSeparator + "QueueDensity.txt");

    return 0;
}

/*
 * Process the frame
 * @param thrd: contains the data of cur frame
*/
void *ProcessFrame(void *thrd){

    struct thread_data *thrd_data = (struct thread_data *) thrd;
    
    Mat fgMask;
    // Finds the cropped image of the given frame
    Mat frame = thrd_data->frame;

    frame = CropImage(frame);
    absdiff(frame, src_crop, fgMask);
    int row = fgMask.rows;
    int col = fgMask.cols;
    int x =count(fgMask, 0, row, col, 15);

    // Exit from the thread
    pthread_exit((void *)x);
    
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