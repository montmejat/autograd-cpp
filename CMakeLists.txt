cmake_minimum_required(VERSION 3.10)
project(AutoGrad)

add_library(autograd STATIC src/scalar.cpp)

target_include_directories(autograd PUBLIC include)

enable_testing()
add_subdirectory(tests) 
