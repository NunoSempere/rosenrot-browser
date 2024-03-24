# Rosenrot

This branch contains a bare-bones version of rosenrot, a webkit and gtk based browser

- similar in many ways to the [original version of rose](https://github.com/mini-rose/rose-browser/tree/60173b6f5b562861b11dea17e3869ad6c3462bbb/src).
  - But with the up to date libwebkit2gtk-4.1!
- compilable with [tcc](https://bellard.org/tcc/)! (also with gcc/clang)
- *One c file*, 390 lines (~356 excluding comments)
- Still some minimal niceties: zoom, parsing more than one url from the command line, max number of tabs, plenty of shortcuts.
- Missing many quality of life features
- Meant for developers seeking to understand or replicate rosenrot
