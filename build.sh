#!/bin/bash
clang -o activate_linux -lX11 -lXfixes -lcairo -I /usr/include/cairo activate_linux.c
