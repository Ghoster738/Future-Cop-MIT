cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project( "Future Cop: MIT" DESCRIPTION "Open-source reimplementation of Future Cop: L.A.P.D. game")

# Set the C++ standard to C++ 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# This makes SDL2 work. It has something to do with how windows and mingw handles the main function.
if (WIN32)
  if( MINGW )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lmingw32")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lmingw32")
  else()
    message(WARNING "MinGW is not being used for this Windows build. This CMakeLists file does not account for this compiler.")
  endif()
endif()

option(ENABLED_FC_TESTS "Run tests to the project to see any potential problems." OFF)
option(USE_INTERNAL_SDL2 "Use the internal SDL 2 CMake files stored in this project." OFF)

if( USE_INTERNAL_SDL2 )
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/SDL2")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

file(GLOB_RECURSE UTILITIES_SOURCE_FILES src/Utilities/*.cpp )
file(GLOB_RECURSE DATA_SOURCE_FILES src/Data/*.cpp )

add_library(FC_IFF_IO STATIC ${UTILITIES_SOURCE_FILES} ${DATA_SOURCE_FILES} )

target_include_directories(FC_IFF_IO PUBLIC "${PROJECT_BINARY_DIR}/src/" )

find_package( ZLIB )
find_package( PNG )
find_package( glm      REQUIRED )
find_package( JSON_CPP REQUIRED )
find_package( SDL2     REQUIRED components SDL2main )
find_package( OpenGL   REQUIRED)
find_package( Git )

# Warn the user of the potential compilation error.
if( GLM_VERSION VERSION_LESS 0.9.8 )
   message( WARNING "GLM version is ${GLM_VERSION} which is not above 0.9.8. This version of GLM might produce compilation errors." )
endif()

option(USE_GLAD_2 "Use glad to use OpenGL for Windows." OFF)

if (WIN32)
  if (NOT USE_GLAD_2)
   message( WARNING "Enable the Glad 2 Option to get OpenGL 2 / OpenGLES 2 working with this project for Windows." )
  endif()
endif()

option(FORCE_FULL_OPENGL_2 "This enables the full OpenGL rather than OpenGLES" ON)
option(SDL_USE_BUILTIN_OPENGL_DEFINITIONS "This forces SDL to use built in OpenGL definitions rather than use the regular include path." OFF)

if (SDL2_FOUND)
    mark_as_advanced(SDL2_DIR)
endif()

# Seems to work only with "LEGACY" at this time
set (OpenGL_GL_PREFERENCE "LEGACY" CACHE STRING "Set the OPENGL affinity, if multiple libraries are available")
mark_as_advanced(OpenGL_GL_PREFERENCE)

if(ZLIB_FOUND)
  include_directories(${ZLIB_INCLUDE_DIR})
  if(PNG_FOUND)
    include_directories(${PNG_INCLUDE_DIRS})
  endif()
endif()

include_directories(${GLM_INCLUDE_DIRS})
include_directories(${JSON_CPP_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${PROJECT_BINARY_DIR})

if(ZLIB_FOUND)
  target_link_libraries (FC_IFF_IO PRIVATE ${ZLIB_LIBRARY})
  if(PNG_FOUND)
    target_link_libraries (FC_IFF_IO PRIVATE ${PNG_LIBRARIES})
  endif()
endif()

target_link_libraries (FC_IFF_IO PRIVATE ${JSON_CPP_LIBRARY})

# add_executable( FCMissionReader src/FCMissionReader.cpp )
# target_link_libraries( FCMissionReader PRIVATE FC_IFF_IO )

include(${CMAKE_SOURCE_DIR}/cmake/GenerateVersionHeader.cmake)

set (FUTURE_COP_MIT_VERSION "${FUTURE_COP_MIT_VERSION_FOUND}" CACHE STRING "Project version, autodetected if possible from")

configure_file (
    ${PROJECT_SOURCE_DIR}/src/Config.h.in
    ${PROJECT_BINARY_DIR}/Config.h
)

# This where the sources are for the engine.

# These source files in the engine always should be compiled no mater which API is used.
file(GLOB CONTROL_SOURCE_FILES src/Controls/*.cpp )
file(GLOB GRAPHICS_SOURCE_FILES src/Graphics/*.cpp )

# SDL 2 is used for the controls.
file(GLOB_RECURSE CONTROL_SDL2_SOURCE_FILES src/Controls/SDL2/*.cpp )

# SDL 2 is also used for the Windowing library for OpenGLES 2, and possibly Vulkan.
file(GLOB GRAPHICS_SDL2_SOURCE_FILES src/Graphics/SDL2/*.cpp )
include_directories( src/Graphics/SDL2/GLES2/glad/include )

# OpenGLES 2 is the one and only API supported for now, but more will come.
file(GLOB_RECURSE GRAPHICS_SDL2_GLES2_SOURCE_FILES src/Graphics/SDL2/GLES2/*.cpp src/Graphics/SDL2/GLES2/*.c )

add_library(FC_Engine STATIC ${CONTROL_SOURCE_FILES} ${CONTROL_SDL2_SOURCE_FILES} ${GRAPHICS_SOURCE_FILES} ${GRAPHICS_SDL2_SOURCE_FILES} ${GRAPHICS_SDL2_GLES2_SOURCE_FILES} )

target_link_libraries (FC_Engine PRIVATE FC_IFF_IO)

target_link_libraries (FC_Engine PRIVATE ${SDL2_LIBRARIES})
target_link_libraries (FC_Engine PRIVATE ${OPENGL_gl_LIBRARY})

add_executable(FCModelViewer src/FCModelViewer.cpp )

target_link_libraries (FCModelViewer PRIVATE FC_IFF_IO)
target_link_libraries (FCModelViewer PRIVATE FC_Engine)
target_link_libraries (FCModelViewer PRIVATE SDL2main)

add_executable(FCMapViewer src/FCMapViewer.cpp )

target_link_libraries (FCMapViewer PRIVATE FC_IFF_IO)
target_link_libraries (FCMapViewer PRIVATE FC_Engine)
target_link_libraries (FCMapViewer PRIVATE SDL2main)

if( ENABLED_FC_TESTS )
  # This is the testing section.
  add_subdirectory( src/Tests )
endif()
