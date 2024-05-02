## [Unreleased](https://github.com/LostArtefacts/TR2X/compare/stable...develop) - ××××-××-××
- added dev console with the following commands:
    - `/pos`
    - `/tp [room_num]`
    - `/tp [x] [y] [z]`
    - `/hp`
    - `/hp [num]`
    - `/heal`
- changed the music backend to SDL + libav

## [0.1.1](https://github.com/LostArtefacts/TR2X/compare/0.1...0.1.1) - 2024-04-27
- fixed Lara's shadow with z-buffer option on (#64, regression from 0.1)
- fixed rare camera issues (#65, regression from 0.1)
- fixed flat rectangle colors (#70, regression from 0.1)
- fixed medpacks staying open after use in Lara's inventory (#69, regression from 0.1)
- fixed pickup sprites UI drawn forever in Lara's Home (#68, regression from 0.1)

## [0.1](https://github.com/rr-/TR2X/compare/...0.1) - 2024-04-26
- added version string to the inventory
- fixed CDAudio not playing on certain versions (uses PaulD patch)
- fixed TGA screenshots crashing the game
