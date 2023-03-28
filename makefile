# Variables

## C compiler
CC=clang

## Main file
SRC=rose.c

## Dependencies
DEPS='webkit2gtk-4.0'
DEBUG= #'-g'

INCS=`pkg-config --cflags ${DEPS}`
LIBS=`pkg-config --libs ${DEPS}`

# Plugins
# REQS=./plugins/stand_in/stand_in.c
LIBRE_REDIRECT=./plugins/libre_redirect/libre_redirect.c ./plugins/libre_redirect/str_replace_start.c 
READABILITY=./plugins/readability/readability.c 
STYLE=./plugins/style/style.c

STAND_IN=./plugins/stand_in/stand_in.c # gives function definitions for the above, which do nothing

PLUGS=$(LIBRE_REDIRECT) $(READABILITY) $(STYLE)
# PLUGS=$(STAND_IN)
# Note that if you want some plugins but not others,
# You should edit the stand_in.c file

# Optional adblocking
# depends on https://github.com/jun7/wyebadblock
ADBLOCK='-L/usr/lib/wyebrowser/adblock.so'

# CONFIG
CONFIG=config.h
# cp -f config.def.h config.h

build: $(SRC) $(PLUGS) $(CONFIG)
	$(CC) $(DEBUG) $(INCS) $(PLUGS) $(SRC) -o rose $(LIBS) $(ADBLOCK)

install: rose
	cp -f rose /usr/bin
	mkdir -p /usr/share/themes/rose
	cp style.css /usr/share/themes/rose/
	cp rose-mklink /usr/bin

