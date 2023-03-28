# make
# make build
# (sudo) make install
# make format
# make clean
# make uninstall

## C compiler
CC=clang

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

build: $(SRC) $(PLUGS) $(CONFIG)
	$(CC) $(DEBUG) $(INCS) $(PLUGS) $(SRC) -o rose $(LIBS) $(ADBLOCK)

install: rose
	cp -f rose /usr/bin
	mkdir -p /usr/share/themes/rose
	cp style.css /usr/share/themes/rose/
	cp rose-mklink /usr/bin

format: $(SRC) $(PLUGS)
	$(FORMATTER) $(SRC) $(PLUGS)


