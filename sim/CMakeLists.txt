# Setup the project
#
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(G4examples)

#----------------------------------------------------------------------------
# Add coverage to debug flags for gcc compilation
if (CMAKE_BUILD_TYPE STREQUAL Debug)
  MESSAGE( "Build mode: Debug" )
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -O0 --coverage")
  add_link_options("--coverage")
endif(CMAKE_BUILD_TYPE STREQUAL Debug)

#----------------------------------------------------------------------------
# Find ROOT
#
if(ROOTSYS)
  message(STATUS "* G4 example * INFO: ROOT will be used")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  SET(CMAKE_CXX_STANDARD 14)
  find_package(ROOT REQUIRED COMPONENTS Core RIO Hist Graf Gpad Geom GenVector Physics MathCore)
endif(ROOTSYS)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()


#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

#----------------------------------------------------------------------------
# Locate sources and libraries for this project
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/geometry/*.cc ${PROJECT_SOURCE_DIR}/src/manager/*.cc ${PROJECT_SOURCE_DIR}/src/phys/*.cc)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 and other libraries
#
add_executable(mkgeant4 mkgeant4.cc ${sources})

if(ROOTSYS)
  target_link_libraries(mkgeant4 ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
else()
  target_link_libraries(mkgeant4 ${Geant4_LIBRARIES})
endif(ROOTSYS)
set_property(TARGET mkgeant4 PROPERTY CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR}/include/geometry ${PROJECT_SOURCE_DIR}/include/manager ${PROJECT_SOURCE_DIR}/include/phys)

# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS mkgeant4 DESTINATION bin)
