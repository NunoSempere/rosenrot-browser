# Rosenrot

Rosenrot is a small browser forked from an earlier version of [rose](https://github.com/mini-rose/rose). It has some additional quality of life improvements tailored to my (@NunoSempere) tastes and setup, and detailed installation instructions for Ubuntu 20.04.

![](https://raw.githubusercontent.com/NunoSempere/rosenrot-browser/master/images/7-hello-world-search.png)

![](https://raw.githubusercontent.com/NunoSempere/rosenrot-browser/master/images/6-hello-world.png)

### Installation and usage

You can see detailed instructions [here](./user-scripts/ubuntu-20.04/install-with-dependencies.sh), for Ubuntu 20.04 in particular—though they should generalize easily to other distributions. Or a video installing rosenrot in a fresh Ubuntu 20.04 virtual machine [here](https://video.nunosempere.com/w/t3oAvJLPHTSAMViQ6zbwTV).

The general steps are to install dependencies, and then

```
make build
make install # or sudo make install
rose
```

You can also collect some profiling info, and then use that to get a perhaps faster version:

```
make fast ## will ask you to use the browser for a bit
make install
rose
```

You can also create a rose.desktop file so that it will show up in your desktop environment. You can see this documented [here](./user-scripts/ubuntu-20.04/install-with-dependencies.sh).

## Features

- Tabs, cookies, caching
- Minimal ui, autohiding elements
- ~454L core code (the rose.c file)
- Customize appearance of the browser through css
- Built-in rose-mklink script for in-shell static links
- Optional adblocking through [wyebadblock](https://github.com/jun7/wyebadblock)
- Plugin system, seeded with:
  - Libre redirect: Redirect annoying websites to open source frontends
  - Readability: Strip webpages of unnecessary elements for ease of reading with a custom shortcut
  - Custom style: Override the css of predetermined websites
  - Max number of tabs (by default 8), configurable.
  - Stand in plugin: Mimick function definitions which do nothing for the above plugins so that they can be quickly removed

You can see some screenshots in the [images](./images) folder.

## Comparisons 

### Relationship with [rose](https://github.com/mini-rose/rose)

- Rose is a minimal browser based on webkit2gtk. Previously, it described itself as aiming to be a "basement for creating your own browser using [the] gtk and webkit libraries". 
- Rosenrot is my (@NunoSempere's) fork from rose. It has accumulated quality of life features/cruft that I like, like a "readability" plugin that simplifies annoying websites like [Matt Levine's Money Stuff newsletter](https://www.bloomberg.com/opinion/articles/2022-10-18/matt-levine-s-money-stuff-credit-suisse-was-a-reverse-meme-stock). It also incorporates ad-blocking.
- Rosenrot is also a song by the German hardcore rock band [Rammstein](https://www.youtube.com/watch?v=af59U2BRRAU).

### Comparison with [surf](https://git.suckless.org/surf/file/surf.c.html)

- Surf is another browser based on GTK/Webkit, from the suckless community. 
- It is more complex: surf.c has [2170](https://git.suckless.org/surf/file/surf.c.html) lines, vs rose.c's [454](https://git.nunosempere.com/open.source/rosenrot/src/branch/master/rose.c)
- I find its code messier and harder to understand
- Conversely, surf has significantly more configuration options, and digs deeper into webkit internals.
- Anecdotically, surf feels slower, though I haven't tested this rigorously.
- surf has a larger community, with patches and modifications
- surf is more opinionated, but also less amateurish. For instance, rosenrot 
- My recommendation would be to use rose, and if you find some feature missing, either look how surf does it and import it to rose, or move to surf.

## Folk wisdom

Of general interest:

- I just found out that you can inspect a GTK application with the GTK explorer if you set a certain command-line variable. Try this with `make inspect`.
- Static variables keep their value between invocations.
- By default the searchbar is pretty gigantic. I've made this so because I'm a bit myopic, but also work with my laptop in a laptop stand. Anyways, if you are a more normal person you can change this in the style.css.
- The style.css usage isn't updated until installation. This is because by default rose uses the theme located in /usr/share/themes/rose/style.css, and that file isn't updated until make install.

The "architecture" of the application looks as follows:

![](https://raw.githubusercontent.com/NunoSempere/rosenrot-browser/master/images/0-architecture.png)

Specific to my own system:

- [ ] At some point, I tried to install libsoup-3 and borked some unknown installation option/paths. So now I need to run rose with `GIO_MODULE_DIR=/usr/lib/x86_64-linux-gnu/gio/modules/ /bin/rose` (or put `export GIO_MODULE_DIR=/usr/lib/x86_64-linux-gnu/gio/modules/` in my .bashrc). This won't affect new users though, just double checked on a fresh machine.
