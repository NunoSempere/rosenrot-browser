
# C compiler
CC=gcc # other options: tcc, clang, zig cc 
WARNINGS=-Wall
DEBUG= #'-g'
OPTIMIZED_SOME=-O3 
OPTIMIZED_MORE=-Ofast -march=native -funit-at-a-time -flto # binary will not be compatible with other computers, but may be much faster

# Dependencies
DEPS='webkit2gtk-4.0'
INCS=`pkg-config --cflags ${DEPS}`
LIBS=`pkg-config --libs ${DEPS}`

# Code
SRC=rose.c
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
## Cache
USER=`whoami`
DEFAULT_CACHE_DIR=/home/loki/.cache/rose
CURRENT_CACHE_DIR=/home/$(USER)/.cache/rose
## dir
DEFAULT_DIR=/home/loki/Documents/core/software/fresh/C/rose-browser/rosenrot
CURRENT_DIR=`pwd`

build: $(SRC) $(PLUGINS) $(CONFIG)
	# Recompute constants
	cd plugins/readability/ && sh recompute_READABILITY_N.sh
	cd plugins/style && sh recompute_STYLE_N.sh 
	# Make cache
	mkdir -p $(CURRENT_CACHE_DIR)
	# Hardcode cache path
	find $(CURRENT_DIR) -type f -not -path "*.git*" -not -path "*makefile*" -exec \
		sed -i "s|$(DEFAULT_CACHE_DIR)|$(CURRENT_CACHE_DIR)|g" {} +
	# Hardcode git repository path
	find $(CURRENT_DIR) -type f -not -path "*.git*" -not -path "*makefile*" -exec \
		sed -i "s|$(DEFAULT_DIR)|$(CURRENT_DIR)|g" {} +
	# Compile rosenrot
	GIO_MODULE_DIR=/usr/lib/x86_64-linux-gnu/gio/modules/
	$(CC) $(WARNINGS) $(OPTIMIZED_SOME) $(DEBUG) $(INCS) $(PLUGINS) $(SRC) -o rose $(LIBS) $(ADBLOCK)

fast: $(SRC) $(PLUGINS) $(CONFIG)
	rm -f *.gcda
	GIO_MODULE_DIR=/usr/lib/x86_64-linux-gnu/gio/modules/
	$(CC) $(WARNINGS) $(OPTIMIZED_MORE) -fprofile-generate $(INCS) $(PLUGINS) $(SRC) -o rose $(LIBS) $(ADBLOCK)
	@echo "Now use the browser for a while to gather some profiling data"
	sleep 2
	./rose
	$(CC) $(WARNINGS) $(OPTIMIZED_MORE) -fprofile-use $(INCS) $(PLUGINS) $(SRC) -o rose $(LIBS) $(ADBLOCK)
	rm -f *.gcda

lint: 
	clang-tidy $(SRC) $(PLUGINS) -- -Wall -O3    `pkg-config --cflags 'webkit2gtk-4.0'` -o rose `pkg-config --libs 'webkit2gtk-4.0'`

inspect: build
	GTK_DEBUG=interactive ./rose

install: rose
	GIO_MODULE_DIR=/usr/lib/x86_64-linux-gnu/gio/modules/
	cp -f rose /usr/bin
	mkdir -p /usr/share/themes/rose
	cp style.css /usr/share/themes/rose/
	cp rose-mklink /usr/bin
	sudo cp rose /usr/bin/rose-browser/twitter # custom twitter tweaks

uninstall: 
	rm -r /usr/share/themes/rose
	rm /usr/bin/rose
	rm /usr/bin/rose-mklink
	rm $(CACHE_DIR)

clean:
	rm rose
	rm $(CACHE_DIR)

format: $(SRC) $(PLUGINS)
	$(FORMATTER) $(SRC) $(PLUGINS) $(rose.h)
