#!/bin/ksh

echo "KORN timing"
(time (sort -u big | tr 0-9 a-j > c) 2>>d) | cat -
