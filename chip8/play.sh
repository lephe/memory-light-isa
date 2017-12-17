#! /usr/bin/env bash

../emu/emu -d -g chip8.bin --geometry 512k:256k:256k:327680 --load 528384:GAMES/$1 --chip8
