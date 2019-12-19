#!/bin/sh
dir=`dirname $0`
dir=`(cd $dir; pwd -P)`
cd $dir
time ./sudoku -r4 >/tmp/sudoku.$$ 2>&1
