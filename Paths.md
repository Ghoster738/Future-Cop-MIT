# Future Cop MIT

## Data directories

Future Cop MIT uses five types of data directories, for various data types:

| Data Type  | Description                     |
|------------|---------------------------------|
| `config`   | Game configuration.             |
| `user`     | Savedgame, screenshots.         |
| `win-data` | Original Windows game data.     |
| `mac-data` | Original Macintosh game data.   |
| `psx-data` | Original Playstation game data. |

Note: Only one game data directory is needed to run this software.

For each data type, several directories are searched automatically, and the
first matching one will be used.

Additionally, several command line parameters can be used to specify each
data type location, if needed:

`futurecopmit --config "path/to/config/dir/or/file" --user "path/to/user/dir/" --win-data "path/to/data/dir/"`

### Search algorithm


### Directories location

Depending on the platform, different directories are used. They are searched in
the order specified below.

For the configuration file, the directories listed are searched for the
configuration file itself (named `futurecop.ini`) before being used. The
directory marked with `(default)` will be used if no configuration file
can be located in any of the searched directories.

#### Windows

* Config Directory:
  * `%APPDATA%\FutureCopMIT\` (default)
  * `%USERPROFILE%\FutureCopMIT\`
  * `.\` (in the curent directory)
* User Directory:
  * `%APPDATA%\FutureCopMIT\` (default)
  * `%USERPROFILE%\FutureCopMIT\`
  * `.\` (in the curent directory)
* Windows Data Directory:
  * `%PROGRAM_FILES_X86%\Electronic Arts\Future Cop\` (default)
  * `%APPDATA%\FutureCopMIT\data\windows\`
  * `%USERPROFILE%\FutureCopMIT\data\windows\`
  * `.\data\platform\windows` (in the curent directory)
* Macintosh Data Directory:
  * `%APPDATA%\FutureCopMIT\data\platform\macintosh\` (default)
  * `%USERPROFILE%\FutureCopMIT\data\platform\macintosh\`
  * `.\data\platform\macintosh\` (in the curent directory)
* Playstation Data Directory:
  * `%APPDATA%\FutureCopMIT\data\platform\playstation\` (default)
  * `%USERPROFILE%\FutureCopMIT\data\platform\playstation\`
  * `.\data\platform\playstation\` (in the curent directory)

#### Mac OS

* Config Directory:
  * `$XDG_DATA_HOME/futurecopmit/` (if `$XDG_DATA_HOME` is defined)
  * `$HOME/Library/Application Support/FutureCopMIT/` (default)
  * `$HOME/.futurecopmit/`
  * `./` (in the curent directory)
* User Directory:
  * `$XDG_DATA_HOME/futurecopmit/` (if `$XDG_DATA_HOME` is defined)
  * `$HOME/Library/Application Support/FutureCopMIT/` (default)
  * `$HOME/.futurecopmit/`
  * `./` (in the curent directory)
* Windows Data Directory:
  * `$XDG_DATA_HOME/futurecopmit/Data/Platform/Windows/` (if `$XDG_DATA_HOME` is defined)
  * `$XDG_DATA_DIRS/futurecopmit/Data/Platform/Windows/` (for each directory in `$XDG_DATA_DIRS` if `$XDG_DATA_DIRS` is defined)
  * `$HOME/Library/Application Support/FutureCopMIT/Data/Platform/Windows/`
  * `$HOME/.futurecopmit/Data/Platform/Windows/`
  * `./Data/Platform/Windows/` (in the curent directory)
* Macintosh Data Directory:
  * `$XDG_DATA_HOME/futurecopmit/Data/Platform/Macintosh/` (if `$XDG_DATA_HOME` is defined)
  * `$XDG_DATA_DIRS/futurecopmit/Data/Platform/Macintosh/` (for each directory in `$XDG_DATA_DIRS` if `$XDG_DATA_DIRS` is defined)
  * `$HOME/Library/Application Support/FutureCopMIT/Data/Platform/Macintosh/`
  * `$HOME/.futurecopmit/Data/Platform/Macintosh/`
  * `./Data/Platform/Macintosh/` (in the curent directory)
* Playstation Data Directory:
  * `$XDG_DATA_HOME/futurecopmit/Data/Platform/Playstation/` (if `$XDG_DATA_HOME` is defined)
  * `$XDG_DATA_DIRS/futurecopmit/Data/Platform/Playstation/` (for each directory in `$XDG_DATA_DIRS` if `$XDG_DATA_DIRS` is defined)
  * `$HOME/Library/Application Support/FutureCopMIT/Data/Platform/Playstation/`
  * `$HOME/.futurecopmit/Data/Platform/Playstation/`
  * `./Data/Platform/Playstation/` (in the curent directory)

  
#### Linux

* Config Directory:
  * `$XDG_CONFIG_HOME/futurecopmit/` (if `$XDG_CONFIG_HOME` is defined)
  * `$HOME/.config/futurecopmit/` (default)
  * `./` (in the curent directory)
* User Directory:
  * `$XDG_DATA_HOME/futurecopmit/` (if `$XDG_DATA_HOME` is defined)
  * `$HOME/.local/share/futurecopmit/`
  * `$HOME/futurecopmit/`
  * `./` (in the curent directory)
* Windows Data Directory:
  * `$XDG_DATA_HOME/futurecopmit/Data/Platform/Windows` (if `$XDG_DATA_HOME` is defined)
  * `$HOME/.local/share/futurecopmit/Data/Platform/Windows` (default if `$XDG_DATA_HOME` is not defined)
  * `$XDG_DATA_DIRS/futurecopmit/Data/Platform/Windows` (for each directory in `$XDG_DATA_DIRS` if `$XDG_DATA_DIRS` is defined)
  * `/usr/local/share/futurecopmit/Data/Platform/Windows`
  * `/usr/share/futurecopmit/Data/Platform/Windows`
  * `./Data/Platform/Windows` (in the curent directory)


