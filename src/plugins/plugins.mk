## Shared
COMMON_CODE=./src/plugins/strings/strings.c

## Plugins
CUSTOM_STYLES=./src/plugins/style/style.c
SHORTCUTS=./src/plugins/shortcuts/shortcuts.c
READABILITY=./src/plugins/readability/readability.c 
LIBRE_REDIRECT=./src/plugins/libre_redirect/libre_redirect.c

STAND_IN=./src/plugins/stand_in/stand_in.c # gives function definitions for the above, which do nothing

PLUGINS=$(COMMON_CODE) $(CUSTOM_STYLES) $(SHORTCUTS) $(READABILITY) $(LIBRE_REDIRECT) 
# PLUGINS=$(STAND_IN)


