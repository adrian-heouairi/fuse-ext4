#!/bin/bash

set -o pipefail

script_path=$(realpath -- "$0")

debug-handler() {
    local line=$1
    command_old=$command
    command=$(sed -- "${line}q;d" "$script_path" | sed -E 's/^[[:blank:]]+//')

    if [ "$command" = 'read; clear' ]; then
        echo -ne "\nPress Enter to continue"
    elif [ "$command_old" != "$command" ]; then
        echo -e "\nJe lance la commande \e[1;34m${command}\e[m"
    fi
}

ls() {
    command ls --color=always "$@"
}

trap 'debug-handler $LINENO' DEBUG

clear
pwd
read; clear
make clean
read; clear
ls *.c *.h
make
read; clear
mkdir -pv mntpt
./fe4 mntpt &
read; clear
mount | grep mntpt
read; clear
cd mntpt # Nous resterons dans ce dossier pour le reste de la présentation
pwd
ls -Rla .
read; clear
#realpath .
#realpath ..
#read; clear
echo abc > a
ls -Rla .
read; clear
cat a
read; clear
mkdir -v x
ls -Rla .
read; clear
#ls -la x/..
#read; clear
#echo def > x/b
#ls -Rla .
#read; clear
mv -v a x/a
ls -Rla .
read; clear
#stat x/a
#read; clear
rmdir -v x # Fails
ls -Rla .
read; clear
#touch c
#ls -la .
#read; clear

rm -rvf x
ls -Rla .
read; clear

#rm -v x/a x/b
#ls -Rla .
#read; clear
#rmdir -v x
#ls -Rla .
#read; clear

#mkdir -pv y/z
#ls -Rla .
#read; clear
#rm -rvf y
#ls -Rla .
#read; clear
