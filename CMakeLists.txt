cmake_minimum_required(VERSION 3.20)
project(CrashOutSolution CXX)

# Set C++ standard (modify to 20 if you are using C++20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add your subprojects
add_subdirectory(CrashOut)
add_subdirectory(Sandbox)