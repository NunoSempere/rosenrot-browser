## Shared
COMMON_CODE=./plugins/strings/strings.c

## Plugins
CUSTOM_STYLES=./plugins/style/style.c
SHORTCUTS=./plugins/shortcuts/shortcuts.c
READABILITY=./plugins/readability/readability.c 
LIBRE_REDIRECT=./plugins/libre_redirect/libre_redirect.c

ADBLOCK='-L/usr/lib/wyebrowser/adblock.so' # optional adblocking; depends on https://github.com/jun7/wyebadblock

STAND_IN=./plugins/stand_in/stand_in.c # gives function definitions for the above, which do nothing

PLUGINS=$(COMMON_CODE) $(CUSTOM_STYLES) $(SHORTCUTS) $(READABILITY) $(LIBRE_REDIRECT) 
# PLUGINS=$(STAND_IN)


