# MafIXMAS2018
## Christmas task for the course "Mathematik für Informatiker 1" at TU Dortmund

With default settings, the project requires at least the Intel® Advanced Vector Extensions 2. If your processor doesn't support AVX2, set 
`#define SIMD_SIZE` in `SIMD-Bitset/bitset.h` in line 8 to 128 instead of 256. If your processor doesn't support this, either, you cannot compile the project.

To compile the project to ./build, run ./rel.sh on a unix system with cmake, ninja and g++ installed. 

After that you can run `./build/MafIXMAS2018 12` to start the search with 12 threads. If you drop the command line parameter, the default value is 8 threads.
