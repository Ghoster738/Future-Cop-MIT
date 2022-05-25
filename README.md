# Future Cop: MIT

Open Source Game Engine Project

# About

This is an __incomplete__ reimplementation attempt of Future Cop: L.A.P.D. Right now there is no gameplay yet. However, there is a map viewer which views the contents of the mission files. There is also a model viewer which can view the models with animations if avialable. There is also a MissionReader which is a pure terminal program. Unforturnatly for now, the programs require using the terminal.

## What are these tools

This has a map viewer, a model viewer, and a mission file ripper. However, these tools are not capable of making new mission files! They only decode and export. If you want to make a new map use BahTwoKooj's tools, https://github.com/BahKooJ/Fcop-Parse-Lib and https://github.com/BahKooJ/FCEditor-build. However, this code can actually read from three versions of Future Cop Windows, Macintosh and Playstation. However, this can read from the Windows version of Future Cop the best.

# System Requirements

I plan on making this code work on very low spec computers, but not the original computers that ran Future Cop. This should work on most computers, and in a mater of fact this code was successful compiled on the Raspberry PI 4! I plan on making this code on being very portable. The project is not planed to work on the Playstation 1, because of its lack of C++11 support. As making this project would be harder to write in C99 with the constant worry of memory management. However, the code might be able to run on Raspberry PI nano, which would be cool.

# Project Requirements

The files from Future Cop: LAPD. I did not provide those files because they are under copyright for Electronic Arts.

# Compilation Requirements

## Warning

&ensp; I only compiled this for the Linux operating system using g++.

&ensp; The Windows port is graphics does not work.

## For the Mission Reader

This source code requires C++ 11. It compiles fine using the G++ compiler. The libraries used are...

&ensp; __zlib__

&ensp; __libpng__

&ensp; __jsoncpp__

## For the Map and Model Viewer

&ensp; The requirements from the Mission Reader

&ensp; __SDL2__

&ensp; __OpenGL__ more specifically __OpenGLES 2__ is used.

# What kind of mission resources can be read by this Project

&ensp; __ACT__: I think this holds the "actors" of the game. As of now only one type of act can be read. More will come.

&ensp; __ANM__: This is the animated 64x48 images. It is only used in Crime War in Future Cop for some reason.

&ensp; __BMP__: This holds the textures. However, there is still things I do not understand about the Windows and Mac format.

&ensp; __FNTP__: This holds the font resource.

&ensp; __MSIC__: This holds the music audio data.

&ensp; __NET__: This holds the Navigation Node data for mostly the ground allies and the enemies.

&ensp; __Obj__: This holds the Model Information for Future Cop. I even got out the animations. However, there is some polygon it cannot read... yet.

&ensp; __PTC__: This holds the map tile index information.

&ensp; __PYR__: This holds billboards or the particle's textures for Future Cop.

&ensp; __SNDS__: This holds the voice data of Future Cop.

&ensp; __Til__: The map cluster of tiles. This format is a little weird but cleaver.

&ensp; __WAV__: This holds the audio for the 3d sound effects.

## What does the FCReader Decoder Export.

### WAV audio for

&ensp; __WAV__: Very easy to convert.

&ensp; __SNDS__: PCM audio.

&ensp; __MSIC__: Also PCM audio.

### PNG images for

&ensp; __ANM__: All the animation frames are stored in a single texture with the images being stored vertically.

&ensp; __BMP__: A single 256x256 texture for each BMP.

&ensp; __PYR__: This gets separated into textures.

### GLTF the model format

Every one of those files does not store the textures, but uses the paths to the textures locally.

&ensp; __Obj__: A single 3D model per resource is created. The bone animations gets excluded.

&ensp; __Til__: A single 3D model of the tile clusters.

### Font

&ensp; __FNTP__: This writes two files. One is an BMFont text file, and the other is a PNG image corresponding to it.

### JSON is used to make the exports more readable

&ensp; __ACT__: Only exports what I think are the "Prop" types from the game.

&ensp; __NET__: This holds the positions and what I could gather from this format.
