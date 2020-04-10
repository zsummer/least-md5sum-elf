#!/bin/bash
gcc-8  -Os -fdata-sections -ffunction-sections -flto  -Wl,--gc-sections -Wl,--strip-all -nostdlib -nostdinc -m32 -fno-asynchronous-unwind-tables start.c
