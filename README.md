# activate-linux
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

The "Activate Windows" watermark ported to Linux with Xlib and cairo in C

"Science isn't about WHY. It's about WHY NOT. Why is so much of our science dangerous? Why not marry safe science if you love it so much. In fact, why not invent a special safety door that won't hit you on the butt on the way out, because you are fired." — Cave Johnson (Portal 2)

> Read [ARGS.md](ARGS.md) for information about command line arguments.

## Building

```
make
```

> Note that the executable's name depends on the target platform


## Linux

### Dependencies

This project depends on:
- `libcairo2-dev`
- `libxi-dev`
- `libx11-dev`
- `x11proto-core-dev`
- `x11proto-dev`
- `libxt-dev`
- `libxfixes-dev`
- `libxinerama-dev`

### Installing

#### Ubuntu
@eddelbuettel runs a PPA with activate linux in it, so it can be installed with
```
sudo add-apt-repository ppa:edd/misc
sudo apt update
sudo apt install activate-linux
```

#### Arch Linux
This project is in the AUR under [activate-linux-git](https://aur.archlinux.org/packages/activate-linux-git).

Install/uninstall it using your favorite AUR helper.

#### OpenSUSE (Open Build Service)
This project is in the OBS under [activate-linux](https://software.opensuse.org//download.html?project=home%3AWoMspace&package=activate-linux).

### Nix (NixOS)
This repository is a flake. Run it using `nix run "github:MrGlockenspiel/activate-linux"`.

#### Other

You can use `make install` to install and `make uninstall` to remove it.

## MacOS (Horrific)

### Dependencies

Use MacPorts to install the following, then build normally.

- `xorg-server`
- `cairo`
- `xorg-libXinerama`

### Wayland
This works out of the box through XWayland, or alternatively you can use [this port of the project by Kljunas2](https://github.com/Kljunas2/activate-linux).

## Example:

![screenshot](screenshot.png)
