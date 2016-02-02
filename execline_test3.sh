#!/usr/local/cs/execline-2.1.4.5/bin/execlineb

redirfd -w 1 c
redirfd -a 2 d
pipeline {
    sort -u small
}
tr 0-9 a-j
