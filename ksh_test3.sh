#!/bin/ksh

echo "KORN timing"
(time (sort -u small | tr 0-9 a-j > c) 2>>d) | cat -
