# COP290
Assignment 1 for the course COP290. Details can be found [here](https://www.cse.iitd.ac.in/~rijurekha/cop290_2021.html).

## Additional Packages

1. OpenCV
2. Boost: `sudo apt install libboost-all-dev`

## Run Instructions
### Via Command Line
1. Go to `src` Directory (sources root)
2. execute the following command:  
```g++ angle_correction.cpp -o angle_correction -pthread -std=c++11 `pkg-config --cflags --libs opencv` -lboost_program_options```
3. Finally, execute the program via `./angle_correction -f <path_to_image_input>`
4. Execution instructions can be viewed using `./angle_correction --help` (or making a mistake in the command line argument/supplying an invalid image path).
5. Here are all the valid command line input for this program -
```
Allowed options:
  -h [ --help ]         print usage message
  -f [ --file ] arg     Path to Image file
``` 

### Using MakeFile
1. From the project root, run `make`
2. Type `bin\runner -f <path_to_image_input>` in terminal to run the application. The command line parameters have already been described in the previous section.
3. Type `make clean` to remove the generated build object files and executables

## Contributors
1. Pratyush Pandey (2017EE10938)
2. VNS Aditya (2019CS50471)