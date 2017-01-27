#!/bin/bash
#==============================================================================
# Title         : move2src-inc.sh
# Description   : Move files to src or inc directory in pwd
# Author        : Manuel Del Basso (manuel.delbasso@gmail.com)
# Date          : 2017-01-26
# Usage         : 
# Notes         : -
# bash version  : GNU bash, version 4.3.42(1)-release (x86_64-pc-linux-gnu)
# Version       : 0.1    
#==============================================================================

#=== Define some fancy fonts ==================================================
#==============================================================================
BD=$(tput bold)
BDOFF=$(tput sgr0)
GY='\e[38;5;245m'
GN='\e[38;5;76m'
RD='\e[38;5;9m' 
BU='\e[38;5;38m'
COLOFF='\e[0m'

#==============================================================================
# Check for integer number argument.
# [[ $(_isNumber 5) ]] && echo "is number"
#==============================================================================
_isNumber() {
   [[ ! -z "${1##*[!0-9]*}" ]] && echo 1;
}

#==============================================================================
# Test whether the filename has a known "double dot" extension. 
# True: *.tar.7z *.tar.gz *.tar.lzma *.tar.xz
# [[ $(_hasDdArch "file name.tar.gz") ]] && echo "is archive"
#==============================================================================
_hasDdArch() {
   [[ "$1" == $(echo "$1" | grep -oE '.*\.(tar|gz|zip)(\.[7bglx]z.?+)') ]] && echo 1;
}

#==============================================================================
# Return input argument file extension 
#==============================================================================
_getExtension() {
   if [[ ! $(_hasDdArch "$1") ]]; then
      echo "$1" | sed -r 's/(.*)\.([-0-9a-zA-Z]+)/.\2/g' 
   else
      echo "$1" | sed -r 's/(.*)\.([-0-9a-zA-Z]*)\.([-0-9a-zA-Z]*)/.\2.\3/g' 
   fi
}

#=== END OF TEMPLATE HEAD =====================================================
#==============================================================================

RE_SRC='.*\.c\(pp\)?'
RE_INC='.*\.h'

tree -L 2

if [[ ! -z "$(find . -maxdepth 1 -regex $RE_SRC)" ]]; then
    [[ ! -d "src" ]] && { echo "No .src/ directory found, mkdir src ... done!"; mkdir --parents src; }
    find . -maxdepth 1 -regex $RE_SRC -exec mv {} src/ \;
fi    

if [[ ! -z "$(find . -maxdepth 1 -regex $RE_INC)" ]]; then
    [[ ! -d "inc" ]] && { echo "No .inc/ directory found, mkdir inc ... done!"; mkdir --parents inc; }
    find . -maxdepth 1 -regex $RE_INC -exec mv {} inc/ \;
fi    

tree -L 2

