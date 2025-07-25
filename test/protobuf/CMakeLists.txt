cmake_minimum_required(VERSION 3.15)
project(mprpc)

set(CMAKE_CXX_STANDARD 20)

find_package(Protobuf REQUIRED)
add_executable(mprpc main.cpp test.pb.cc)
target_link_libraries(mprpc PRIVATE protobuf::libprotobuf pthread)
