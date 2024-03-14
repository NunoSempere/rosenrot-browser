# C compiler
CC=gcc # other options: tcc, clang, zig cc 
WARNINGS=-Wall
DEBUG=#'-g'
OPTIMIZED_SOME=-O3 
OPTIMIZED_MORE=-Ofast -march=native -funit-at-a-time -flto # binary will not be compatible with other computers, but may be much faster
# COMPILETIME_DEPRECATION_WARNINGS=#-DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED # turns out that webkit2gtk-4.1 is using some deprecated stuff, lol

# Dependencies
DEPS='webkit2gtk-4.1'
INCS=`pkg-config --cflags ${DEPS}`
LIBS=`pkg-config --libs ${DEPS}`

# Code
SRC=rosenrot.c
CONFIG=config.h

## Plugins
LIBRE_REDIRECT=./plugins/libre_redirect/libre_redirect.c ./plugins/libre_redirect/str_replace_start.c ./plugins/libre_redirect/str_init.c 
CUSTOM_STYLES=./plugins/style/style.c
READABILITY=./plugins/readability/readability.c 
SHORTCUTS=./plugins/shortcuts/shortcuts.c
STAND_IN=./plugins/stand_in/stand_in.c # gives function definitions for the above, which do nothing
PLUGINS=$(LIBRE_REDIRECT) $(READABILITY) $(CUSTOM_STYLES) $(SHORTCUTS)
ADBLOCK='-L/usr/lib/wyebrowser/adblock.so' ## optional adblocking; depends on https://github.com/jun7/wyebadblock

## Formatter
STYLE_BLUEPRINT=webkit
FORMATTER=clang-format -i -style=$(STYLE_BLUEPRINT)

# Change hardcoded paths when building
## Data dirs
USER=`whoami`
DEFAULT_DATA_DIR=/home/nuno/.cache/rosenrot
USER_DATA_DIR=/home/$(USER)/.cache/rosenrot
## Startup image dir
DEFAULT_DIR=/home/nuno/Documents/workspace/rosenrot
CURRENT_DIR=`pwd`

build: $(SRC) $(PLUGINS) $(CONFIG)
	# Recompute constants
	cd plugins/readability/ && sh recompute_READABILITY_N.sh
	cd plugins/style && sh recompute_STYLE_N.sh 
	# Make cache
	mkdir -p $(USER_DATA_DIR)
	# Hardcode cache path
	find $(CURRENT_DIR) -type f -not -path "*.git*" -not -path "*makefile*" -exec \
		sed -i "s|$(DEFAULT_DATA_DIR)|$(USER_DATA_DIR)|g" {} +
	# Hardcode git repository path
	find $(CURRENT_DIR) -type f -not -path "*.git*" -not -path "*makefile*" -exec \
		sed -i "s|$(DEFAULT_DIR)|$(CURRENT_DIR)|g" {} +
	# Compile rosenrot
	GIO_MODULE_DIR=/usr/lib/x86_64-linux-gnu/gio/modules/
	$(CC) $(WARNINGS) $(OPTIMIZED_MORE) $(DEBUG) $(INCS) $(PLUGINS) $(SRC) -o rosenrot $(LIBS) $(ADBLOCK)

install: rosenrot
	GIO_MODULE_DIR=/usr/lib/x86_64-linux-gnu/gio/modules/
	cp -f rosenrot /usr/bin
	mkdir -p /usr/share/themes/rosenrot
	cp style.css /usr/share/themes/rosenrot/
	cp rosenrot-mklink /usr/bin
	sudo mkdir -p /usr/bin/rosenrot-browser
	sudo cp rosenrot /usr/bin/rosenrot-browser/twitter # custom twitter tweaks

uninstall: 
	rm -r /usr/share/themes/rosenrot
	rm /usr/bin/rosenrot
	rm /usr/bin/rosenrot-mklink
	rm $(DATA_DIR)

clean:
	rm rosenrot
	rm $(DATA_DIR)

format: $(SRC) $(PLUGINS)
	$(FORMATTER) $(SRC) $(PLUGINS) $(rosenrot.h)

lint: 
	clang-tidy $(SRC) $(PLUGINS) -- -Wall -O3    `pkg-config --cflags 'webkit2gtk-4.1'` -o rosenrot `pkg-config --libs 'webkit2gtk-4.1'`

## A few more commands:

fast: $(SRC) $(PLUGINS) $(CONFIG)
	rm -f *.gcda
	GIO_MODULE_DIR=/usr/lib/x86_64-linux-gnu/gio/modules/
	$(CC) $(WARNINGS) $(OPTIMIZED_MORE) -fprofile-generate $(INCS) $(PLUGINS) $(SRC) -o rosenrot $(LIBS) $(ADBLOCK)
	@echo "Now use the browser for a while to gather some profiling data"
	sleep 2
	./rosenrot
	$(CC) $(WARNINGS) $(OPTIMIZED_MORE) -fprofile-use $(INCS) $(PLUGINS) $(SRC) -o rosenrot $(LIBS) $(ADBLOCK)
	rm -f *.gcda

inspect: build
	GTK_DEBUG=interactive ./rosenrot

diagnose_deprecations:
	G_ENABLE_DIAGNOSTIC=1 ./rosenrot

view-gtk3-version:
	dpkg -l libgtk-3-0
