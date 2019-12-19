#!/bin/sh
dir=`dirname $0`
dir=`(cd $dir; pwd -P)`
cd $dir
time ./puzzler -r4 >/tmp/puzzler.$$ 2>&1
