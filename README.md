# Super Mario Spooky Adventure
## Source Code

### Preamble

Here's the source code of **SUPER MARIO SPOOKY ADVENTURE**. It compiles, as far as I know. There's no notable new features, just some hardcoding changes and stuff like that.

*-MandyIGuess, 1st February 2023*

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

### Setup

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
    `Kamek/tools/cw` if you are using Wine.

### Usage

There are two scripts in the Kamek folder that invoke `makeGame.py`, which will build the mod in two different ways.

- makeSpookyAdv (regular Spooky Adventure, doesn't have any sort of debugging features, or other features that are commented out)
- makeSpookyDebug (has extra debugging and other misc features in it, that aren't compiled with regular SpookyAdv)

On Windows, run the batch versions - you can just double-click them.

On macOS, Linux, or other Unix-like systems, use the bash versions.
Open a terminal in the same directory and run them like:

`./makeSpookyAdv`

The resulting output is placed in a folder called `Build_X`
(where X is the name of the Kamek project). These files go in `/SpookyRes` on the game disc.

[cw]: http://cache.nxp.com/lgfiles/devsuites/PowerPC/CW55xx_v2_10_SE.exe?WT_TYPE=IDE%20-%20Debug,%20Compile%20and%20Build%20Tools&WT_VENDOR=FREESCALE&WT_FILE_FORMAT=exe&WT_ASSET=Downloads&fileExt=.exe
[cwIA]: http://web.archive.org/web/20160602205749/http://www.nxp.com/products/software-and-tools/software-development-tools/codewarrior-development-tools/downloads/special-edition-software:CW_SPECIALEDITIONS
[dkp]:
https://devkitpro.org/wiki/Getting_Started
[Wine]:
https://www.winehq.org/
