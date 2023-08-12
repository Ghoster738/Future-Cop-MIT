# Future Cop MIT

## Data directories

Future Cop MIT uses three types of data directories, for various data types:

| Data Type | Description             |
|-----------|-------------------------|
| `config`  | Game configuration.     |
| `user`    | Savedgame, screenshots. |
| `data`    | Original game data.     |

For each data type, several directories are searched automatically, and the
first matching one will be used.

Additionally, several command line parameters can be used to specify each
data type location, if needed:

`futurecopmit --config "path/to/config/dir/or/file" --user "path/to/user/dir/" -data "path/to/data/dir/"`

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
  * `.\` (in the curent directory)
  * `%USERPROFILE%\FutureCopMIT\` (default)
* User directory:
  * `.\` (in the curent directory)
  * `%USERPROFILE%\FutureCopMIT\`
* Data directory:
  * `.\data\` (in the curent directory)
  * `%USERPROFILE%\FutureCopMIT\data\`

#### Mac OS

* Config directory:
  * `./` (in the curent directory)
  * `$XDG_DATA_HOME/futurecopmit/` (if `$XDG_DATA_HOME` is defined)
  * `$HOME/Library/Application Support/FutureCopMIT/`
  * `$HOME/.futurecopmit/` (default)
* User directory:
  * `$XDG_DATA_HOME/futurecopmit/` (if `$XDG_DATA_HOME` is defined)
  * `$HOME/Library/Application Support/FutureCopMIT/`
  * `$HOME/.futurecopmit/`
  * `./` (in the curent directory)
* Data directory:
  * `./data/` (in the curent directory)
  * `$XDG_DATA_HOME/futurecopmit/data/` (if `$XDG_DATA_HOME` is defined)
  * `$HOME/Library/Application Support/FutureCopMIT/data/`
  * `$HOME/.futurecopmit/data/`

  
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
* Data directory:
  * `./data/` (in the curent directory)
  * `$XDG_DATA_HOME/futurecopmit/data/` (if `$XDG_DATA_HOME` is defined)
  * `$HOME/.local/share/futurecopmit/data/`
  * `$XDG_DATA_DIRS/futurecopmit` (for each directory in `$XDG_DATA_DIRS` if `$XDG_DATA_DIRS` is defined)
  * `/usr/local/share/futurecopmit/`
  * `/usr/share/futurecopmit/`


