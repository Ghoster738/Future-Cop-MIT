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
GlblData - This does not store any 3D model.

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

## For Example

```bash
./FCMissionReader -i path/to/mission/File -o path/to/existing/directory -d
```
