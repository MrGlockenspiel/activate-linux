# activate-linux
The "Activate Windows" watermark ported to Linux with Xlib and cairo in C

"Science isn't about WHY. It's about WHY NOT. Why is so much of our science dangerous? Why not marry safe science if you love it so much. In fact, why not invent a special safety door that won't hit you on the butt on the way out, because you are fired."


## Dependencies (Linux)
This project depends on:
- `libcairo2-dev`
- `libxi-dev`
- `libx11-dev`
- `x11proto-core-dev`
- `x11proto-dev`
- `libxt-dev`
- `libxfiles-dev`

## Building
```
make
./activate_linux
```

## Dependencies (MacOS) (Experimental)
- XQuartz
- Cairo

## Building
```
make
./activate_macos
```


![screenshot](screenshot.png)
