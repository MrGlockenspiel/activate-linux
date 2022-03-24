# activate-linux
The "Activate Windows" watermark ported to Linux and MacOS with Xlib and cairo in C

"Science isn't about WHY. It's about WHY NOT. Why is so much of our science dangerous? Why not marry safe science if you love it so much. In fact, why not invent a special safety door that won't hit you on the butt on the way out, because you are fired."

## Dependencies
This project depends on:
- `libcairo2-dev`
- `libxi-dev`
- `libx11-dev`
- `x11proto-core-dev`
- `x11proto-dev`
- `libxt-dev`
- `libxfiles-dev`

## Building
Linux
```
sh build_linux.sh
./activate_linux
```
MacOS
```
sh build.sh
./activate_macos
```

![screenshot](screenshot.PNG)
