#!/bin/bash

CC=clang
SRC=rose.c
REQS=./plugins/*/*.c
DEPS=('webkit2gtk-4.0')
DEBUG="" # set to "-g" for debug mode.

INCS=`pkg-config --cflags ${DEPS[@]}`
LIBS=`pkg-config --libs ${DEPS[@]}`

# Optional adblocking depends on https://github.com/jun7/wyebadblock
WYEBAB='-L/usr/lib/wyebrowser/adblock.so'

# echo $CC $INCS $LIBS $SRC $REQS $WYEBAB -o rose
$CC $DEBUG $INCS $LIBS $SRC $REQS $WYEBAB -o rose
