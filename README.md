# COP290
Assignment 1 for the course COP290. Details can be found [here](https://www.cse.iitd.ac.in/~rijurekha/cop290_2021.html).

## Additional Packages

1. OpenCV
2. Boost: `sudo apt install libboost-all-dev`

## Run Instructions:
### angle_correction.cpp
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

### traffic_density.cpp
1. execute the following command from the project root:
    ```
    g++ src/traffic_density.cpp -o build/traffic_density -pthread -std=c++11 `pkg-config --cflags --libs opencv` -O3 -flto -march=native -mtune=knl -lboost_program_options -lstdc++fs
    ```
2. Execute the program via `build/traffic_density -f <path to video> -n <number of frames to skip>`
3. Usage options are given below:
    ```
    Allowed options:
      -h [ --help ]             Print help message
      -f [ --file ] arg         Path to Video file
      -n [ --nframes ] arg (=3) Decides FPS by processing every Nth frame of the 
                                video. Default Value: 3
    ```

### Using MakeFile
1. From the project root, run `make`
2. Type `bin/runner -f <path_to_image_input>` in terminal to run `angle_correction.cpp`. The command line parameters have already been described in the previous section.
3. Type `make clean` to remove the generated build object files and executables

## Contributors
1. Pratyush Pandey (2017EE10938)
2. VNS Aditya (2019CS50471)
