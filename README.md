## Rosenrot

A small browser forked from [rose](https://github.com/mini-rose/rose). 

- Rose is a minimal browser based on webkit2gtk which aims to be a "basement for creating your own browser using [the] gtk and webkit libraries". 
- Rosenrot is my fork from rose. It has accumulated cruft that I like, like a "readability" plugin that simplifies annoying websites like [Matt Levine's Money Stuff newsletter](https://www.bloomberg.com/opinion/articles/2022-10-18/matt-levine-s-money-stuff-credit-suisse-was-a-reverse-meme-stock).
- Rosenrot is also a song by the German hardcore rock band [Rammstein](https://www.youtube.com/watch?v=af59U2BRRAU).

You can see some screenshots in the [screenshots](./screenshots) folder.

### Features

- tabs, cookies, caching
- minimal ui, autohiding elements
- ~467L core code (the rose.c file)
- custom gtk and websites css
- builtin rose-mklink script for in-shell static links
- A few quality of life improvements.
  - Optional adblocking through [wyebadblock](https://github.com/jun7/wyebadblock)
  - Plugin system:
    - Libre redirect: Redirect annoying websites to open source frontends
    - Readability: Strip webpages of unnecessary elements for ease of reading with a custom shortcut
    - Custom style: Override the css of predetermined websites
    - Stand in plugin: Mimick function definitions which do nothing for the above plugins so that they can be quickly removed

### Installation

Install dependencies, then:

```
make build
make install # or sudo make install
```

You can also see more detailed instructions [here](./user-scripts/ubuntu-20.04/install-with-dependencies.sh), for Ubuntu 20.04 in particular—though they should generalize trivially. Or a video installing rosenrot in a fresh Ubuntu 20.04 virtual machine [here](https://video.nunosempere.com/w/t3oAvJLPHTSAMViQ6zbwTV).

### 👐 Contribute

- Contribute upstream to [github.com/mini-rose/rose](https://github.com/mini-rose/) for core functionality changes.
- Contribute here for quality of life improvements, by sending a pull request on Github.

### To do

- [ ] Look at using relative rather than absolute paths
- [ ] Add css for js alerts
- [ ] Figure out if downloading files is doable.
- [ ] Figure out better way to have plugins
- [ ] Double check newtab/next-tab behavior
- [ ] Find out what each of the css elements refers to.
- [ ] Use something other than Whatsapp as an example syslink.
- [ ] Set [`webkit_web_context_set_sandbox_enabled`](<https://webkitgtk.org/reference/webkit2gtk/2.36.8/WebKitWebContext.html#webkit-web-context-set-sandbox-enabled>), as recommended [here](<https://blogs.gnome.org/mcatanzaro/2022/11/04/stop-using-qtwebkit/>)
- [ ] Fix bug about distorted audio. Maybe related to [this pipewire issue](<https://gitlab.freedesktop.org/pipewire/pipewire/-/issues/1547>)?

Done:

- [x] Streamline installation a bit
  - [x] Substitute paths in makefile
  - [x] Create cache directory automatically
- [x] Add an installation video walkthrough. Done, [here](https://video.nunosempere.com/w/t3oAvJLPHTSAMViQ6zbwTV)
- [x] Document `stand_in.c` better
- [x] Use a makefile.
  - [x] Add clean, uninstall to makefile
- [x] Mask user agent
- [x] Launch with more than one tab from command line
- [x] Figure out merge with upstream
- [x] String substitution on uri in order to redirect to better frontends.
- [x] Present "standard" browser keybindings as an alternative.
- [x] Fix zoom in new tab
- [x] Reader mode
- [x] Add reader mode to config.def.
- [x] Make tab bar slightly prettier.
- [x] Add "open in new window" functionality. 
  - Useful for opening links in new tab when clicking on them and selecting that option
  - And for actually opening links with the href new_tab option.
  - Links: [1](<https://docs.gtk.org/gobject/func.signal_connect.html>), [2](<https://webkitgtk.org/reference/webkit2gtk/2.37.90/signal.AutomationSession.create-web-view.html>), [3](<https://webkitgtk.org/reference/webkit2gtk/2.26.0/WebKitWebView.html#WebKitWebView-create>), [4](<https://stackoverflow.com/questions/40180757/webkit2gtk-get-new-window-link>)

### Known bugs

- [ ] Doesn't work with when Spanish is selected as the language, for some reason. 
- [ ] At some point, I tried to install libsoup-3 and borked some unknown installation option/paths. So now I need to run rose with `GIO_MODULE_DIR=/usr/lib/x86_64-linux-gnu/gio/modules/ /bin/rose` (or put `export GIO_MODULE_DIR=/usr/lib/x86_64-linux-gnu/gio/modules/` in my .bashrc). This won't affect new users though, just double checked on a fresh machine.
