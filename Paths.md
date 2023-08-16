# Future Cop MIT

## Data directories

Future Cop MIT uses three types of data directories, for various data types:

| Data Type  | Description                     |
|------------|---------------------------------|
| `config`   | Game configuration.             |
| `user`     | Savedgame, screenshots.         |
| `win-data` | Original Windows game data.     |
| `mac-data` | Original Macintosh game data.   |
| `psx-data` | Original Playstation game data. |

Note only one game data directory need to run the program.

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

* Config directory:
  * `%APPDATA%\FutureCopMIT\` (default)
  * `%USERPROFILE%\FutureCopMIT\`
  * `.\` (in the curent directory)
* User directory:
  * `%APPDATA%\FutureCopMIT\` (default)
  * `%USERPROFILE%\FutureCopMIT\`
  * `.\` (in the curent directory)
* Windows Data directory:
  * `%PROGRAM_FILES_X86%\Electronic Arts\Future Cop\` (default)
  * `%APPDATA%\FutureCopMIT\data\windows\`
  * `%USERPROFILE%\FutureCopMIT\data\windows\`
  * `.\data\platfrom\windows` (in the curent directory)
* Macintosh Data directory:
  * `%APPDATA%\FutureCopMIT\data\platfrom\macintosh\` (default)
  * `%USERPROFILE%\FutureCopMIT\data\platfrom\macintosh\`
  * `.\data\platfrom\macintosh\` (in the curent directory)

#### Mac OS

* Config directory:
  * `$XDG_DATA_HOME/futurecopmit/` (if `$XDG_DATA_HOME` is defined)
  * `$HOME/Library/Application Support/FutureCopMIT/` (default)
  * `$HOME/.futurecopmit/`
  * `./` (in the curent directory)
* User directory:
  * `$XDG_DATA_HOME/futurecopmit/` (if `$XDG_DATA_HOME` is defined)
  * `$HOME/Library/Application Support/FutureCopMIT/` (default)
  * `$HOME/.futurecopmit/`
  * `./` (in the curent directory)
* Windows Data directory:
  * `$XDG_DATA_HOME/futurecopmit/Data/Platform/Windows/` (if `$XDG_DATA_HOME` is defined)
  * `$XDG_DATA_DIRS/futurecopmit/Data/Platform/Windows/` (for each directory in `$XDG_DATA_DIRS` if `$XDG_DATA_DIRS` is defined)
  * `$HOME/Library/Application Support/FutureCopMIT/Data/Platform/Windows/`
  * `$HOME/.futurecopmit/Data/Platform/Windows/`
  * `./Data/Platform/Windows/` (in the curent directory)
* Macintosh Data directory:
  * `$XDG_DATA_HOME/futurecopmit/Data/Platform/Macintosh/` (if `$XDG_DATA_HOME` is defined)
  * `$XDG_DATA_DIRS/futurecopmit/Data/Platform/Macintosh/` (for each directory in `$XDG_DATA_DIRS` if `$XDG_DATA_DIRS` is defined)
  * `$HOME/Library/Application Support/FutureCopMIT/Data/Platform/Macintosh/`
  * `$HOME/.futurecopmit/Data/Platform/Macintosh/`
  * `./Data/Platform/Macintosh/` (in the curent directory)
* Playstation Data directory:
  * `./Data/Platform/Playstation/` (in the curent directory)
  * `$XDG_DATA_HOME/futurecopmit/Data/Platform/Playstation/` (if `$XDG_DATA_HOME` is defined)
  * `$XDG_DATA_DIRS/futurecopmit/Data/Platform/Playstation/` (for each directory in `$XDG_DATA_DIRS` if `$XDG_DATA_DIRS` is defined)
  * `$HOME/Library/Application Support/FutureCopMIT/Data/Platform/Playstation/`
  * `$HOME/.futurecopmit/Data/Platform/Playstation/`

  
#### Linux

* Config directory:
  * `./` (in the curent directory)
  * `$XDG_CONFIG_HOME/futurecopmit/` (if `$XDG_CONFIG_HOME` is defined)
  * `$HOME/.config/futurecopmit/` (default)
* User directory:
  * `$XDG_DATA_HOME/futurecopmit/` (if `$XDG_DATA_HOME` is defined)
  * `$HOME/.local/share/futurecopmit/`
  * `$HOME/futurecopmit/`
  * `./` (in the curent directory)
* Windows Data directory:
  * `$XDG_DATA_HOME/futurecopmit/Data/Platform/Windows` (if `$XDG_DATA_HOME` is defined)
  * `$HOME/.local/share/futurecopmit/Data/Platform/Windows` (default if `$XDG_DATA_HOME` is not defined)
  * `$XDG_DATA_DIRS/futurecopmit/Data/Platform/Windows` (for each directory in `$XDG_DATA_DIRS` if `$XDG_DATA_DIRS` is defined)
  * `/usr/local/share/futurecopmit/Data/Platform/Windows`
  * `/usr/share/futurecopmit/Data/Platform/Windows`
  * `./Data/Platform/Windows` (in the curent directory)


