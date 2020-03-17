cmake_minimum_required(VERSION 3.0.0)
project(producer VERSION 0.1.0)

include(CTest)
enable_testing()
include(FindProtobuf)
find_package(Protobuf REQUIRED)

set (CMAKE_CXX_STANDARD 11)

include_directories(/usr/local/include)
target_link_libraries(${executable_name} protobuf::libprotobuf)

add_executable(producer main.cpp pnr.pb.cc)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
