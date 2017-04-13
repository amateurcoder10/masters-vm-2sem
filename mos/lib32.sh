#!/bin/bash
objdump -p ./bin/*|
grep NEEDED|
awk '{print $2}'|
sort -u|
xargs dpkg -S|
awk '{print $1}'|
sort -u|
sed 's/:amd64:/:i386:/'|
sort -u|
sed 's/:$//'
