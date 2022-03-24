#!/bin/bash
clang -o activate_macos -lX11 -lXfixes -lcairo -I /usr/include/cairo activate_macos.c
