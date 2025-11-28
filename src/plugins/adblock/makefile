# Rosenrot Adblock Extension Makefile
#
# This builds the adblock web extension as a shared library (.so)
# that WebKit loads into the web process.

CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2 -fPIC
LDFLAGS ?= -shared

# WebKit dependencies
# Note: webkitgtk-web-process-extension-6.0 provides headers for web extensions in webkitgtk 6.0
WEBKIT_EXT_DEPS = webkitgtk-web-process-extension-6.0
WEBKIT_EXT_CFLAGS = $(shell pkg-config --cflags $(WEBKIT_EXT_DEPS) glib-2.0 gio-2.0 2>/dev/null || echo "-I/usr/include/webkitgtk-6.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include")
WEBKIT_EXT_LIBS = $(shell pkg-config --libs $(WEBKIT_EXT_DEPS) glib-2.0 gio-2.0 2>/dev/null || echo "-lwebkit2gtk-6.0 -lglib-2.0 -lgio-2.0")

# Configuration
ADBLOCK_FILTERLIST_PATH ?= /opt/rosenrot/easylist.txt
EXTENSION_DIR ?= /opt/rosenrot/extensions

# Source files
SRCS = adblock_extension.c uri_tester.c
OBJS = $(SRCS:.c=.o)
TARGET = librosenrot-adblock.so

# Build flags
ALL_CFLAGS = $(CFLAGS) $(WEBKIT_EXT_CFLAGS) -DADBLOCK_FILTERLIST_PATH=\"$(ADBLOCK_FILTERLIST_PATH)\"
ALL_LDFLAGS = $(LDFLAGS) $(WEBKIT_EXT_LIBS)

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(ALL_CFLAGS) $(SRCS) -o $@ $(ALL_LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJS)

install: $(TARGET)
	install -d $(DESTDIR)$(EXTENSION_DIR)
	install -m 755 $(TARGET) $(DESTDIR)$(EXTENSION_DIR)/

uninstall:
	rm -f $(DESTDIR)$(EXTENSION_DIR)/$(TARGET)

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: all
