#!/bin/ksh

echo "KORN timing"
echo "Profile case 1: small files"
(time (sort -u small | tr 0-9 a-j > c) 2>>d) | cat -

echo "Profile case 2: small files"
(time (sort -u medium | tr 0-9 a-j > c) 2>>d) | cat -

echo "Profile case 3: small files"
(time (sort -u big | tr 0-9 a-j > c) 2>>d) | cat -
