#!/bin/sh
ip=`ifconfig eth0 | sed -n 's/^[[:space:]]*inet \(.*\)  netmask.*/\1/p' | tr -d '[[:space:]]'`
touch /data/solution.who.$ip
/puzzler -r4 -o /data/solution.$$.$RANDOM >/dev/null 2>&1
