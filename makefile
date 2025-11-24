# C compiler
CC=gcc # alternatives: tcc, clang, zig cc 
WARNINGS=-Wall -Wextra -Wshadow -Wformat=2 -Wno-unused-parameter
WARNINGS_PEDANTIC=-Wpedantic 
OPTIMIZED_SOME=-O3 
OPTIMIZED_MORE=-Ofast -march=native -funit-at-a-time -flto # binary will not be compatible with other computers, but may be much faster
DEBUG=-g
STD=-std=c99 # maybe consider moving to c11 and using safer string handling
SECURITY=-fstack-protector-strong # detect stack buffer overflows at runtime

# Dependencies for WebkitGTK6/GTK4
SRC=src/rosenrot.c
DEPS='webkitgtk-6.0'
INCS=`pkg-config --cflags ${DEPS}` `pkg-config --cflags gtk4`
LIBS=`pkg-config --libs ${DEPS}` `pkg-config --libs gtk4`
DEPRECATION_FLAGS=-DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED

# User config
CONFIG=src/config.h

# Plugins
include src/plugins/plugins.mk
# PLUGINS=./src/plugins/stand_in/stand_in.c
ADBLOCK='-L/usr/lib/wyebrowser/adblock.so' # optional adblocking; depends on https://github.com/jun7/wyebadblock

## Formatter
STYLE_BLUEPRINT="{BasedOnStyle: webkit, AllowShortIfStatementsOnASingleLine: true, IndentCaseLabels: true, AllowShortEnumsOnASingleLine: true}" 
FORMATTER_C=clang-format -i -style=$(STYLE_BLUEPRINT)
FORMATTER_JS=npx prettier -w 

# Runtime files 
MAINTAINER_CACHE_DIR=/home/nuno/.cache/rosenrot
USER_CACHE_DIR=/home/`whoami`/.cache/rosenrot
RUNTIME_FILES_DIR=/opt/rosenrot/

build: $(SRC) $(PLUGINS) $(CONFIG) constants user_cache
	$(CC) $(STD) $(WARNINGS) $(SECURITY) $(DEPRECATION_FLAGS) $(OPTIMIZED_MORE) $(DEBUG) $(INCS) $(PLUGINS) $(SRC) -o out/rosenrot $(LIBS) $(ADBLOCK)
	@echo

format: $(SRC) $(PLUGINS)
	$(FORMATTER_C) $(SRC) $(PLUGINS) $(CONFIG)
	$(FORMATTER_JS) src/plugins/readability/readability.js
	$(FORMATTER_JS) src/plugins/style/style.js

# Installation

install: out/rosenrot runtime_files
	cp -f out/rosenrot /usr/bin/rosenrot
	@echo

uninstall: 
	rm -rf /opt/rosenrot
	rm -f /usr/bin/rosenrot
	rm -rf $(USER_CACHE_DIR)

clean:
	rm -f out/rosenrot
	rm -rf $(USER_CACHE_DIR)

constants:
	@echo
	@echo "# Computing constants"
	cd src/plugins/readability/ && sh recompute_READABILITY_N.sh
	cd src/plugins/style && sh recompute_STYLE_N.sh 
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
	sudo cp src/styles-gtk/style-gtk4.css /opt/rosenrot/
	sudo touch /opt/rosenrot/uris.txt
	sudo chmod a+rw /opt/rosenrot/uris.txt
	sudo cp -r src/images/flower-imgs /opt/rosenrot/
	sudo cp src/plugins/style/style.js /opt/rosenrot/
	sudo cp src/plugins/readability/readability.js /opt/rosenrot/

# More misc recipes

lint: 
	clang-tidy $(SRC) $(PLUGINS) -- -Wall -O3 $(INCS) -o rosenrot $(LIBS)

fast: $(SRC) $(PLUGINS) $(CONFIG)
	rm -f *.gcda
	GIO_MODULE_DIR=/usr/lib/x86_64-linux-gnu/gio/modules/
	$(CC) $(WARNINGS) $(SECURITY) $(OPTIMIZED_MORE) -fprofile-generate $(INCS) $(PLUGINS) $(SRC) -o out/rosenrot $(LIBS) $(ADBLOCK)
	@echo "Now use the browser for a while to gather some profiling data"
	sleep 2
	./out/rosenrot
	$(CC) $(WARNINGS) $(SECURITY) $(OPTIMIZED_MORE) -fprofile-use $(INCS) $(PLUGINS) $(SRC) -o out/rosenrot $(LIBS) $(ADBLOCK)
	rm -f *.gcda

inspect: out/rosenrot 
	GTK_DEBUG=interactive ./out/rosenrot

diagnose_deprecations: out/rosenrot
	G_ENABLE_DIAGNOSTIC=1 ./out/rosenrot

twitter:
	sudo mkdir -p /usr/bin/rosenrot-browser
	sudo cp out/rosenrot /usr/bin/rosenrot-browser/twitter 
