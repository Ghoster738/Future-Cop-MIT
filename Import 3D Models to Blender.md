# How to Extract 3D Models From Future Cop using FCMissionReader

## Disclaimer:
The 3D Models and other assets are owned by Electronic Arts.
Distribute them at your own peril.

## Intro
For now, it is a bit complicated to extract specific models.
However, I am sure in the future I will make it so this model viewer be able to export code.

## Operating Systems Specific Instructions

### What if your using Windows?
You simply replace "./FCMissionReader" to "FCMissionReader.exe"

### Others
They are pretty much the same.

## Make Sure You have Future Cop files.
More specifically almost every asset of Future Cop are stored in IFF files.
The IFF files are called Mission files in this project. This is a list of
what they are named. They are stored in the missions folder.

### In the computer future cop installation/missions/
```
GlblData - This is not a map, but a Globals file.

--Crime War
M2C   - Griffith Park
M3A   - Zuma Beach
M3B   - La Brea Tar Pits
OV    - Venice Beach
M1A1  - Hell's Gate Prison
Un    - Studio City
LAX1  - LAX Spaceport
LAX2  - LAX Spaceport Part Two
M4A1  - Long Beach

--Precient Assualt
ConFt - Urban Jungle
HK    - Hollywood Keys
Slim  - Proving Ground
JOKE  - Bug Hunt
Mp    - La Centrina
```

### In the playstation future cop there is a directory named "cw"
```
--Crime War
fe.mis    - This is not a map, but a Globals file.
m2c.mis   - Griffith Park
m3a.mis   - Zuma Beach
m3b.mis   - La Brea Tar Pits
ov.mis    - Venice Beach
m1a1.mis  - Hell's Gate Prison
un.mis    - Studio City
lax1.mis  - LAX Spaceport
lax2.mis  - LAX Spaceport Part Two
m4a1.mis  - Long Beach
```
### In the playstation future cop there is a directory named "pa"
```
--Precient Assualt
conft.mis - Urban Jungle
hk.mis    - Hollywood Keys
slim.mis  - Proving Ground
joke.mis  - Bug Hunt
- La Centrina does not exist.
```

## Using a terminal program open the folder or same directory that FCMissionReader is in.
Create a directory in which the new files will be placed by this command.
```bash
mkdir extract
```

You will now need to locate a mission file in which you would want to extract the files from. Get its path.

For Linux, it would look something like this made up path.
```
"/home/some_user/.conf/Future Cop LAPD/Platform/Windows/missions/M2C"
```
For Windows, it would look something like this made up path.
```
"C:\Program Files (x86)\Future Cop Location\missions/M2C"
```

Now, that the mission file's path is known you can now use this command. The -i <filepath> commands the program to read the file. The -o <filepath> specifies which folder or directory would contain the decoded files. The -d commands the FCMissionReader to actually output these files.
```bash
./FCMissionReader -i "/home/some_user/.conf/Future Cop LAPD/Platform/Windows/missions/M2C" -o extract -d
```

## Making use of the decoded information.
This FCMissionReader's output could look something like this.

<img width="657" alt="image" src="https://user-images.githubusercontent.com/101094904/200162538-916bb694-fdc1-43b7-bb92-3157eabda535.png">


Now open Blender.

<img width="1440" alt="image" src="https://user-images.githubusercontent.com/101094904/200162672-e1ead510-3ea8-48af-9f71-4da4ddf0a78f.png">


Delete the box, and add a plane sized to about ~500 meters.

<img width="1440" alt="image" src="https://user-images.githubusercontent.com/101094904/200162755-1cbceb86-f684-4df3-a54c-a29075ab533c.png">

Click on the file tab.

<img width="718" alt="image" src="https://user-images.githubusercontent.com/101094904/200162800-79ec91ca-3f1f-4d4b-bc0f-176635518b57.png">

Click on "Import".

<img width="527" alt="image" src="https://user-images.githubusercontent.com/101094904/200162840-88047fe2-b51e-4754-881e-d5eeb3cbbb8e.png">

Click on "glTF 2.0."

<img width="527" alt="image" src="https://user-images.githubusercontent.com/101094904/200162861-e626b578-27ad-43c1-8e46-451163c44789.png">

You will see this menu. Go to where the extract folder is, and you should see something simular to this.

![image](https://user-images.githubusercontent.com/101094904/200163406-2626d0ab-4b8e-4f6d-b0c6-d58d19fab4a3.png)

Select every GLTF file that starts with cobj. (You would generally not want til GLTF files because you would be viewing pieces of a map.)

<img width="1202" alt="image" src="https://user-images.githubusercontent.com/101094904/200163453-90e93151-f1dd-425f-8f2a-31368c9cfe8b.png">

Applogies for programming errors. (I did not expect that).

<img width="1440" alt="image" src="https://user-images.githubusercontent.com/101094904/200163510-e0677e58-1eb2-4ef0-8d7d-19f06f3c2d3a.png">

After these errors, you should see this.

<img width="1440" alt="image" src="https://user-images.githubusercontent.com/101094904/200163590-0a2dd050-e188-4f71-8d0b-3861faae8df5.png">
  
Now, press 'Z' and set it to material preview. Wait for textures to load.

<img width="1440" alt="image" src="https://user-images.githubusercontent.com/101094904/200163637-2a4b5b8f-37b9-4620-9b97-8b80957b4ad6.png">

Now, manually seperate each model from the preview. (Yes, I noticed a bug where the CBMP one is not being loaded by Blender).

<img width="1440" alt="image" src="https://user-images.githubusercontent.com/101094904/200163959-5661c8df-a69b-46e0-be6c-faf7b781b959.png">

