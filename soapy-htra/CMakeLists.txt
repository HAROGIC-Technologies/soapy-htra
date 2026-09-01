cmake_minimum_required(VERSION 3.8)
project(SoapyHarogic CXX)

# SoapyHarogic is implemented as a standard SoapySDR device module.
# GNU Radio support is provided by GNU Radio's built-in Soapy Custom Source;
# no HTRA-specific GRC block is installed by this project.

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(SoapySDR "0.8" REQUIRED)
message(STATUS "Found SoapySDR: ${SoapySDR_VERSION} - Includes: ${SoapySDR_INCLUDE_DIRS}, Libs: ${SoapySDR_LIBRARIES}")

find_package(LibHTRA REQUIRED)
message(STATUS "Found LibHTRA: ${LibHTRA_VERSION} - Includes: ${LibHTRA_INCLUDE_DIRS}, Libs: ${LibHTRA_LIBRARIES}")

set(SOURCES
    HarogicDevice.cpp
)

SOAPY_SDR_MODULE_UTIL(
    TARGET HarogicSupport
    SOURCES ${SOURCES}
    LIBRARIES ${LibHTRA_LIBRARIES}
)

target_include_directories(HarogicSupport PRIVATE
    ${SoapySDR_INCLUDE_DIRS}
    ${LibHTRA_INCLUDE_DIRS}
)

target_link_libraries(HarogicSupport PRIVATE
    ${SoapySDR_LIBRARIES}
    ${LibHTRA_LIBRARIES}
)

message(STATUS "Configuration successful! Target 'HarogicSupport' will be built.")
message(STATUS "Final install path will be: ${SoapySDR_MODULE_INSTALL_DIR}")
message(STATUS "GNU Radio usage: Core -> Soapy -> Source -> Soapy Custom Source")
