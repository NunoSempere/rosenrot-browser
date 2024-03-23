# C compiler
CC=gcc # alternatives: tcc, clang, zig cc 
WARNINGS=-Wall
OPTIMIZED_SOME=-O3 
OPTIMIZED_MORE=-Ofast -march=native -funit-at-a-time -flto # binary will not be compatible with other computers, but may be much faster
DEBUG= # -g

# Dependencies
DEPS='webkit2gtk-4.1'
INCS=`pkg-config --cflags ${DEPS}`
LIBS=`pkg-config --libs ${DEPS}`

# Code
SRC=rosenrot.c
CONFIG=config.h

# Plugins
include plugins/plugins.mk
# PLUGINS=./plugins/stand_in/stand_in.c

## Formatter
STYLE_BLUEPRINT="{BasedOnStyle: webkit, AllowShortIfStatementsOnASingleLine: true}" 
FORMATTER=clang-format -i -style=$(STYLE_BLUEPRINT)

# Runtime files 
MAINTAINER_CACHE_DIR=/home/nuno/.cache/rosenrot
USER_CACHE_DIR=/home/`whoami`/.cache/rosenrot
RUNTIME_FILES_DIR=/opt/rosenrot/

build: $(SRC) $(PLUGINS) $(CONFIG) constants user_cache
	$(CC) $(WARNINGS) $(OPTIMIZED_MORE) $(DEBUG) $(INCS) $(PLUGINS) $(SRC) -o rosenrot $(LIBS) $(ADBLOCK)

constants:
	@echo "# Computing constants"
	cd plugins/readability/ && sh recompute_READABILITY_N.sh
	cd plugins/style && sh recompute_STYLE_N.sh 
	@echo

user_cache:
	@if [ `id -u` -eq 0 ]; then echo "can't run make user_cache with sudo, because USER_CACHE_DIR would be /home/root/.cache"; return 1; fi
	@echo "# Create user cache"
	mkdir -p $(USER_CACHE_DIR)
	find . -type f -not -path "*.git*" -not -path "*makefile*" -exec \
		sed -i "s|$(MAINTAINER_CACHE_DIR)|$(USER_CACHE_DIR)|g" {} +
	@echo

runtime_files:
	sudo mkdir -p /opt/rosenrot/
	sudo cp style.css /opt/rosenrot/
	sudo cp -r images/flower-imgs /opt/rosenrot/
	sudo cp plugins/style/style.js /opt/rosenrot/
	sudo cp plugins/readability/readability.js /opt/rosenrot/

install: rosenrot runtime_files
	cp -f rosenrot /usr/bin
	cp rosenrot-mklink /usr/bin

uninstall: 
	rm -r /opt/rosenrot
	rm /usr/bin/rosenrot
	rm /usr/bin/rosenrot-mklink
	rm $(USER_CACHE_DIR)

clean:
	rm rosenrot
	rm $(USER_CACHE_DIR)

format: $(SRC) $(PLUGINS)
	$(FORMATTER) $(SRC) $(PLUGINS) $(rosenrot.h)

lint: 
	clang-tidy $(SRC) $(PLUGINS) -- -Wall -O3 $(INCS) -o rosenrot $(LIBS)
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

inspect: rosenrot 
	GTK_DEBUG=interactive ./rosenrot

diagnose_deprecations: rosenrot
	G_ENABLE_DIAGNOSTIC=1 ./rosenrot

view-gtk3-version:
	dpkg -l libgtk-3-0

twitter:
	sudo mkdir -p /usr/bin/rosenrot-browser
	sudo cp rosenrot /usr/bin/rosenrot-browser/twitter 

# COMPILETIME_DEPRECATION_WARNINGS=#-DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED # turns out that webkit2gtk-4.1 is using some deprecated stuff, lol
