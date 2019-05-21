cmake_minimum_required(VERSION 3.10)
project(TIN_Server)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)

add_executable(TIN_Server mainServer.cpp)

find_package(Boost 1.65.1 REQUIRED COMPONENTS system filesystem thread)
target_link_libraries(TIN_Server ${Boost_LIBRARIES})