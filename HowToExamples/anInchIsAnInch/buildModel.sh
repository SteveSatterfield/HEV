#! /bin/bash

( \
  savg-cube | savg-scale 9.    .125  .125 | savg-color -r 1 -b 0 -g 0 -a 1 ;\
  savg-cube | savg-scale  .125 6.    .125 | savg-color -r 0 -b 1 -g 0 -a 1 ;\
  savg-cube | savg-scale  .125  .125 3    | savg-color -r 1 -b 0 -g 1 -a 1 ;\
  ) > triad.savg


