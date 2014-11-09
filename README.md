Panelz2
=======

Author: Adam O'Brien

E-mail: a.obrien@mail.utoronto.ca

Institution: University of Toronto

A two-dimensional unsteady source/vortex panel method solver for inviscid flows around airfoils. This code utilizes Petsc for parallel computations on dense matrices. The file format for the airfoils is very simple. The code is designed to read in airfoil data in the same format found at:

http://m-selig.ae.illinois.edu/ads/coord_database.html

The code should work with just about any airfoil data base, as it simply reads in the first line as the airfoil name, and all subsequent lines are assumed to be the x and y coordinates of the airfoil separated by whitespace.

Build

In order to build and run Panelz2, the following libraries should be available on the system:

1) Any MPI library

2) Petsc - http://www.mcs.anl.gov/petsc/

3) Boost - http://www.boost.org/

4) CMake - http://www.cmake.org/

In order to build the project, run

$ cmake CMakeLists.txt

in the "Panelz2" directory. This will produce the makefiles needed for the build. Then, simply run

$ make

Which will produce the binary "panelz2" in the "bin" directory. See below for information on how to run simulations.

Run:

In linux systems, usage is simlpy:

$ panelz2 --user-file [user input filename] --airfoil-file [airfoil data filename]

For an example of the user input file, see the "Panelz2/Files" directory. This project will have functionality continually added to it, but will remain relatively lightweight and simple, as it is mostly just a way for me to experiment with parallel matrix computations. Contributors are always welcome, however! See the included license file for information regarding distribution.

