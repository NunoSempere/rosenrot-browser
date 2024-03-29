#!/bin/sh
sed_wrapper()
{
  find ./ -type f -exec sed -i -e "$1" {} \;
} ## e.g., sedr "s/target/replacement/g"

READABILITY_N=`wc -c readability.js | cut -d " " -f 1`
sed_wrapper "s/^#define READABILITY_N .*/#define READABILITY_N $READABILITY_N + 1000/g"

