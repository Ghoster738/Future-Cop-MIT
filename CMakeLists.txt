cmake_minimum_required(VERSION 2.8)

project( FutureCopDecoder )

set(CMAKE_CXX_FLAGS "-std=c++11 -pthread ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

add_library(FC_IFF_IO SHARED ./src/Utilities/DataTypes.cpp ./src/Utilities/DataHandler.cpp ./src/Utilities/Buffer.cpp ./src/Utilities/Math.cpp ./src/Utilities/ImageData.cpp ./src/Utilities/ModelBuilder.cpp ./src/Data/Mission/Resource.cpp ./src/Data/Mission/ACT/Prop.cpp ./src/Data/Mission/ACT/Unknown.cpp ./src/Data/Mission/ACTResource.cpp ./src/Data/Mission/ACTManager.cpp ./src/Data/Mission/ACT/Internal/Hash.cpp ./src/Data/Mission/UnkResource.cpp ./src/Data/Mission/PYRResource.cpp ./src/Data/Mission/Til/Colorizer.cpp ./src/Data/Mission/Til/Mesh.cpp ./src/Data/Mission/TilResource.cpp ./src/Data/Mission/NetResource.cpp ./src/Data/Mission/PTCResource.cpp ./src/Data/Mission/SNDSResource.cpp ./src/Data/Mission/MSICResource.cpp ./src/Data/Mission/ANMResource.cpp ./src/Data/Mission/BMPResource.cpp ./src/Data/Mission/FontResource.cpp ./src/Data/Mission/ObjResource.cpp ./src/Data/Mission/WAVResource.cpp ./src/Data/Mission/IFF.cpp ./src/Data/Manager.cpp )

find_package(ZLIB     REQUIRED)
find_package(PNG      REQUIRED)
find_package(JSON_CPP REQUIRED)
find_package(SDL2     REQUIRED)
find_package(OpenGL   REQUIRED)

include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIRS})
include_directories(${JSON_CPP_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OPENGL_LIBRARIES})

target_link_libraries (FC_IFF_IO PRIVATE ${ZLIB_LIBRARY})
target_link_libraries (FC_IFF_IO PRIVATE ${PNG_LIBRARIES})
target_link_libraries (FC_IFF_IO PRIVATE ${JSON_CPP_LIBRARY})
# target_link_libraries(FC_IFF_IO PRIVATE stdc++fs)

add_executable( FCMissionReader ./src/FCMissionReader.cpp )
target_link_libraries( FCMissionReader PRIVATE FC_IFF_IO )

# add_executable( FCPaletteFinder ./src/NumberSequenceGenerator.cpp ./src/FCPaletteFinder.cpp )
# target_link_libraries( FCPaletteFinder PRIVATE FC_IFF_IO )

add_library(FC_Graphics_OpenglES2 SHARED ./src/Controls/SDL2/Input.cpp ./src/Controls/SDL2/InputSet.cpp ./src/Controls/SDL2/StandardInputSet.cpp ./src/Controls/SDL2/CursorInputSet.cpp ./src/Controls/SDL2/System.cpp ./src/Graphics/SDL2/GLES2/Internal/Extensions.cpp ./src/Graphics/SDL2/GLES2/Internal/VertexAttributeArray.cpp ./src/Graphics/SDL2/GLES2/Internal/Mesh.cpp ./src/Graphics/SDL2/GLES2/Internal/StaticModelDraw.cpp ./src/Graphics/SDL2/GLES2/Internal/MorphModelDraw.cpp ./src/Graphics/SDL2/GLES2/Internal/SkeletalModelDraw.cpp ./src/Graphics/SDL2/GLES2/Internal/World.cpp ./src/Graphics/SDL2/GLES2/Internal/Texture2D.cpp ./src/Graphics/SDL2/GLES2/Internal/FontSystem.cpp ./src/Graphics/SDL2/GLES2/Internal/Shader.cpp ./src/Graphics/SDL2/GLES2/Internal/Program.cpp ./src/Graphics/SDL2/GLES2/Text2DBuffer.cpp ./src/Graphics/Camera.cpp ./src/Graphics/SDL2/Window.cpp ./src/Graphics/SDL2/GLES2/ModelInstance.cpp ./src/Graphics/SDL2/GLES2/Environment.cpp )
target_link_libraries (FC_Graphics_OpenglES2 PRIVATE FC_IFF_IO)
target_link_libraries (FC_Graphics_OpenglES2 PRIVATE ${SDL2_LIBRARIES})
target_link_libraries (FC_Graphics_OpenglES2 PRIVATE ${OPENGL_gl_LIBRARY})

add_executable(FCModelViewer ./src/FCModelViewer.cpp )

target_link_libraries (FCModelViewer PRIVATE FC_IFF_IO)
target_link_libraries (FCModelViewer PRIVATE FC_Graphics_OpenglES2)

add_executable(FCMapViewer ./src/FCMapViewer.cpp )

target_link_libraries (FCMapViewer PRIVATE FC_IFF_IO)
target_link_libraries (FCMapViewer PRIVATE FC_Graphics_OpenglES2)
