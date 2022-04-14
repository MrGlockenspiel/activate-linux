# activate-linux
The "Activate Windows" watermark ported to Linux with Xlib and cairo in C

"Science isn't about WHY. It's about WHY NOT. Why is so much of our science dangerous? Why not marry safe science if you love it so much. In fact, why not invent a special safety door that won't hit you on the butt on the way out, because you are fired." — Cave Johnson (Portal 2)


#### Read ARGS.md for information about command line arguments.

## Dependencies (Linux)
This project depends on:
- `libcairo2-dev`
- `libxi-dev`
- `libx11-dev`
- `x11proto-core-dev`
- `x11proto-dev`
- `libxt-dev`
- `libxfiles-dev`
- `libxinerama`

## Building
```
make
./bin/activate_linux
```

## Arch Linux
This project is in the AUR under "activate-linux-git".
[Link To AUR page](https://aur.archlinux.org/packages/activate-linux-git).

Use your AUR helper to install.

## OpenSUSE (Open Build Service)
This project is in the OBS under "activate-linux".
[Link to OBS page](https://software.opensuse.org//download.html?project=home%3AWoMspace&package=activate-linux)

## Dependencies (MacOS) (Experimental)
Use MacPorts to install the following, then build normally
- `xorg-server`
- `cairo`
- `xorg-libXinerama`

## Building
```
make
./bin/activate_macos
```

#### Note that the executable is located in bin/


## Example Image:

![screenshot](screenshot.png)
