# C compiler
CC=gcc # alternatives: tcc, clang, zig cc 
WARNINGS=-Wall
OPTIMIZED_SOME=-O3 
OPTIMIZED_MORE=-Ofast -march=native -funit-at-a-time -flto # binary will not be compatible with other computers, but may be much faster
DEBUG=
STD=-std=c99 # maybe consider moving to c11 and using safer string handling

# Dependencies for WebkitGTK4/GTK3
SRC_3=rosenrot3.c
DEPS_3='webkit2gtk-4.1'
INCS_3=`pkg-config --cflags ${DEPS_3}`
LIBS_3=`pkg-config --libs ${DEPS_3}`

# Dependencies for WebkitGTK6/GTK4
SRC_4=rosenrot4.c
DEPS_4='webkitgtk-6.0'
INCS_4=`pkg-config --cflags ${DEPS_4}` `pkg-config --cflags gtk4`
LIBS_4=`pkg-config --libs ${DEPS_4}` `pkg-config --libs gtk4`
# DEPRECATION_FLAGS=-DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED

# Dependencies starting from scratchpad
SRC_4_greenfield=rosenrot4_greenfield_minimal.c
DEPS_4_greenfield='webkitgtk-6.0'
INCS_4_greenfield=`pkg-config --cflags ${DEPS_4_greenfield}` `pkg-config --cflags gtk4`
LIBS_4_greenfield=`pkg-config --libs ${DEPS_4_greenfield}` `pkg-config --libs gtk4`
# DEPRECATION_FLAGS=-DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED

# User config
CONFIG=config.h

# Plugins
include plugins/plugins.mk
# PLUGINS=./plugins/stand_in/stand_in.c
ADBLOCK='-L/usr/lib/wyebrowser/adblock.so' # optional adblocking; depends on https://github.com/jun7/wyebadblock

## Formatter
STYLE_BLUEPRINT="{BasedOnStyle: webkit, AllowShortIfStatementsOnASingleLine: true, IndentCaseLabels: true, AllowShortEnumsOnASingleLine: true}" 
FORMATTER_C=clang-format -i -style=$(STYLE_BLUEPRINT)
FORMATTER_JS=npx prettier -w 

# Runtime files 
MAINTAINER_CACHE_DIR=/home/nuno/.cache/rosenrot
USER_CACHE_DIR=/home/`whoami`/.cache/rosenrot
RUNTIME_FILES_DIR=/opt/rosenrot/

# Start 3 to 4 transition 
# https://docs.gtk.org/gtk4/migrating-3to4.html
# https://github.com/WebKit/WebKit/blob/ed1422596dce5ff012e64a38faf402ac1674fc7e/Source/WebKit/gtk/migrating-to-webkitgtk-6.0.md

build: $(SRC_3) $(PLUGINS) $(CONFIG) constants user_cache
	$(CC) $(STD) $(WARNINGS) $(OPTIMIZED_MORE) $(DEBUG) $(INCS_3) $(PLUGINS) $(SRC_3) -o rosenrot $(LIBS_3) $(ADBLOCK)
	@echo

build4: $(SRC_4) $(PLUGINS) $(CONFIG) constants user_cache
	$(CC) $(STD) $(WARNINGS) $(DEPRECATION_FLAGS) $(OPTIMIZED_MORE) $(DEBUG) $(INCS_4) $(PLUGINS) $(SRC_4) -o rosenrot $(LIBS_4) $(ADBLOCK)
	@echo

build4_greenfield: $(SRC_4_greenfield) $(PLUGINS) $(CONFIG) constants user_cache
	$(CC) $(STD) $(WARNINGS) $(DEPRECATION_FLAGS) $(OPTIMIZED_MORE) $(DEBUG) $(INCS_4_greenfield) $(PLUGINS) $(SRC_4_greenfield) -o rosenrot $(LIBS_4_greenfield) $(ADBLOCK)
	@echo

diagnose:
	G_ENABLE_DIAGNOSTIC=1 ./rosenrot

constants:
	@echo
	@echo "# Computing constants"
	cd plugins/readability/ && sh recompute_READABILITY_N.sh
	cd plugins/style && sh recompute_STYLE_N.sh 
	@echo

user_cache:
	@if [ `id -u` -eq 0 ]; then echo "can't run make user_cache with sudo, because USER_CACHE_DIR would be /home/root/.cache"; return 1; fi
	@echo "# Create user cache"
	mkdir -p $(USER_CACHE_DIR)
	find . -type f -not -path "*.git*" -not -path "*makefile*" \
	  -exec sed -i "s|$(MAINTAINER_CACHE_DIR)|$(USER_CACHE_DIR)|g" {} +
	@echo

runtime_files:
	@echo
	sudo mkdir -p /opt/rosenrot/
	sudo cp style-gtk3.css /opt/rosenrot/style.css
	sudo cp -r images/flower-imgs /opt/rosenrot/
	sudo cp plugins/style/style.js /opt/rosenrot/
	sudo cp plugins/readability/readability.js /opt/rosenrot/

install: rosenrot runtime_files
	cp -f rosenrot /usr/bin
	cp rosenrot-mklink /usr/bin
	@echo

uninstall: 
	rm -r /opt/rosenrot
	rm /usr/bin/rosenrot
	rm /usr/bin/rosenrot-mklink
	rm $(USER_CACHE_DIR)

clean:
	rm rosenrot
	rm $(USER_CACHE_DIR)

format: $(SRC) $(PLUGINS)
	$(FORMATTER_C) $(SRC) $(PLUGINS) $(rosenrot.h)
	$(FORMATTER_C) $(SRC_4) $(PLUGINS) $(rosenrot.h)
	$(FORMATTER_C) $(SRC_4_greenfield) $(PLUGINS) $(rosenrot.h)
	$(FORMATTER_JS) plugins/readability/readability.js
	$(FORMATTER_JS) plugins/style/style.js

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
