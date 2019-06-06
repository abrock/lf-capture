cmake_minimum_required(VERSION 2.8)

project(lf-capture)
add_executable(${PROJECT_NAME} "main.cpp")

find_package(Boost COMPONENTS system filesystem iostreams REQUIRED)

FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_Include_Dirs})

add_library(lfserial serial.cpp)

target_link_libraries(lfserial
    serial
)

add_library(capture capture.cpp)
target_link_libraries(capture
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_IOSTREAMS_LIBRARY}
    ${OpenCV_LIBS}
)

add_library(turntable turntable.cpp)

target_link_libraries(turntable
    lfserial
    pthread
)

target_link_libraries(${PROJECT_NAME}
    lfserial
    capture
    turntable
)

