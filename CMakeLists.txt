
cmake_minimum_required(VERSION 2.6)
project(VdbViewer)


include_directories( /local/xu29mapu/projects/libQGLViewer)
link_directories( /local/xu29mapu/projects/libQGLViewer/QGLViewer)
# FIND_PACKAGE(QGLViewer REQUIRED)
# INCLUDE_DIRECTORIES(${QGLVIEWER_INCLUDE_DIR})

set(QT_MIN_VERSION "5.3.0")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package( Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Widgets Xml OpenGL DBus)
find_package( OpenGL 3.0 REQUIRED )
find_package( CUDA REQUIRED )
find_package(Boost REQUIRED)

add_definitions(-DBUILD_OPENVDB)
set( GVDB_INCLUDE_DIR ~/.local/include )

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Xml_INCLUDE_DIRS}
    ${Qt5OPENGL_INCLUDE_DIRS}
    ${GVDB_INCLUDE_DIR}
    /usr/include/x86_64-linux-gnu/qt5/QtOpenGL/
    )
    

aux_source_directory( source SOURCES )

file(GLOB UI RELATIVE  "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui )
file(GLOB UI_INCLUDES RELATIVE  "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp )
qt5_wrap_cpp(MOC_SRC ${UI_INCLUDES})
qt5_wrap_ui(MOC_UI ${UI})

set( OPENVDB_LIBRARIES
                            tbb
                            openvdb
                            Half                                                                      
                            IexMath                                                                  
                            Iex                                                                             
                            IlmThread                                                               
                            Imath)

cuda_add_executable(VdbViewer
					main.cpp 
					OpenVdbViewer.cpp
					${MOC_UI}
					${MOC_SRC}
			    )


target_link_libraries(VdbViewer PUBLIC Qt5::Widgets Qt5::DBus Qt5::OpenGL QGLViewer-qt5 ${OPENGL_LIBRARIES} /local/xu29mapu/projects/gvdb-voxels/source/gvdb_library/build/libgvdb.so 
/local/xu29mapu/projects/cudpp/release/lib/libcudpp.a ${CUDA_LIBRARIES} ${Boost_LIBRARIES} ${OPENVDB_LIBRARIES} GLU)

