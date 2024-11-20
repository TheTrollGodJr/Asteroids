cmake_minimum_required(VERSION 3.29)
project(asteroids)

set(CMAKE_CXX_STANDARD 20)

add_executable(asteroids main.cpp
        Asteroid.cpp
        Asteroid.h)
