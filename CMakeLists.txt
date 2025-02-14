cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project( "Future Cop: MIT" DESCRIPTION "Open-source reimplementation of Future Cop: L.A.P.D. game")

# Set the C++ standard to C++ 17 for file system support.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

option(FCOption_TESTS "Run tests to the project to see any potential problems." OFF)
option(FCOption_SDL2_VENDORED "Use SDL2 vendored library. Make sure the submodule is there before using this option." OFF)
option(FCOption_GLM_VENDORED "Use GLM or OpenGL Mathematics vendored library. Make sure the submodule is there before using this option." OFF)
# "Warning: Enabling this would mess with the locations where the program would normally compile to. Use jsoncpp vendored library. Make sure the submodule is there before using this option."
set(FCOption_JSONCPP_VENDORED OFF)
option(FCOption_ZLIB_VENDORED "Use zlib vendored library. Make sure the submodule is there before using this option. Zlib is not needed, but it would be nice to have for PNG export support." OFF)
option(FCOption_RELATIVE_PATHS_ONLY "This disables the complex file searchs inside the file structure of the operating system. Use this if you are making a headless version of this engine." OFF)
option(FCOption_AUTOMATIC_MAP_SWITCHING "This makes the primary game switch maps every 10 seconds. This is for debugging purposes." OFF)
option(FCOption_PREGCC_9_1_LIBRARIES "This configures this project to work with pre g++ 9.1. This is a workaround to get the filesystem working." OFF)
option(FCOption_ACTOR_FACTORY "Enables compiling the actor factory which is for development use." OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

file(GLOB_RECURSE UTILITIES_SOURCE_FILES src/Utilities/*.cpp )
file(GLOB_RECURSE DATA_SOURCE_FILES src/Data/*.cpp )

add_library(FC_IFF_IO STATIC ${UTILITIES_SOURCE_FILES} ${DATA_SOURCE_FILES} )

target_include_directories(FC_IFF_IO PUBLIC "${PROJECT_BINARY_DIR}/src/" )

if( EXISTS "${CMAKE_SOURCE_DIR}/submodules/mINI/CMakeLists.txt" )
  add_subdirectory(submodules/mINI)
  target_link_libraries( FC_IFF_IO mINI )
else()
  message( ERROR "Sorry, but the mINI submodule is required for the compilation of this project.")
endif()

if( FCOption_GLM_VENDORED )
  add_subdirectory(submodules/glm)
  target_link_libraries( FC_IFF_IO glm::glm )
else()
  find_package( glm REQUIRED )
  
  # Warn the user of the potential compilation error.
  if( GLM_VERSION VERSION_LESS 0.9.9.8 )
    if( GLM_VERSION VERSION_LESS 0.9.9 )
      message( ERROR "GLM version is ${GLM_VERSION} which is below 0.9.9. It will cause GLM to crash. To fix this error either update OpenGL Mathematics above 0.9.9.8, or use the vendored GLM library." )
    else() #Failed to release that GLM often lacks full version numbers leading to this complexity
      message( WARNING "GLM version is ${GLM_VERSION} which is might be bellow 0.9.9.8. It might cause a compilation issue. To fix this potential compilation error either update OpenGL Mathematics above 0.9.9.8, or use the vendored GLM library." )
    endif()
  endif()

  include_directories(${GLM_INCLUDE_DIRS})
endif()

# Use the vendor version if the SDL2 find files could not be found.
if( FCOption_SDL2_VENDORED )
  add_subdirectory(submodules/SDL2 EXCLUDE_FROM_ALL)

  # This is a workaround made to make this project compile. There might be a better way.
  include_directories(${PROJECT_BINARY_DIR}/submodules/SDL2/include-config-/) # TODO Make sure this works.
elseif( WIN32 )
  # If SDL2 is not there or SDL2 or SDL2main are missing then Windows will not work with this build.
  find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2 SDL2main)
else()
  # 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
  find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
  find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main) # SDL2main is optional for this build.
endif()

find_package( OpenGL REQUIRED)
find_package( Git )

# Seems to work only with "LEGACY" at this time
set (OpenGL_GL_PREFERENCE "LEGACY" CACHE STRING "Set the OPENGL affinity, if multiple libraries are available")
mark_as_advanced(OpenGL_GL_PREFERENCE)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${PROJECT_BINARY_DIR})

if( FCOption_ZLIB_VENDORED )
  add_subdirectory(submodules/zlib EXCLUDE_FROM_ALL)

  target_link_libraries (FC_IFF_IO zlib)
  include_directories(${ZLIB_PUBLIC_HDRS})
  set( ZLIB_FOUND True )

else()
  find_package( ZLIB )

  if(ZLIB_FOUND)
    target_link_libraries (FC_IFF_IO ZLIB::ZLIB)
    include_directories(${ZLIB_INCLUDE_DIRS})
  endif()
endif()

find_package( PNG )

if(PNG_FOUND AND ZLIB_FOUND)
  target_link_libraries (FC_IFF_IO PNG::PNG)
  include_directories(${PNG_INCLUDE_DIRS})
endif()

if( FCOption_JSONCPP_VENDORED )
  add_subdirectory(submodules/jsoncpp EXCLUDE_FROM_ALL)
  target_link_libraries( FC_IFF_IO  jsoncpp_lib )
else()
  find_package( JSON_CPP REQUIRED )
  include_directories(${JSON_CPP_INCLUDE_DIR})
  target_link_libraries( FC_IFF_IO ${JSON_CPP_LIBRARY})
endif()

if( FCOption_PREGCC_9_1_LIBRARIES )
  target_link_libraries( FC_IFF_IO stdc++fs )

  set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
  set(THREADS_PREFER_PTHREAD_FLAG TRUE)
  find_package( Threads REQUIRED )

  target_link_libraries( FC_IFF_IO Threads::Threads )
endif()

add_executable( FCMissionReader src/FCMissionReader.cpp )
target_link_libraries( FCMissionReader PRIVATE FC_IFF_IO )

if( FCOption_ACTOR_FACTORY )
  add_executable( FCActorFactory src/FCActorFactory.cpp )
  target_link_libraries( FCActorFactory PRIVATE FC_IFF_IO )
endif()

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
file(GLOB SOUND_SOURCE_FILES src/Sound/*.cpp )

# SDL 2 is used for the controls.
file(GLOB_RECURSE CONTROL_SDL2_SOURCE_FILES src/Controls/SDL2/*.cpp )

# SDL 2 is also used for the Windowing library for OpenGLES 2, and possibly Vulkan.
file(GLOB GRAPHICS_SDL2_SOURCE_FILES src/Graphics/SDL2/*.cpp )
include_directories( src/Graphics/SDL2/GLES2/glad/include )

# OpenGLES 2 is the one and only API supported for now, but more will come.
file(GLOB_RECURSE GRAPHICS_SDL2_GLES2_SOURCE_FILES src/Graphics/SDL2/GLES2/*.cpp src/Graphics/SDL2/GLES2/*.c )

file(GLOB_RECURSE SOUND_SDL2_DUMMY_SOURCE_FILES src/Sound/Dummy/*.cpp )

file(GLOB_RECURSE SOUND_SDL2_MOJO_AL_SOURCE_FILES src/Sound/OpenAL/*.cpp src/Sound/OpenAL/*.c )
include_directories( src/Sound/OpenAL/MojoAL/Library/AL/ )

add_library(FC_Engine STATIC ${CONTROL_SOURCE_FILES} ${CONTROL_SDL2_SOURCE_FILES} ${GRAPHICS_SOURCE_FILES} ${GRAPHICS_SDL2_SOURCE_FILES} ${GRAPHICS_SDL2_GLES2_SOURCE_FILES} ${SOUND_SOURCE_FILES} ${SOUND_SDL2_DUMMY_SOURCE_FILES} ${SOUND_SDL2_MOJO_AL_SOURCE_FILES} src/InputMenu.cpp src/MainProgram.cpp )

file(GLOB_RECURSE FCOP_MIT_GAME src/Game/*.cpp )

add_executable( FCopMIT WIN32 src/FCopMIT.cpp src/PrimaryGame.cpp src/AnnouncementPlayer.cpp src/SoundPlayer.cpp src/ParticleViewer.cpp src/ModelViewer.cpp src/Menu.cpp src/MainMenu.cpp src/OptionsMenu.cpp src/MapSelectorMenu.cpp ${FCOP_MIT_GAME} )

if( TARGET SDL2::SDL2main )
  target_link_libraries (FCopMIT PRIVATE SDL2::SDL2main)
endif()
target_link_libraries (FC_Engine PRIVATE SDL2::SDL2)
target_link_libraries (FCopMIT PRIVATE SDL2::SDL2)

target_link_libraries (FC_Engine PRIVATE FC_IFF_IO)
target_link_libraries (FC_Engine PRIVATE ${OPENGL_gl_LIBRARY})

target_link_libraries (FCopMIT PRIVATE FC_IFF_IO)
target_link_libraries (FCopMIT PRIVATE FC_Engine)

if (SDL2_FOUND)
    mark_as_advanced(SDL2_DIR)
endif()

if( FCOption_TESTS )
  # This is the testing section.
  add_subdirectory( src/Tests )
endif()
