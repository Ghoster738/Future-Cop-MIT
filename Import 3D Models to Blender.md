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
```
GlblData - This does not store any 3D model.
M2C - griffith_park
M3A - zuma_beach
```

## TODO Complete this when I have more time.

## For Example

```bash
~ ./FCMissionReader -i path/to/mission/File -o path/to/existing/directory -d
```
