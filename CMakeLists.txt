cmake_minimum_required(VERSION 3.14)

project(qpk11setpassword LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt5Keychain REQUIRED)

add_library(qpk11setpassword SHARED
  qpk11setpassword_global.h
  qpk11setpassword.cpp
  qpk11setpassword.h
)

target_link_libraries(qpk11setpassword PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt5Keychain::Qt5Keychain)

target_compile_definitions(qpk11setpassword PRIVATE QPK11SETPASSWORD_LIBRARY)
