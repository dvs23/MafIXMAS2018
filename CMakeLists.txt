cmake_minimum_required(VERSION 2.8.9)
project(MafIXMAS2018)

link_directories(/usr/lib)

include_directories(SIMD-Bitset)
include_directories(/usr/include)

#-fexceptions -Wnon-virtual-dtor -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wfloat-equal -Wunreachable-code -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Wzero-as-null-pointer-constant -Wmain -Wextra -Wall

set(CMAKE_CXX_FLAGS_RELEASE "-Wall -g -O3 -std=c++17 -march=native -ftree-vectorize -ftree-vectorizer-verbose=2")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall  -std=c++17 -march=native -g -fno-strict-aliasing")
message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
 
add_executable(MafIXMAS2018 main.cpp ${SOURCES})
target_link_libraries(
MafIXMAS2018
${PROJECT_LINK_LIBS}
pthread
)
