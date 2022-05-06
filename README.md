# activate-linux
The "Activate Windows" watermark ported to Linux with Xlib and cairo in C

"Science isn't about WHY. It's about WHY NOT. Why is so much of our science dangerous? Why not marry safe science if you love it so much. In fact, why not invent a special safety door that won't hit you on the butt on the way out, because you are fired." — Cave Johnson (Portal 2)

> Read [ARGS.md](ARGS.md) for information about command line arguments.

# Linux

## Dependencies
This project depends on:
- `libcairo2-dev`
- `libxi-dev`
- `libx11-dev`
- `x11proto-core-dev`
- `x11proto-dev`
- `libxt-dev`
- `libxfixes-dev`
- `libxinerama-dev`

## Building
```
make
./bin/activate-linux
```

> Note that the executable is located in `bin/`

## Installing

### Arch Linux
This project is in the AUR under [activate-linux-git](https://aur.archlinux.org/packages/activate-linux-git).

Install it using your favorite AUR helper.

### OpenSUSE (Open Build Service)
This project is in the OBS under [activate-linux](https://software.opensuse.org//download.html?project=home%3AWoMspace&package=activate-linux).


# MacOS (Experimental)
## Dependencies
Use MacPorts to install the following, then build normally.
- `xorg-server`
- `cairo`
- `xorg-libXinerama`

## Building
```
make
./bin/activate-macos
```

> Note that the executable is located in `bin/`

### Installing
You can use `make install` to install to /usr/bin, and `make uninstall` to remove it

## Wayland
This works out of the box through XWayland, or alternatively you can use [this port of the project by Kljunas2](https://github.com/Kljunas2/activate-linux).

# Example:

![screenshot](screenshot.png)
