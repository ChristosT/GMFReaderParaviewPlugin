# create a plugin that adds a reader to the ParaView GUI
# it is added in the file dialog when doing open

cmake_minimum_required(VERSION 2.8)
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")

find_package(ParaView REQUIRED)
include(${PARAVIEW_USE_FILE})
set(GMF_READER_LIB_NAME vtkGMFReader)
ADD_PARAVIEW_PLUGIN(${GMF_READER_LIB_NAME} "1.0"
  SERVER_MANAGER_XML vtkGMFReader.xml
  SERVER_MANAGER_SOURCES vtkGMFReader.cxx
  REQUIRED_ON_SERVER)

###    LIBMESH Library   ###
find_path(LIBMESH_INCLUDE_DIR libmeshb7.h HINTS "${HINT_PATH}/include/")
if(NOT LIBMESH_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find libmeshb7.h . Please set LIBMESH_INCLUDE_DIR")
endif()

include_directories( ${LIBMESH_INCLUDE_DIR})
find_library(LIBMESH_LIBRARY Meshb.7 HINT "${HINT_PATH}/lib")
if(NOT LIBMESH_LIBRARY)
    message(FATAL_ERROR "Could not find libMeshb.7  Please set LIBMESH_LIBRARY")
endif()


target_link_libraries(${GMF_READER_LIB_NAME} PUBLIC ${LIBMESH_LIBRARY} ${VTK_LIBRARIES})
