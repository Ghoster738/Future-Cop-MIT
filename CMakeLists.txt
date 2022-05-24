cmake_minimum_required(VERSION 3.10)

project( FutureCopDecoder )

# Set the C++ standard to C++ 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(USE_INTERNAL_SDL2 "Use the internal SDL 2 CMake files stored in this project." OFF)

if( USE_INTERNAL_SDL2 )
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/SDL2")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

file(GLOB_RECURSE UTILITIES_SOURCE_FILES src/Utilities/*.cpp )
file(GLOB_RECURSE DATA_SOURCE_FILES src/Data/*.cpp )

add_library(FC_IFF_IO SHARED ${UTILITIES_SOURCE_FILES} ${DATA_SOURCE_FILES} )

target_include_directories(FC_IFF_IO PUBLIC "${PROJECT_BINARY_DIR}/src/" )

find_package(ZLIB     REQUIRED)
find_package(PNG      REQUIRED)
find_package(JSON_CPP REQUIRED)
find_package(SDL2     REQUIRED)
find_package(GLEW)
find_package(OpenGL   REQUIRED)

option(FORCE_FULL_OPENGL_2 "This enables the full OpenGL rather than OpenGLES" OFF)
option(GRAPHICS_GLES2_EXCLUDE_CONTEXT_PROFILE "Disable the forcing of the use for OpenGLES 2. Use this if the graphics card has OpenGL 2.1 support, but not OpenGLES." OFF)
option(SDL_USE_BUILTIN_OPENGL_DEFINITIONS "This forces SDL to use built in OpenGL definitions rather than use the regular include path." OFF)

configure_file(src/Config.h.in src/Config.h)

include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIRS})
include_directories(${JSON_CPP_INCLUDE_DIR})
if(GLEW_FOUND EQUAL true)
  include_directories(${GLEW_INCLUDE_DIRS})
endif()
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OPENGL_LIBRARIES})

target_link_libraries (FC_IFF_IO PRIVATE ${ZLIB_LIBRARY})
target_link_libraries (FC_IFF_IO PRIVATE ${PNG_LIBRARIES})
target_link_libraries (FC_IFF_IO PRIVATE ${JSON_CPP_LIBRARY})

add_executable( FCMissionReader ./src/FCMissionReader.cpp )
target_link_libraries( FCMissionReader PRIVATE FC_IFF_IO )

# This where the sources are for the engine.

# These source files in the engine always should be compiled no mater which API is used.
file(GLOB CONTROL_SOURCE_FILES src/Controls/*.cpp )
file(GLOB GRAPHICS_SOURCE_FILES src/Graphics/*.cpp )

# SDL 2 is used for the controls.
file(GLOB_RECURSE CONTROL_SDL2_SOURCE_FILES src/Controls/SDL2/*.cpp )

# SDL 2 is also used for the Windowing library for OpenGLES 2, and possiably Vulkan.
file(GLOB GRAPHICS_SDL2_SOURCE_FILES src/Graphics/SDL2/*.cpp )

# OpenGLES 2 is the one and only API supported for now, but more will come.
file(GLOB_RECURSE GRAPHICS_SDL2_GLES2_SOURCE_FILES src/Graphics/SDL2/GLES2/*.cpp )

add_library(FC_Engine SHARED ${CONTROL_SOURCE_FILES} ${CONTROL_SDL2_SOURCE_FILES} ${GRAPHICS_SOURCE_FILES} ${GRAPHICS_SDL2_SOURCE_FILES} ${GRAPHICS_SDL2_GLES2_SOURCE_FILES} )

target_link_libraries (FC_Engine PRIVATE FC_IFF_IO)
if(GLEW_FOUND)
  target_link_libraries (FC_Engine PRIVATE ${GLEW_LIBRARIES})
endif()
target_link_libraries (FC_Engine PRIVATE SDL2::Main)
target_link_libraries (FC_Engine PRIVATE ${OPENGL_gl_LIBRARY})
if(GLEW_FOUND EQUAL true)
  target_link_libraries (FC_Engine PRIVATE ${GLEW_LIBRARIES})
endif()

add_executable(FCModelViewer src/FCModelViewer.cpp )

target_link_libraries (FCModelViewer PRIVATE FC_IFF_IO)
target_link_libraries (FCModelViewer PRIVATE FC_Engine)

add_executable(FCMapViewer src/FCMapViewer.cpp )

target_link_libraries (FCMapViewer PRIVATE FC_IFF_IO)
target_link_libraries (FCMapViewer PRIVATE FC_Engine)
