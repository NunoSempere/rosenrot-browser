# To do

- [ ] Document creating new applications, e.g., as in [Asana for Linux](https://git.nunosempere.com/NunoSempere/asana-for-linux)
  - [ ] This time, use something other than Whatsapp as an example syslink.
- [ ] Fix bug about distorted audio. Maybe related to [this pipewire issue](<https://gitlab.freedesktop.org/pipewire/pipewire/-/issues/1547>)?
  - See whether it even exists at all
- [ ] Upgrade to GTK-4 / Webkitgtk 6.0? Will take a fair amount of time, since GTK4 redesigns the application model somewhat.
  - Instructions for webkit-6.0 [here](https://github.com/WebKit/WebKit/blob/ed1422596dce5ff012e64a38faf402ac1674fc7e/Source/WebKit/gtk/migrating-to-webkitgtk-6.0.md)
  - Instructions for GTK-4 [here](https://docs.gtk.org/gtk4/migrating-3to4.html)
  - [ ] Prepare for GTK-3 to GTK-4 transition
    - [ ] Understand wtf is going on with signals and events: <https://docs.gtk.org/gtk4/migrating-3to4.html#stop-using-gtkwidget-event-signals>. <https://github.com/mini-rose/rose-browser/blob/288bf060d095c4895946669ae50d14193168b69c/src/window.c#L42>
  - [ ] Remove webkit2gtk-4.1 and download webkit2gtk-6.0
  - [ ] Attempt to compile

# Previously done

- [x] Fix PageUp/PageDown shortcuts.
- ~~[ ] Set [`webkit_web_context_set_sandbox_enabled`](<https://webkitgtk.org/reference/webkit2gtk/2.36.8/WebKitWebContext.html#webkit-web-context-set-sandbox-enabled>), as recommended [here](<https://blogs.gnome.org/mcatanzaro/2022/11/04/stop-using-qtwebkit/>)~~. Irrelevant with upgrade to libsoup3.
- [x] Update to webkit2gtk-4.1
  - [x] Change README and point to last Ubuntu 20.04 commit
- [x] Add list of similar projects: <https://github.com/qutebrowser/qutebrowser#similar-projects>
- [x] Add comparisons against rose & surf
  - [x] Compare against rose
  - [x] Compare against surf
- [x] ~~Doesn't work with when Spanish is selected as the language, for some reason~~ => Previously misdiagnosed. The real issue was that it freezes when interacting with [Espanso](https://espanso.org/) substitutions, which I had set-up automatically on my machine when using words containing an ñ, like my own name, Nuño.
- [x] Add css for js alerts
  - [x] Add custom alert whose css can be customized
  - [ ] ~~Debug problems, e.g., this version is non-blocking.~~ => will leave as is 
- [x] Figure out better way to have plugins => stand_in code seems superfluous
- [x] Double check newtab/next-tab behavior => custom style now loading correctly.
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

