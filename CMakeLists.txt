cmake_minimum_required(VERSION 2.8)

project(lf-capture)
add_executable(${PROJECT_NAME} "main.cpp")

add_executable(lf-signal lf-signal.cpp)

find_package(Boost COMPONENTS system filesystem iostreams REQUIRED)

FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(/usr/local/include/aravis-0.8)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})

link_directories(${GLIB_LIBRARY_DIRS})

find_package(Qt5GStreamer REQUIRED)

add_library(lfserial serial.cpp)

target_link_libraries(lfserial
    serial
)

find_package(RunningStats REQUIRED)
include_directories(${RUNNINGSTATS_INCLUDE_DIRS})

add_library(capture capture.cpp)
target_link_libraries(capture
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_IOSTREAMS_LIBRARY}
    ${OpenCV_LIBS}
    araviscapture
)

add_executable(lf-webcam-scale-crop webcam-scale-crop.cpp)
target_link_libraries(lf-webcam-scale-crop
    ${OpenCV_LIBS}
)

add_executable(analyze-luminosity analyze-luminosity.cpp)
target_link_libraries(analyze-luminosity
    ${OpenCV_LIBS}
)

add_executable(analyze-blinkled analyze-blinkled.cpp)
target_link_libraries(analyze-blinkled
    ${OpenCV_LIBS}
)

add_executable(analyze-blink-log analyze-blink-log.cpp)
target_link_libraries(analyze-blink-log ${RUNNINGSTATS_LIBRARIES})

add_executable(analyze-qr analyze-qr.cpp)
target_link_libraries(analyze-qr
    ${OpenCV_LIBS}
)

add_executable(plot-numbers plot-numbers.cpp)
target_link_libraries(plot-numbers
    ${OpenCV_LIBS}
)

add_library(turntable turntable.cpp)

add_library(araviscapture araviscapture.cpp)

target_link_libraries(araviscapture
    aravis-0.8
    ${OpenCV_LIBS}
    gobject-2.0
)

add_executable(aravis-test aravis-test.cpp)

target_link_libraries(
    aravis-test
    araviscapture
    )

target_link_libraries(turntable
    lfserial
    pthread
)

target_link_libraries(${PROJECT_NAME}
    capture
    lfserial
    turntable
)

find_package(RunningStats REQUIRED)
include_directories(${RUNNINGSTATS_INCLUDE_DIRS})

add_executable(analyze-flicker analyze-flicker.cpp)

find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

target_link_libraries(analyze-flicker
      ${RUNNINGSTATS_LIBRARIES}
      ${CERES_LIBRARIES}

    )
