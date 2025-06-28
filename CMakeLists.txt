cmake_minimum_required(VERSION 3.31)
project(WaffleBot)

# include directory
include_directories(include)

set(CMAKE_CXX_STANDARD 23)

add_executable(WaffleBot main.cpp
        Action.cpp
        Action.h
        AudioHandling.cpp
        AudioHandling.h
        Input.h
        Macro.cpp
        Macro.h
        Timer.h
        include/simdjson.cpp
)

# Link to libsndfile
find_library(SNDFILE_LIB sndfile)

if(SNDFILE_LIB)
    target_link_libraries(WaffleBot ${SNDFILE_LIB})
else()
    message(FATAL_ERROR "libsndfile not found. Please install it.")
endif()
