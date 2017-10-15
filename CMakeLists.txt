cmake_minimum_required(VERSION 3.8)
project(Event)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Event.h Delegate.h)
add_executable(Event ${SOURCE_FILES})