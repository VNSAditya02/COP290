# COP290
Assignment 1 for the course COP290. Details can be found [here](https://www.cse.iitd.ac.in/~rijurekha/cop290_2021.html).

## Additional Packages

1. OpenCV
2. Boost: `sudo apt install libboost-all-dev`
3. OpenMP: `sudo apt-get install libomp-dev`

## Run Instructions:

### Using MakeFile
1. From the project root, run `make all` to build the entire project
2. Type `make task1` to build only the first subtask. Similarly, you can use `make task2` and `make task3`
3. For `task3`, you can make each of the four parts as `make part{i}` where `{i} in {1,2,3,4}` 
4. Type `./build/angle_correction -f <path_to_image_input>` in terminal to run `angle_correction.cpp`. The command line parameters have already been described in the previous section.
5. Similarly, the other parts can be run using their respective command line arguments
6. Type `make clean` to remove the generated build object files and executables

### Part 1: angle_correction.cpp
1. Go to `src` Directory (sources root)
2. execute the following command:  
    ```
    g++ angle_correction.cpp -o angle_correction -pthread -std=c++11 `pkg-config --cflags --libs opencv` -lboost_program_options
    ```
3. Finally, execute the program via `./angle_correction -f <path_to_image_input>`
4. Execution instructions can be viewed using `./angle_correction --help` (or making a mistake in the command line argument/supplying an invalid image path).
5. Here are all the valid command line input for this program -
    ```
    Allowed options:
      -h [ --help ]         print usage message
      -f [ --file ] arg     Path to Image file
    ``` 

### Part 2: traffic_density.cpp
1. Type `export OMP_NUM_THREADS=2` in terminal, then execute the following command from the project root:
    ```
    g++ src/traffic_density.cpp -o build/traffic_density -pthread -std=c++11 `pkg-config --cflags --libs opencv` -O3 -flto -march=native -mtune=knl -lboost_program_options -lstdc++fs -fopenmp
    ```
2. Execute the program via `build/traffic_density -f <path to video> -n <number of frames to skip> -t [0|1]`.
3. Usage options are given below:
    ```
    Allowed options:
      -h [ --help ]             Print help message
      -f [ --file ] arg         Path to Video file
      -n [ --nframes ] arg (=3) Decides FPS by processing every Nth frame of the 
                                video. Default Value: 3
      -t [ --threads ] arg (=0) Set 1 for Parallel execution using OpenMP and 0 for
                                Serial Execution. Default Value: 0. Type export 
                                OMP_NUM_THREADS=2 in terminal before compiling.

    ```
4. Output files are stored in out folder in the project root 

### Part 3: Understanding and analyzing trade-offs in software design
### Method - 1: sub-sampling frames 
1.  ```
    g++ src/Partc_Method_1.cpp -o build/Partc_Method_1 -lpthread -std=c++11 `pkg-config --cflags --libs opencv` -lboost_program_options -lstdc++fs
    ```
2. Execute the program via `build/Partc_Method_1 -f <path to video> -n <number of frames to skip> -t [0|1]`.

```
Allowed options:
  -h [ --help ]             Print help message
  -f [ --file ] arg         Path to Video file
  -n [ --nframes ] arg (=3) Decides FPS by processing every Nth frame of the 
                            video. Default Value: 3
  -t [ --threads ] arg (=0) Set 1 for Parallel execution using OpenMP and 0 for
                            Serial Execution. Default Value: 0. Type export 
                            OMP_NUM_THREADS=2 in terminal before compiling.
```

### Method - 2: reduce resolution

1.  ```
    g++ src/Partc_Method_2.cpp -o build/Partc_Method_2 -lpthread -std=c++11 `pkg-config --cflags --libs opencv` -lboost_program_options -lstdc++fs
    ```
2. Execute the program via `build/Partc_Method_2 -f <path to video> -n <number of frames to skip> -t [0|1] -X <width of resoluted image> -Y <height of resoluted image>`.
```
Allowed options:
  -h [ --help ]             Print help message
  -f [ --file ] arg         Path to Video file
  -n [ --nframes ] arg (=3) Decides FPS by processing every Nth frame of the 
                            video. Default Value: 3
  -t [ --threads ] arg (=0) Set 1 for Parallel execution using OpenMP and 0 for
                            Serial Execution. Default Value: 0. Type export 
                            OMP_NUM_THREADS=2 in terminal before compiling.
  -X [ --width ] arg (=1)   Decides width for changing resolution
  -Y [ --height ] arg (=1)  Decides height for changing resolution
```

### Method - 3: split work spatially 

1.  ```
    g++ src/Partc_Method_3.cpp -o build/Partc_Method_3 -lpthread -std=c++11 `pkg-config --cflags --libs opencv` -lboost_program_options -lstdc++fs
    ```
2. Execute the program via `build/Partc_Method_3 -f <path to video> -n <number of frames to skip> -t [0|1] -x <no of threads>`.

```
Allowed options:
  -h [ --help ]                Print help message
  -f [ --file ] arg            Path to Video file
  -n [ --nframes ] arg (=3)    Decides FPS by processing every Nth frame of the
                               video. Default Value: 3
  -t [ --threads ] arg (=0)    Set 1 for Parallel execution using OpenMP and 0 
                               for Serial Execution. Default Value: 0. Type 
                               export OMP_NUM_THREADS=2 in terminal before 
                               compiling.
  -x [ --numThreads ] arg (=1) Decides no of threads using pthreads for 
                               parallel execution. Default Value: 1.
```

### Method - 4: split work temporally

1.  ```
    g++ src/Partc_Method_4.cpp -o build/Partc_Method_4 -lpthread -std=c++11 `pkg-config --cflags --libs opencv` -lboost_program_options -lstdc++fs
    ```
2. Execute the program via `build/Partc_Method_4 -f <path to video> -n <number of frames to skip> -t [0|1] -x <no of threads>`.

```
Allowed options:
  -h [ --help ]                Print help message
  -f [ --file ] arg            Path to Video file
  -n [ --nframes ] arg (=3)    Decides FPS by processing every Nth frame of the
                               video. Default Value: 3
  -t [ --threads ] arg (=0)    Set 1 for Parallel execution using OpenMP and 0 
                               for Serial Execution. Default Value: 0. Type 
                               export OMP_NUM_THREADS=2 in terminal before 
                               compiling.
  -x [ --numThreads ] arg (=1) Decides no of threads using pthreads for 
                               parallel execution. Default Value: 1.
```

## Contributors
1. Pratyush Pandey (2017EE10938)
2. VNS Aditya (2019CS50471)
