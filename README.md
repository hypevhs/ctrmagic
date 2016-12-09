# CS321 Graphics Final Project - 3DS Homebrew
Source and commit history from https://github.com/libjared/ctrmagic

## File list
- png2tex.py - python program to convert png textures into raw .bin files
- Makefile - used to compile the .3dsx binary. You must use the make program provided by devkitPro.
- assets/ - texture BINs and model OBJs
- source/main.c - majority of the code lies in here
- source/diamondsquare.c - my implementation of diamond-square algorithm
- source/music.c - 3DS sound driver and music player
- source/myfs.c - file reading frontend (used to be more involved before fopen etc. started working)
- source/tinyobj_loader_c.h - [OBJ parsing library](https://github.com/syoyo/tinyobjloader-c)
- source/vshader.pica - PICA200 GPU shader program assembly source code

## Prerequisites
You must have

- a 3DS with homebrew
- devkitPro installed
- the latest ctrulib from GitHub
- the latest citro3d from GitHub
- libxmp-lite from [3ds-portlibs](https://github.com/devkitPro/3ds_portlibs) installed in $DEVKITPRO/portlibs/armv6k/{include,lib}

## How to build
- Transfer all bin/obj assets to your 3DS SD card `sdmc:/3ds/ctrmagic/`
- Press Y on Homebrew Channel to accept loading a program over WiFi

```
$ make
$ 3dslink -a <IP address> ctrmagic.3dsx
```

- Then the game should automatically run.
