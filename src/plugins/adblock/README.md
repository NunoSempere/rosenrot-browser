# Rosenrot Adblock Plugin

A WebKit web extension that blocks ads using EasyList filter rules.

## How It Works

This plugin consists of two parts:

1. **Web Extension** (`librosenrot-adblock.so`) - A shared library loaded by WebKit into the web process. It intercepts all HTTP requests and checks them against the filter rules.

2. **URI Tester** (`uri-tester.c`) - The filter matching engine, adapted from Epiphany/GNOME Web browser. It parses EasyList-format filter rules and matches URLs against them.

## Installation

### 1. Download EasyList

```bash
sudo mkdir -p /opt/rosenrot
curl -o /tmp/easylist.txt https://easylist.to/easylist/easylist.txt
sudo mv /tmp/easylist.txt /opt/rosenrot/easylist.txt
```

### 2. Build the Extension

```bash
cd src/plugins/adblock
make
sudo make install
```

This installs `librosenrot-adblock.so` to `/opt/rosenrot/extensions/`.

### 3. Enable in Rosenrot

Set `ADBLOCK_ENABLED` to `true` in `src/config.h` and rebuild rosenrot:

```c
#define ADBLOCK_ENABLED true
```

```bash
make build
```

## Configuration

Configuration is in `src/config.h`:

- `ADBLOCK_ENABLED` - Set to `true` to enable adblocking (default: `false`)
- `ADBLOCK_FILTERLIST_PATH` - Path to easylist.txt (default: `/opt/rosenrot/easylist.txt`)
- `ADBLOCK_EXTENSIONS_DIR` - Directory containing the extension (default: `/opt/rosenrot/extensions`)

## Updating Filter Lists

To update your filter list:

```bash
curl -o /tmp/easylist.txt https://easylist.to/easylist/easylist.txt
sudo mv /tmp/easylist.txt /opt/rosenrot/easylist.txt
```

Restart rosenrot to reload the filters.

## Troubleshooting

### Extension not loading

1. Check that the extension exists:
   ```bash
   ls -la /opt/rosenrot/extensions/librosenrot-adblock.so
   ```

2. Run with debug output:
   ```bash
   G_MESSAGES_DEBUG=all ./out/rosenrot
   ```

### Filters not working

1. Check that easylist.txt exists and is readable:
   ```bash
   head /opt/rosenrot/easylist.txt
   ```

2. Ensure `ADBLOCK_ENABLED` is `true` in config.h

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     ROSENROT PROCESS                         │
│  ┌────────────────────────────────────────────────────────┐ │
│  │                    WebKit Web Process                   │ │
│  │  ┌──────────────────────────────────────────────────┐  │ │
│  │  │         librosenrot-adblock.so (extension)       │  │ │
│  │  │                                                  │  │ │
│  │  │  on_send_request() ──► ephy_uri_tester_test_uri()│  │ │
│  │  │         │                       │                │  │ │
│  │  │         ▼                       ▼                │  │ │
│  │  │  Return TRUE (block)    Check against filters    │  │ │
│  │  │  or FALSE (allow)       loaded from easylist.txt │  │ │
│  │  └──────────────────────────────────────────────────┘  │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

## Credits

- URI tester code adapted from [Epiphany/GNOME Web](https://gitlab.gnome.org/GNOME/epiphany)
- Original wyebadblock by [jun7](https://github.com/jun7/wyebadblock)
- Filter format by [EasyList](https://easylist.to/)

## License

GPL-3.0 (same as Epiphany source code)
