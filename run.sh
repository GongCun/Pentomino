#!/bin/sh
dir=`dirname $0`
dir=`(cd $dir; pwd -P)`
cd $dir
./sudoku >/tmp/sudoku.$$
