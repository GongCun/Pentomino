#!/bin/sh
dir=`dirname $0`
dir=`(cd $dir; pwd -P)`
cd $dir
./sudoku -r4 -o /tmp/sudoku.$$
