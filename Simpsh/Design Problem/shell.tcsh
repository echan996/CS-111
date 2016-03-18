#!/bin/tcsh

echo "Profile case 1: small files"

time sort -u small > small2
time tr 0-9 a-j < small >c


echo "Profile case 2: medium files"

time sort -u medium > medium
time tr 0-9 a-j < medium >c



echo "Profile case 3: big files"

time sort -u big > big2
time tr 0-9 a-j < big2 >c



