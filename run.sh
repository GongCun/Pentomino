#!/bin/sh
dir=`dirname $0`
dir=`(cd $dir; pwd -P)`
cd $dir
./puzzler -r1 -o /proc/self/fd/2
