## Rosebud

Personal tweaks for [rose](https://github.com/mini-rose/rose), a minimal browser based on webkit2gtk

#### Features

- tabs, cookies, caching
- minimal ui, autohiding elements
- ~400L code base (edit: no longer)
- custom gtk and websites css
- hackable without any knowledge
- builtin rose-mklink script for in-shell static links
- A few quality of life improvements.

### 👐 Contribute 
This is my personal version. Contribute upstream to [github.com/mini-rose/rose](https://github.com/mini-rose/) instead.

### To do

- [ ] Add "open in new window" functionality. <https://stackoverflow.com/questions/40180757/webkit2gtk-get-new-window-link>
  - Useful for opening links in new tab when clicking on them and selecting that option
  - And for actually opening links with the href newtab option.
  - Links: <https://docs.gtk.org/gobject/func.signal_connect.html>, <https://webkitgtk.org/reference/webkit2gtk/2.37.90/signal.AutomationSession.create-web-view.html>, <https://webkitgtk.org/reference/webkit2gtk/2.26.0/WebKitWebView.html#WebKitWebView-create>
- [ ] Find out what each of the css elements refers to.
- [ ] Use something other than Whatsapp as an example syslink.

Done: 
- [x] String substitution on uri in order to redirect to better frontends.
- [x] Present "standard" browser keybindings as an alternative.
- [x] Fix zoom in new tab
- [x] Reader mode
- [x] Add reader mode to config.def.
- [x] Make tab bar slightly prettier.

### Known bugs

- [ ] Doesn't work with a Spanish keyboard layout, for some reason. 
