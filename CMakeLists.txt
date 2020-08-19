cmake_minimum_required(VERSION 3.14)

#CASROOT = /opt/opencascade-7.4.0
#OCCLIB  = $$CASROOT/build/lin64/gcc/lib

project (Converter)

message("Starting...")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 98)

#QT5
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
#add_executable(ProgramQt5 testmain.cpp)
#target_link_libraries(ProgramQt5 Qt5::Core Qt5::Widgets)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/opt/opencascade-7.4.0/build")

find_package(OpenGL REQUIRED)
find_package(OpenCASCADE COMPONENTS FoundationClasses REQUIRED)
find_package(OpenCASCADE COMPONENTS ModelingData REQUIRED)
find_package(OpenCASCADE COMPONENTS DataExchange REQUIRED)
include_directories(${OpenCASCADE_INCLUDE_DIR})
set (LIBS ${LIBS} ${OpenCASCADE_LIBRARY_DIR})
# OCCT libraries for using


# Add source files
file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add header files
file(GLOB_RECURSE HEADER_FILES
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.hpp)

set (LIBS
        -lTKernel -lTKMath -lTKService -lTKV3d -lTKOpenGl
        -lTKBRep -lTKIGES -lTKSTL -lTKVRML -lTKSTEP -lTKSTEPAttr -lTKSTEP209
        -lTKSTEPBase -lTKGeomBase -lTKGeomAlgo -lTKG3d -lTKG2d
        -lTKXSBase -lTKShHealing -lTKHLR -lTKTopAlgo -lTKMesh -lTKPrim
        -lTKCDF -lTKBool -lTKBO -lTKFillet -lTKOffset
        -lTKCAF -lTKLCAF -lTKXCAF -lTKXDESTEP -lTKVCAF
        -lX11
        )

add_executable(Converter ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(Converter Qt5::Core Qt5::Widgets ${LIBS} ${uwp_USED_LIBS})

