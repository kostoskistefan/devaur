#!/bin/bash

if [ "$EUID" -ne 0 ]; then 
    echo "Please run as root"
    exit
fi

SCRIPT_PATH=$(dirname $(realpath $0))

installDependencies() {
    sudo pacman -S make gcc libgit2 --needed
}

makePackage() {
    make -C $SCRIPT_PATH
}

copyPackage() {
    sudo cp $SCRIPT_PATH/build/devaur /usr/bin/devaur
}

makeSymlink() {
    ln -sf /usr/bin/devaur /usr/bin/da
}

installDependencies
makePackage
copyPackage
makeSymlink
