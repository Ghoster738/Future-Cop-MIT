# Future Cop: MIT

Open Source Game Engine Project

# About

This is an __incomplete__ reimplementation attempt of Future Cop: L.A.P.D. Right now there is no gameplay yet. However, there is a map viewer which views the contents of the mission files. There is also a model viewer which can view the models with animations if avialable. There is also a MissionReader which is a pure terminal program. Unforturnatly for now, the programs require using the terminal.

## What are these tools

This has a map viewer, a model viewer, and a mission file ripper. However, these tools are not capable of making new mission files! They only decode and export. If you want to make a new map use BahTwoKooj's tools, https://github.com/BahKooJ/Fcop-Parse-Lib and https://github.com/BahKooJ/FCEditor-build. However, this code can actually read from three versions of Future Cop Windows, Macintosh and Playstation. However, this can read from the Windows version of Future Cop the best.

## Command Line Arguments for FCMissionReader
This is a pure terminal program built in order to show the decoding abitily of this program.

This command will display what the commands do.

**./FCMissionReader -h**

The **-i** command loads the mission file to be read.

**./FCMissionReader -i path/to/mission/File**

The **-o** command tells where the output directory should go. **Warning:** This should be an existing directory, and it should be empty.

**./FCMissionReader -o path/to/mission/File**

The **-r** command dumps the raw resources of the mission file.

**./FCMissionReader -i path/to/mission/File -o path/to/existing/directory -r**

The **-d** command exports the resources of the mission file into more common data formats.

**./FCMissionReader -i path/to/mission/File -o path/to/existing/directory -d**

## Command Line Arguments for FCMapViewer and FCModelViewer
The command line for the Future Cop Viewers are a bit different from the FCMissionReader.

### Direct Pathing Method
This indirect pathing is created for the modder, so they would be able to view their own maps.

**./FCMapViewer --global /path/to/global_mission --path "/path/to/mission"**

**--global** is the path to the global file which every map uses.

**--path** is the path to the mission file which contains the rest of the data like the map.

### Autoloader Method
If you want to load a specific mission file with a specific global file you could do this.

**-w** means load from ./Data/Platform/Windows

**-m** means load from ./Data/Platform/Macintosh

**-p** means load from ./Data/Platform/Playstation

**--id** means which mission id to load from. See **The ID System** for more info

### Testing Arguments for the Autoloader
This is useful for testing only. **These arguments are useless and overkill for non-developers.**

**--platform-all** This tells the mission manager to attempt to load from all three platforms for the given --id. If --load-all is also present on the command line then the program will load all the levels.

**--load-all** If you like high loading times use this. This tells the mission manager to load every single map.

#### The ID system.
This only works if the **Autoloading Requirements** are meet. These are the valid names of the system

**The IDs for Crime War names.**

griffith_park

zuma_beach

la_brea_tar_pits

venice_beach

hells_gate_prison

studio_city

lax_spaceport

lax_spaceport_part_2

long_beach

**The IDs for Precinct Assault.**

pa_urban_jungle

pa_venice_beach

pa_hollywood_keys

pa_proving_ground

pa_bug_hunt

pa_la_centina

# System Requirements

I plan on making this code work on very low spec computers, but not the original computers that ran Future Cop. This should work on most computers, and in a mater of fact this code was successful compiled on the Raspberry PI 4! I plan on making this code on being very portable. The project is not planed to work on the Playstation 1, because of its lack of C++11 support. As making this project would be harder to write in C99 with the constant worry of memory management. However, the code might be able to run on Raspberry PI nano, which would be cool.

# Project Requirements

The files from Future Cop: LAPD. I did not provide those files because they are under copyright for Electronic Arts.

## Autoloading Requirements
This holds the file structure of the program. (Right now only mission files can be read).
In the future, this project should store these files to different places depending on the OS.

Install_Destination/Data/Platform/Windows     -- should have the installation files that contains the primary executable and the missions floder.

Install_Destination/Data/Platform/Macintosh   -- should have the installation files that contains the primary executable for the Mac files.

Install_Destination/Data/Platform/Playstation -- should have the installation files that contains the Playstation 1 files.

# Compilation Requirements

## Warning

&ensp; I only compiled this for the Linux operating system using g++.

&ensp; The Windows port is graphics does not work.

## For the Mission Reader

This source code requires C++ 11. It compiles fine using the G++ compiler. The libraries used are...

&ensp; __zlib__ (Optional)

&ensp; __libpng__ (Optional, but requires __zlib__)

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

### QOI or PNG (if compilied with libpng) images for

&ensp; __ANM__: All the animation frames are stored in a single texture with the images being stored vertically.

&ensp; __BMP__: A single 256x256 texture for each BMP.

&ensp; __PYR__: This gets separated into textures.

&ensp; __FNTP__: The font is uses raster images after all.

### GLTF the model format

Every one of those files does not store the textures, but uses the paths to the textures locally.

&ensp; __Obj__: A single 3D model per resource is created. The bone animations gets excluded.

&ensp; __Til__: A single 3D model of the tile clusters.

### Font

&ensp; __FNTP__: This writes two files. One is an BMFont text file.

### JSON is used to make the exports more readable

&ensp; __ACT__: Only exports what I think are the "Prop" types from the game.

&ensp; __NET__: This holds the positions and what I could gather from this format.
