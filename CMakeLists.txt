# create a plugin that adds a reader to the ParaView GUI
# it is added in the file dialog when doing opens/saves.

cmake_minimum_required(VERSION 2.8)
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")
 
if(ParaView_SOURCE_DIR)
  include_directories(
    ${PARAVIEW_INCLUDE_DIRS}
    ${PARAVIEW_GUI_INCLUDE_DIRS}
    ${PARAVIEW_KWSYS_INCLUDE_DIRS}
    ${VTK_INCLUDE_DIR}
    ${MPI_INCLUDE_PATH}
  )
else(ParaView_SOURCE_DIR)
  find_package(ParaView REQUIRED)
  include(${PARAVIEW_USE_FILE})
endif(ParaView_SOURCE_DIR)
  
if(PARAVIEW_BUILD_QT_GUI)

  find_package(Qt4)
  if(QT4_FOUND)
    include(${QT_USE_FILE})

    include_directories(${PARAVIEW_INCLUDE_DIRS}
                        ${VTK_INCLUDE_DIR}
                        ${pqCore_BINARY_DIR}
                        ${pqCore_SOURCE_DIR}
                        ${pqComponents_BINARY_DIR}
                        ${pqComponents_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}
    )
      
    if(${CMAKE_VERSION} VERSION_LESS 2.8.7 AND NOT WIN32)
      include(Qt4Macros-CMake2.8.7)
    endif()
  ENDIF(QT4_FOUND)
ENDIF(PARAVIEW_BUILD_QT_GUI)

###    LIBMESH Library   ###
FIND_PATH(LIBMESH_INCLUDE_DIR libmeshb7.h HINTS "${HINT_PATH}/include/")
if(NOT LIBMESH_INCLUDE_DIR)
    MESSAGE(FATAL_ERROR "Could not find libmeshb7.h . Please set LIBMESH_INCLUDE_DIR")
endif()
INCLUDE_DIRECTORIES( ${LIBMESH_INCLUDE_DIR})
FIND_LIBRARY(LIBMESH_LIBRARY Meshb.7 HINT "${HINT_PATH}/lib")
if(NOT LIBMESH_LIBRARY)
    MESSAGE(FATAL_ERROR "Could not find libMeshb.7  Please set LIBMESH_LIBRARY")
endif()

include_directories(${LIBMESH_INCLUDE_DIR})

set(MEDIT_READER_LIB_NAME vtkMEDITReader)
ADD_PARAVIEW_PLUGIN(${MEDIT_READER_LIB_NAME} "1.0"
  SERVER_MANAGER_XML vtkMEDITReader.xml
  SERVER_MANAGER_SOURCES vtkMEDITReader.cxx
  GUI_RESOURCE_FILES vtkMEDITReaderGUI.xml
  )
  SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC" )
target_link_libraries(${MEDIT_READER_LIB_NAME} ${LIBMESH_LIBRARY} ${VTK_LIBRARIES})
