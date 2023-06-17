## Rosenrot

Rosenrot is a small browser forked from an earlier version of [rose](https://github.com/mini-rose/rose). It has some additional quality of life improvements tailored to my (@NunoSempere) tastes and setup, and detailed installation instructions for Ubuntu 20.04.

![](https://raw.githubusercontent.com/NunoSempere/rosenrot-browser/master/screenshots/7-hello-world-search.png)

![](https://raw.githubusercontent.com/NunoSempere/rosenrot-browser/master/screenshots/6-hello-world.png)

### Installation

You can see detailed instructions [here](./user-scripts/ubuntu-20.04/install-with-dependencies.sh), for Ubuntu 20.04 in particular—though they should generalize easily to other distributions. Or a video installing rosenrot in a fresh Ubuntu 20.04 virtual machine [here](https://video.nunosempere.com/w/t3oAvJLPHTSAMViQ6zbwTV).

The general steps are to install dependencies, and then

```
make build
make install # or sudo make install
```

### Usage

After building:

```
./rose
```

After installing:

```
rose
```

You can also create a rose.desktop file so that it will show up in your desktop environment. You can see this documented [here](./user-scripts/ubuntu-20.04/install-with-dependencies.sh).

### Features

- Tabs, cookies, caching
- Minimal ui, autohiding elements
- ~464L core code (the rose.c file)
- Customize appearance of the browser through css
- Built-in rose-mklink script for in-shell static links
- Optional adblocking through [wyebadblock](https://github.com/jun7/wyebadblock)
- Max number of tabs (by default 8), configurable.
- Plugin system, seeded with:
  - Libre redirect: Redirect annoying websites to open source frontends
  - Readability: Strip webpages of unnecessary elements for ease of reading with a custom shortcut
  - Custom style: Override the css of predetermined websites
  - Stand in plugin: Mimick function definitions which do nothing for the above plugins so that they can be quickly removed

You can see some screenshots in the [screenshots](./screenshots) folder.

## Relationship with [rose](https://github.com/mini-rose/rose)

- Rose is a minimal browser based on webkit2gtk. Previously, it described itself as aiming to be a "basement for creating your own browser using [the] gtk and webkit libraries". 
- Rosenrot is my (@NunoSempere's) fork from rose. It has accumulated quality of life features/cruft that I like, like a "readability" plugin that simplifies annoying websites like [Matt Levine's Money Stuff newsletter](https://www.bloomberg.com/opinion/articles/2022-10-18/matt-levine-s-money-stuff-credit-suisse-was-a-reverse-meme-stock). It also incorporates ad-blocking.
- Rosenrot is also a song by the German hardcore rock band [Rammstein](https://www.youtube.com/watch?v=af59U2BRRAU).

### Contribute

- Contribute upstream to [github.com/mini-rose/rose](https://github.com/mini-rose/) for core functionality changes.
- Contribute here for quality of life improvements, by sending a pull request on Github.

### Cool things

I just found out that you can inspect a GTK application with the GTK explorer if you set a certain command-line variable. Try this with `make inspect`.

### Known bugs/gotchas.

- [ ] Doesn't work with when Spanish is selected as the language, for some reason. 
- [ ] At some point, I tried to install libsoup-3 and borked some unknown installation option/paths. So now I need to run rose with `GIO_MODULE_DIR=/usr/lib/x86_64-linux-gnu/gio/modules/ /bin/rose` (or put `export GIO_MODULE_DIR=/usr/lib/x86_64-linux-gnu/gio/modules/` in my .bashrc). This won't affect new users though, just double checked on a fresh machine.
- [ ] By default the searchbar is pretty gigantic. I've made this so because I'm a bit myopic, but also work with my laptop in a laptop stand. Anyways, if you are a more normal person you can change this in the style.css.
- [ ] The style.css usage isn't updated until installation. This is because by default rose uses the theme located in /usr/share/themes/rose/style.css, and that file isn't updated until make install.

### To do

#### Quality of life:

- [ ] Add css for js alerts
  - [x] Add custom alert whose css can be customized
  - [ ] Debug problems, e.g., this version is non-blocking.
- [ ] Figure out better way to have plugins
- [ ] Double check newtab/next-tab behavior
- [ ] Document creating new applications, e.g., as in [Asana for Linux](https://git.nunosempere.com/NunoSempere/asana-for-linux)
- [ ] Add list of similar projects: <https://github.com/qutebrowser/qutebrowser#similar-projects>

#### Maintenance 

- [ ] Set [`webkit_web_context_set_sandbox_enabled`](<https://webkitgtk.org/reference/webkit2gtk/2.36.8/WebKitWebContext.html#webkit-web-context-set-sandbox-enabled>), as recommended [here](<https://blogs.gnome.org/mcatanzaro/2022/11/04/stop-using-qtwebkit/>)
- [ ] Use something other than Whatsapp as an example syslink.
- [ ] Fix bug about distorted audio. Maybe related to [this pipewire issue](<https://gitlab.freedesktop.org/pipewire/pipewire/-/issues/1547>)?
- [ ] Upgrade to GTK-4 / Webkitgtk 6.0? Will take a fair amount of time, since these are not available on Ubuntu 20.04.

#### Previously done

- [x] Add a shortcut for hiding the search tab. => Already exists: Ctrl+K
- [x] Find out what each of the css elements refers to. => done, see make inspect
- [x] Figure out if downloading files is doable. => it is
- [x] Look at using relative rather than absolute paths for configuration. => now makefile is a bit smarter
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

