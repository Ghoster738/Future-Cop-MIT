# Future Cop: MIT
Open Source Game Engine Project

* [About](#about)
* [System Requirements](#system-requirements)
* [Original game data](#original-game-data)
* [Build instructions](#build-instructions)
  + [Linux](#linux)
  + [Windows](#windows)
  + [Mac OS](#mac-os)
* [Tools overview](#tools-overview)
  + [FCMissionReader](#fcmissionreader)
    - [Usage](#usage)
    - [Parameters](#parameters)
    - [Examples](#examples)
  + [FCMapViewer](#fcmapviewer)
    - [Usage](#usage-1)
    - [Parameters](#parameters-1)
    - [Examples](#examples-1)
  + [FCModelViewer](#fcmodelviewer)
    - [Usage](#usage-2)
    - [Parameters](#parameters-2)
  + [The ID system](#the-id-system)
* [Autoloading game data](#autoloading-game-data)
* [Resource types](#resource-types)
  + [Internal](#internal)
  + [Exported](#exported)
* [Thanks](#thanks)

## About
This is an **_incomplete_** re-implementation attempt of the [Future Cop: LAPD][fcop-lapd-wikipedia-link] game, developed by Visceral Games (named **EA Redwood Shores** at the time) and released in 1998 for the PlayStation, Mac OS and Windows platforms.

[fcop-lapd-wikipedia-link]: https://en.wikipedia.org/wiki/Future_Cop:_LAPD "Wikipiedia article about the game"

Right now there is no gameplay yet. However, there is a map viewer that can display the level geometry, a model viewer that can view the models (with animations if available), and a mission reader/ripper that can extract various resources from the game files.

## System Requirements
There are no clearly defined system requirements at this time, however the project's aim is to run on very low spec computers by today standards (but not on the original computers that ran Future Cop when it was released). This should encompass most of today's devices, as the project successful compiled on a Raspberry PI 4.

Although another aim is to create portable code, this code will not work on the PlayStation 1 due to its lack of C++17 support - as developing for C99 would be harder with the constant worry of memory management. However, the code might be able to run on the Raspberry PI Zero.

## Original game data
All these tools (and the actual game when ready) require the presence of the original game data in order to function.

See the [autoloading](#autoloading-game-data) system for a simple way to use it, or use the parameters for the individual programs.

## Build instructions
### Linux
These build instructions are for Ubuntu, might work on Ubuntu derivatives.

1. Install build tools and required packages:
   ```
   apt install build-essential libglm-dev libsdl2-dev libjsoncpp-dev
   ```
    * Optional: install additional tools and packages:
       * `libpng-dev` for PNG export support
       * `git` for repository cloning and build versioning
       ```
       apt install libpng-dev git
       ```
2. Get or clone the source code:
   * Clone: `git clone https://github.com/Ghoster738/Future-Cop-MIT.git`
   * Download the latest [source code][source-code-link].

[source-code-link]: https://github.com/Ghoster738/Future-Cop-MIT/archive/refs/heads/main.zip "Download as zip"
   
3. Prepare an [out of source build][oos-build-link]:
    ```
    cd Future-Cop-MIT
    mkdir build
    cd build
    ```
[oos-build-link]: https://cgold.readthedocs.io/en/latest/tutorials/out-of-source.html "Out-of-source build documentation"
    
4. Configure the build (add your options if needed):
    ```
    cmake .. -DCMAKE_BUILD_TYPE=Release -DOpenGL_GL_PREFERENCE=LEGACY
    ```
5. Build it:
    ```
    make -j4
    ```
### Windows
> :warning:  There are no build instructions for Windows at this time.


### Mac OS
> :warning:  There are no build instructions for Mac OS at this time.

## Tools overview
This repository contains a map viewer, a model viewer, and a mission file ripper that can read game data from all platforms Future Cop was released on, with the Windows version being the best in terms of reading and understanding game data.

Note that these tools are not capable of making new mission files, they only decode and export. If you want to make a new map, use BahKooJ's [FC3DEditor][fc3deditor-link].

[fc3deditor-link]: https://github.com/BahKooJ/FC3DEditor "BahKooJ's FC3DEditor repository "

### FCMissionReader
This is a pure terminal program built in order to showcase the decoding ability. It exports the game resources from the game files to a common formats (`.png` for images, `.wav` for sounds, `.json` for actors, etc)

#### Usage
```
FCMissionReader [-h] [-i <path>] [-o <path>] [-r] [-d] [-c]
```

#### Parameters

[Heads-up about the "&nbsp;" usage for the "Parameter" header:  Not my first choice, but otherwise parameters that accept arguments (contain spaces in their signature) are displayed on multiple lines without artificially increasing the length of the "Parameter" header, forcing a wider column width. Also this means that if longer parameters or arguments are added later, the number of non breaking spaces in the header will need to be manually maintained , so alternatives are welcome]::

| &nbsp;&nbsp;Parameter&nbsp;&nbsp; | Description |
|---|---|
| `-h` | Display the help screen. |
| `-i <path>` | Mission file to be read, up to two files are supported.|
| `-o <path>` | Path to the folder where to write the decoded data. **Warning:** This should be an existing directory, and it should be empty. |
| `-r` | Export the raw resources of the mission file |
| `-d` | Export the resources of the mission file into more common data formats. |
| `-c` | Determine and write the similarities between two inputs. |

#### Examples
```
FCMissionReader -i path/to/mission/File1 -i path/to/mission/File2
FCMissionReader -i path/to/mission/File -o path/to/existing/directory
FCMissionReader -i path/to/mission/File -o path/to/existing/directory -r
FCMissionReader -i path/to/mission/File -o path/to/existing/directory -d
```

### FCMapViewer
A basic map viewer able to display  the in-game maps - only the geometry, not the models.

#### Usage
```
FCMapViewer [-h|--help]  
            [--width <number>] [--height <number>]  
            [-w] [-m] [-p] [--id <id>] [--load-all] [--platform-all]  
            [--global <path>] [--path <path>]
```
#### Parameters
[Heads-up about the "&nbsp;" usage for the "Parameter" header:  see the comment on the FCMissionReader]::

| &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Parameter&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | Description |
|---|---|
| `-h`, `--help` | Display the help screen. |
| `-width <number>` | Window/screen resolution width - defaults to `640` if not specified. |
| `-height <number>` | Window/screen resolution height - defaults to `480` if not specified. |
| `-w` | Load Windows game data from `./Data/Platform/Windows` |
| `-m` | Load Macintosh game data from `./Data/Platform/Macintosh` |
| `-p` | Load PlayStation game data from `./Data/Platform/Playstation` |
| `--id <id>` | Load the specified mission ID. Type in an invalid id to get a listing of valid IDs. |
| `--load-all` | Load every single map. Will take some time. |
| `--platform-all` | Attempt to load from all three platforms for the given ID. If `--load-all` is also present on the command line then the program will load all the levels. |
| `--global <path>` | Path to the global file which every map uses. |
| `--path <path>` | Path to the mission file which contains the rest of the data like the map.

#### Examples
```
FCMapViewer --id "map_id"
FCMapViewer --global /path/to/global_mission --path "/path/to/mission"
```

### FCModelViewer
A basic model viewer able to display the in-game models, including the animations, where available.

#### Usage
```
FCModelViewer [-h|--help]  
              [--width <number>] [--height <number>]  
              [-w] [-m] [-p] [--id <id>]  
              [--model-path <path>] [--type ??] [--start <number>]  
              [--global <path>] [--path <path>]
```

#### Parameters
[Heads-up about the "&nbsp;" usage for the "Parameter" header:  see the comment on the FCMissionReader]::

| &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Parameter&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | Description |
|---|---|
| `-h`, `--help` | Display the help screen. |
| `--width <number>` | Window/screen resolution width - defaults to `640` if not specified. |
| `--height <number>` | Window/screen resolution height - defaults to `480` if not specified. | 
| `-w` | Load Windows game data from `./Data/Platform/Windows` |  
| `-m` | Load Macintosh game data from `./Data/Platform/Macintosh` |
| `-p` | Load PlayStation game data from `./Data/Platform/Playstation` |
| `--id <id>` | Load the specified mission ID. Type in an invalid id to get a listing of valid IDs. |
| `--model-path <path>` | Where to export the models, must point to an existing directory |
| `--type ??` | ?? |
| `--start` | The index of the model to display when the program starts up |
| `--global <path>` | Path to the global file which every map uses. |
| `--path <path>` | Path to the mission file which contains the rest of the data like the map. |

### The ID system
This is a list of level names that can be used with the `--id` parameter of the FCMapViewer/FCModelViewer programs. 

These IDs can be used only if the [autoloading](#autoloading-game-data) is properly configured:

| Crime War | Precinct Assault |
|---|---|
| `griffith_park` | `pa_urban_jungle` |
| `zuma_beach` | `pa_venice_beach` |
| `la_brea_tar_pits` | `pa_hollywood_keys` |
| `venice_beach` | `pa_proving_ground` |
| `hells_gate_prison` | `pa_bug_hunt` |
| `studio_city` | `pa_la_centina` |
| `lax_spaceport` |  |
| `lax_spaceport_part_2` |  |
| `long_beach` |  |


## Autoloading game data
By placing the game data in a certain directory (depending on the platform), the FCMapViewer/FCModelViewer will automatically load and parse the data, without explicitly indicating the paths to different files.

Note that paths are case-sensitive (depending on the OS):

| Platform | Path |
|---|---|
| Windows | `./Data/Platform/Windows` |
| Macintosh | `./Data/Platform/Macintosh` |
| PlayStation | `./Data/Platform/Playstation` |

## Resource types
### Internal
This is a list of internal resources that can be read and understood - there are still some gaps in understanding all of them, noted with *italics*.

| Type | Description |
|--|--|
| `ACT` | The "actors" of the game. *As of now only one type of `act` can be read*. |
| `ANM` | Animated 64x48 images. It is only used in Crime War for some reason. |
| `BMP` | Textures. *Incomplete understanding so far of the Windows and Mac OS format*. |
| `FNTP` | Font resource. |
| `MSIC` | Music resource. |
| `NET` | Navigation node data for ground allies and enemies. |
| `OBJ` | Model information, with animation. *Incomplete understanding of some polygon data*. |
| `PTC` | Map tile index information. |
| `PYR` | *Billboards or the particle textures*. |
| `SNDS` | Voice resource. |
| `TIL` | Map cluster of tiles. Format is a bit weird, but ingenious nevertheless. |
| `WAV` | Sound effects resource. |

### Exported
This is list of exported resource formats and the corresponding internal format for the original game data.

* **Audio resources** - exported to `WAV` audio:
	* `WAV` : Very easy to convert
	* `SNDS`: PCM audio
	* `MSIC`: Also PCM audio
*  **Image resources** - exported to `QOI` (or `PNG` if support is enabled):
	* `ANM`: All the animation frames are exported in a single image with the individual frames being stacked vertically.
	* `BMP`: A single 256x256 image for each resource.
	* `PYR`: This gets separated into textures.
	* `FNTP`: Font data exported as raster image.
* **Model resources** - exported to `glTF` format:
	* `OBJ`: A single 3D model per resource is created.
	* `TIL`: A single 3D model of the tile clusters.
* **Font resources** - exported to `BMFont` format:
  * `FNTP`: Font data exported as character descriptions.
* **Meta resources** - exported to `JSON` format to make the exports more readable:
  * `ACT`: Only what seems to be "prop" types from the game are exported at this time.
  * `NET`: Only coordinates and some other properties are exported at this time.

## Thanks
### Users

* BahKooJ for various information about Future Cop.

* Killermosi for improving most of the terminal parameter system.

### Libraries

* SDL2

* GLAD2

* JsonCPP

* OpenGL Mathematics Library (GLM)

* libpng

* libz
