#!/bin/sh

if [ ! -d "./builds" ] 
then
    mkdir "./builds"
fi

clang -o builds/activate_linux -lX11 -lXfixes -lcairo -I /usr/include/cairo src/activate_linux.c
