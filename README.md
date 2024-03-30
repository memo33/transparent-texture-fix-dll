# Transparent Base Texture DLL for SimCity 4

A DLL plugin for SimCity 4 that fixes the underground view bug (water bug) that occurs on lots using overlay textures without a base texture.

This plugin provides a base texture (0xF83443FA) that is transparent. Lots using overlay textures without a base texture need to be updated to use this transparent base texture instead.

## System requirements

- SimCity 4, version 1.1.641 (the digital edition)
  ([more info](https://community.simtropolis.com/forums/topic/762980-the-future-of-sc4-modding-the-matter-of-digital-vs-disc-and-windows-vs-macos-in-the-dll-era/))
- Windows 10+ or Linux

## Installation

Copy the DLL into the top-level directory of either Plugins folder
(place it directly in `<Documents>\SimCity 4\Plugins` or `<SC4 install folder>\Plugins`, not in a subfolder).
Copy the included .dat file anywhere into your Plugins.

Alternatively, the mod can be installed with Sc4pac: [memo:transparent-base-dll](https://memo33.github.io/sc4pac/channel/?pkg=memo:transparent-base-dll)

## Troubleshooting

The plugin should write a `.log` file in the folder containing the plugin.
The log contains status information for the most recent run of the plugin.

If no `.log` file is written, the DLL failed to load.
In this case, make sure the [Microsoft Visual C++ 2022+ x86 Redistributable](https://aka.ms/vs/17/release/vc_redist.x86.exe) is installed.

------------------------------------------------------------
## Information for developers

### Building the plugin

The DLL is compiled using `clang` as a cross-compiler.
Check the [Makefile](Makefile) for details.
```
git submodule update --init
make
```
The source code is mostly compatible with the MSVC compiler as well, but some tweaks may be needed for that.

### License

This project is licensed under the terms of the GNU Lesser General Public License version 3.0.
See [LICENSE.txt](LICENSE.txt) for more information.

#### 3rd party code

- [gzcom-dll](https://github.com/nsgomez/gzcom-dll/tree/master) Located in the vendor folder, MIT License.
- [Windows Implementation Library](https://github.com/microsoft/wil) MIT License
- [SC4Fix](https://github.com/nsgomez/sc4fix) MIT License
- [NAM-dll](https://github.com/NAMTeam/nam-dll) LGPL 3.0
