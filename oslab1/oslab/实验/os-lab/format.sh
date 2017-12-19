#!/bin/bash

gcc -o diskmap/format diskmap/map.c

./diskmap/format disk game.bin ans.dat check.txt
#./diskmap/format disk  test.txt ans.dat
