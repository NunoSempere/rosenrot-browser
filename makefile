# C compiler
# CC=gcc # alternatives: tcc, clang, zig cc 
CC=tcc

# Dependencies
DEPS='webkit2gtk-4.1'
INCS=`pkg-config --cflags ${DEPS}`
LIBS=`pkg-config --libs ${DEPS}`

# Code
SRC=rosenrot.c

## Runtime files
MAINTAINER_CACHE_DIR=/home/nuno/.cache/rosenrot
USER_CACHE_DIR=/home/`whoami`/.cache/rosenrot

build: $(SRC)  user_cache
	$(CC) $(INCS) $(SRC) -o rosenrot $(LIBS) $(ADBLOCK)

user_cache:
	@if [ `id -u` -eq 0 ]; then echo "can't run make user_cache with sudo, because USER_CACHE_DIR would be /home/root/.cache"; return 1; fi
	@echo "# Create user cache"
	mkdir -p $(USER_CACHE_DIR)
	find . -type f -not -path "*.git*" -not -path "*makefile*" -exec \
		sed -i "s|$(MAINTAINER_CACHE_DIR)|$(USER_CACHE_DIR)|g" {} +
	@echo

install: rosenrot
	cp -f rosenrot /usr/bin

depsdebian:
	sudo apt install tcc make
	sudo apt install libwebkit2gtk-4.1-dev

## Additional niceties

### Formatter
STYLE_BLUEPRINT="{BasedOnStyle: webkit, AllowShortIfStatementsOnASingleLine: true, IndentCaseLabels: true, AllowShortEnumsOnASingleLine: true}" 
FORMATTER=clang-format -i -style=$(STYLE_BLUEPRINT)

format: $(SRC) 
	$(FORMATTER) $(SRC) 

lint: 
	clang-tidy $(SRC) -- -Wall $(INCS) -o rosenrot $(LIBS)

### Cleanup functions
uninstall: 
	rm /usr/bin/rosenrot
	rm $(USER_CACHE_DIR)

clean:
	rm rosenrot
	rm $(USER_CACHE_DIR)

