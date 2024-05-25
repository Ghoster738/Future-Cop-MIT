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
  + [FCopMIT](#fcopmit)
    - [Usage](#usage-1)
    - [Parameters](#parameters-1)
    - [Examples](#examples-1)
  + [The ID system](#the-id-system)
* [Loading game data](#loading-game-data)
* [Resource types](#resource-types)
  + [Internal](#internal)
  + [Exported](#exported)
* [Thanks](#thanks)

## About
This is an **_incomplete_** re-implementation attempt of the [Future Cop: LAPD][fcop-lapd-wikipedia-link] game, developed by Visceral Games (named **EA Redwood Shores** at the time) and released in 1998 for the PlayStation, Mac OS and Windows platforms.

[fcop-lapd-wikipedia-link]: https://en.wikipedia.org/wiki/Future_Cop:_LAPD "Wikipiedia article about the game"

Right now there is no gameplay yet. However, there is a map viewer that can display the level geometry, a model viewer that can view the models (with animations if available), and a mission reader/ripper that can extract various resources from the game files.

Disclaimer: This project no way is affliated with the Massachusetts Institute of Technology (MIT) except that its license is choosen by Ghoster738 for this project.

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
       * `libz-dev` for compression, but libpng-dev requires it so apt would install this with libpng-dev.
       * `libpng-dev` for PNG export support
       * `git` for repository cloning and build versioning
       ```
       apt install libpng-dev git
       ```
2. Get or clone the source code:
   * Clone:
     ```
     git clone https://github.com/Ghoster738/Future-Cop-MIT.git
     ```
     
   * Download the latest [source code][source-code-link]. (Warning: using git is strongly recommended, because of the ease of automatically downloading the submodules.)

[source-code-link]: https://github.com/Ghoster738/Future-Cop-MIT/archive/refs/heads/main.zip "Download as zip"

3. Get the submodules. **Submodule mINI is required for compilation**:
   * For git use this command it would download every submodule used in this project:
     ```
     git submodule update --init --recursive --progress --depth 1
     ```
   * For the direct download all that is required is to get [mINI][mini-source-code-link] and put it into the submodules mINI directory.

[mini-source-code-link]: https://github.com/pulzed/mINI/archive/refs/heads/master.zip
   
5. Prepare an [out of source build][oos-build-link]:
    ```
    cd Future-Cop-MIT
    mkdir build
    cd build
    ```
[oos-build-link]: https://cgold.readthedocs.io/en/latest/tutorials/out-of-source.html "Out-of-source build documentation"
    
5. Configure the build (add your options if needed):

   A. If the gcc version 9.1 or above use this command:
   ```
   cmake .. -DCMAKE_BUILD_TYPE=Release -DOpenGL_GL_PREFERENCE=LEGACY
   ```

   B. Pre 9.1 of gcc would need this command to compile:
   ```
   cmake .. -DFCOption_PREGCC_9_1_LIBRARIES=ON -DCMAKE_BUILD_TYPE=Release -DOpenGL_GL_PREFERENCE=LEGACY
   ```

6. Build it with this command (-j is optional, but it would make compiling faster):
    ```
    make -j<Number of CPU Cores>
    ```
    For example, if the computer has 8 threads then you would use this command:
    ```
    make -j8
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

### FCopMIT
The primary executable of this project.

#### Usage
```
FCopMIT [-h|--help]
        [--width <number>] [--height <number>]
        [--res <number>x<number>]
        [--fullscreen|--window]
        [--config <path>] [--user <path>]
        [--win-data <path>] [--mac-data <path>] [--psx-data <path>]
        [--path <file path>] [--global <file path>]
```
#### Parameters
[Heads-up about the "&nbsp;" usage for the "Parameter" header:  see the comment on the FCMissionReader]::

| &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Parameter&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | Description |
|---|---|
| `-h`, `--help` | Display this help screen and exit. |
| `--width <number>` | Window width, in pixels. |
| `--height <number>` | Window height, in pixels. |
| `---res <number>x<number>` | Window width and height in pixels, as a single parameter. |
| `--fullscreen` | Full screen mode. |
| `--window` | Window mode. |
| `--user <path>` | Path to directory - savegames and screenshots |
| `--config <path>` | Path to game configuration directory |
| `--win-data <path>` | Path to directory - Future Cop LAPD original Windows data |
| `--mac-data <path>` | Path to directory - Future Cop LAPD original Macintosh data |
| `--psx-data <path>` | Path to directory - Future Cop LAPD original Playstation data |
| `--export-path <path>` | Path to directory - path to where exported models go from the model viewer |
| `--global <file path>` | Path to the global file which every map uses. |
| `--path <file path>` | Path to the mission file which contains the rest of the data like the map. |

#### Examples
```
FCopMIT --window --res 1024x768
FCopMIT --fullscreen --res 1920x1080
FCopMIT --global /path/to/global_mission --path "/path/to/mission"
```

### The ID system
This is a list of level names that are used internally.

| Crime War | Precinct Assault |
|---|---|
| `griffith-park` | `pa-urban-jungle` |
| `zuma-beach` | `pa-venice-beach` |
| `la-brea-tar-pits` | `pa-hollywood-keys` |
| `venice-beach` | `pa-proving-grounds` |
| `hells-gate-prison` | `pa-bug-hunt` |
| `studio-city` | `pa-la-centina` |
| `lax-spaceport` |  |
| `lax-spaceport-part-2` |  |
| `long-beach` |  |


## Loading game data
By placing the game data in a certain directory (depending on the platform) the FCopMIT will load and parse data.

Please see the documentation file named [Paths](Paths.md)

Note that paths are case-sensitive on Mac and Linux:

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

* Killermosi/kkmic for improving parameter system, the filesystem, and the options system.

### Libraries

* SDL2

* GLAD2

* mojoAL

* OpenGL Mathematics Library (GLM)

* mINI

* JsonCPP

* libpng

* libz
