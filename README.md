# Computer Simulation Homework 1

Using `simlib` to simulate the system model described in problem 1.


## Getting Started

Type `make` to compile the source code. The input file hw1.in is placed in `input/`, and the output file will be put in `output/`.
The binary file `hw1` will be compiled to `bin`.
To run the code, simply run `./bin/hw1`. (NOTE: you have to run the code under the path `./`, instead of `./bin/`!!)
To test the customered input file, please follow the input format, and rename it as "hw1.in" and put it into `input/`.

### Prerequisites

This project is tested in 

1. Ubuntu 16.04 with gcc 5.4.0
2. MacOS with gcc 4.8.5

### Input Format

Input files follow the format:

Number of station (2 in this problem), Mean arrival time (exponential), Length of simulation (hours) 
Inspection time param a, Inspection time param b (uniform dist.)
Repair time param a, Repair time param b (uniform dist.)
Probability of repair 
Probability of re-inspection

## Author

Wei-Ning Chen, r05942078@ntu.edu.tw
