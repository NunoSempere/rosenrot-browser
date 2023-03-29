# make
# make build
# (sudo) make install
# make format
# make clean
# make uninstall

## C compiler
CC=gcc

## Main file
SRC=rose.c

## Dependencies
DEPS='webkit2gtk-4.0'
DEBUG= #'-g'

INCS=`pkg-config --cflags ${DEPS}`
LIBS=`pkg-config --libs ${DEPS}`

## Optional adblocking
## depends on https://github.com/jun7/wyebadblock
ADBLOCK='-L/usr/lib/wyebrowser/adblock.so'

## Plugins
LIBRE_REDIRECT=./plugins/libre_redirect/libre_redirect.c ./plugins/libre_redirect/str_replace_start.c 
READABILITY=./plugins/readability/readability.c 
CUSTOM_STYLES=./plugins/style/style.c

STAND_IN=./plugins/stand_in/stand_in.c # gives function definitions for the above, which do nothing

PLUGS=$(LIBRE_REDIRECT) $(READABILITY) $(CUSTOM_STYLES)
# PLUGS=$(STAND_IN)
# Note that if you want some plugins but not others,
# You should edit the stand_in.c file

# CONFIG
CONFIG=config.h
# cp -f config.def.h config.h

## Formatter
STYLE_BLUEPRINT=webkit
FORMATTER=clang-format -i -style=$(STYLE_BLUEPRINT)
## Commands

## Hardcoded paths
## Cache
USER=`whoami`
DEFAULT_CACHE_DIR=/home/loki/.cache/rose
CURRENT_CACHE_DIR=/home/$(USER)/.cache/rose
## dir
DEFAULT_DIR='/home/loki/Documents/core/software/fresh/C/rose-browser/rosenrot'
CURRENT_DIR=`pwd`

build: $(SRC) $(PLUGS) $(CONFIG)
	# Make cache
	mkdir -p $(CURRENT_CACHE_DIR)
	# Hardcode cache path
	find $(CURRENT_DIR) -type f -not -path "*.git*" -not -path "*makefile*" -exec \
		sed -i "s|$(DEFAULT_CACHE_DIR)|$(CURRENT_CACHE_DIR)|g" {} +
	# Hardcode git repository path
	find $(CURRENT_DIR) -type f -not -path "*.git*" -not -path "*makefile*" -exec \
		sed -i "s|$(DEFAULT_DIR)|$(CURRENT_DIR)|g" {} +
	# Compile rosenrot
	$(CC) $(DEBUG) $(INCS) $(PLUGS) $(SRC) -o rose $(LIBS) $(ADBLOCK)

install: rose
	cp -f rose /usr/bin
	mkdir -p /usr/share/themes/rose
	cp style.css /usr/share/themes/rose/
	cp rose-mklink /usr/bin

uninstall: 
	rm -r /usr/share/themes/rose
	rm /usr/bin/rose
	rm /usr/bin/rose-mklink
	rm $(CACHE_DIR)

clean:
	rm rose
	rm $(CACHE_DIR)

format: $(SRC) $(PLUGS)
	$(FORMATTER) $(SRC) $(PLUGS)

