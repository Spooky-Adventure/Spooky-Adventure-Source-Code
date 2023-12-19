# Super Mario Spooky Adventure
## Source Code

### Preamble

Here's the source code of Super Mario Spooky Adventure, a work-in-progress hack for NSMBW. Feel free to look around and use whatever's here.
If you have any questions, or simply want to follow development of the mod, join our [Discord Server][discord].

This mod uses the custom code loader from [NSMBW: The Prankster Comets][nsmbwtpc], credits to that go to [Asu-chan](https://github.com/Asu-chan) and [CLF78](https://github.com/CLF78).

Here's a list of the biggest changes made so far:
- Removal of building tools (and some code) for other Newer Team projects
- Updating of Newer-related names to Spooky Adventure (renamed the folders and project stuffs)
- Slight cleanup here and there
- Other miscellaneous changes & additions

*- MandyIGuess, 4th August 2022*

### Licensing

The Spooky Adventure source code and tools are released under the MIT license.
See the `LICENSE` file in this repository for full details.

Copyright (c) 2022/2023 Spooky Adventure Team

### Requirements

- Windows or OS that runs [Wine]
- [devkitPPC][dkp]
- Python 3.x
- Python libraries: PyYAML, pyelftools
- NXP ['CodeWarrior Special Edition'][cw] for MPC55xx/MPC56xx v2.10
  - If this direct link doesn't work, the original page is
    [available on the Internet Archive][cwIA].
  - The installer does not work with Wine; you may use `extractCW.sh`
    from the root of the repo in the same directory as the installer to
    extract the necessary files instead (requires cabextract).
  - You only need the Command Line Tools component in the custom
    install configuration.
  - After installing it, you need `license.dat` from the installation root,
    and the DLL and EXE files inside `PowerPC_EABI_Tools/Command_Line_Tools`.

Full setup instructions are included below.

## Installation

### Loader

***Note: Compiling the loader isn't necessary if you're just editing the code in the Kamek folder.***

- Install devkitPro's devkitPPC. Official instructions for this are found [here][dkp].

Once that is installed, there is a `compileLoader.bat` script in the Loader folder, running it will output a `Loader.bin` file.

The outputed file goes in the root of the Riivolution patch.

### Setup

This focuses on compiling the code in the Kamek folder.

- First install Python 3. If on Windows, ensure it is added to PATH
  upon installation. Next, run:

  - On Windows:
    `py -3 -m pip install pyyaml pyelftools`

  - On other OSs:
    `python3 -m pip install pyyaml pyelftools`

- Install DevkitPro's devkitPPC. Their official instructions for
  this are found [here][dkp].

- The last dependency is NXP ['CodeWarrior Special Edition'][cw]
  for MPC55xx/MPC56xx v2.10. You will also need [Wine] if you
  are running an OS other than Windows.

  - NOTE: the installer is known to fail with Wine.
    You may extract the files from it with ``extractCW.sh``
    on the root of the repo (which requires cabextract,
    available from most package managers).

  - CodeWarrior's tools are by default installed to:
  
  `C:\Program Files (x86)\Freescale\CW for MPC55xx and MPC56xx 2.10\PowerPC_EABI_Tools\Command_Line_Tools`

  - Copy the files inside to `Kamek/tools/cw` - Kamek assumes they
    are there.

  - Also, on Windows, make sure the following system variable is added
    to your environment variables, with the name `LM_LICENSE_FILE`:

  `C:\Program Files (x86)\Freescale\CW for MPC55xx and MPC56xx 2.10\license.dat`

  - The ``license.dat`` file may alernatively be copied to
    `Kamek/tools/cw` if you are using Wine, or prefer not to add the system variable.

### Usage

There is a `makeSpookyAdv` script in the Kamek folder that invokes `makeGame.py`, which will build the project.

On Windows, run the batch version - you can just double-click them.

On macOS, Linux, or other Unix-like systems, use the bash version.
Open a terminal in the same directory and run it like:

`./makeSpookyAdv`

The resulting output is placed in a folder called `Build_SpookyProject`. These files go in `/SpookyRes` on the game disc.

[cw]:
http://cache.nxp.com/lgfiles/devsuites/PowerPC/CW55xx_v2_10_SE.exe?WT_TYPE=IDE%20-%20Debug,%20Compile%20and%20Build%20Tools&WT_VENDOR=FREESCALE&WT_FILE_FORMAT=exe&WT_ASSET=Downloads&fileExt=.exe
[cwIA]:
http://web.archive.org/web/20160602205749/http://www.nxp.com/products/software-and-tools/software-development-tools/codewarrior-development-tools/downloads/special-edition-software:CW_SPECIALEDITIONS
[discord]:
https://discord.gg/fW5D3b6WQ8
[dkp]:
https://devkitpro.org/wiki/Getting_Started
[nsmbwtpc]:
https://github.com/Asu-chan/NSMBWThePranksterComets
[Wine]:
https://www.winehq.org/
