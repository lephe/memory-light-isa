#! /usr/bin/env bash

emu/emu -r -g chip8/chip8.bin --geometry 512k:256k:256k:1M --load 528384:chip8/GAMES/$1 --chip8 --scale 8
