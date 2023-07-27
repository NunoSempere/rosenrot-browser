#!/bin/sh
sed_wrapper()
{
  find ./ -type f -exec sed -i -e "$1" {} \;
} ## e.g., sedr "s/target/replacement/g"

STYLE_N=`wc -c style.js | cut -d " " -f 1`
sed_wrapper "s/^#define STYLE_N .*/#define STYLE_N $STYLE_N + 100/g"

