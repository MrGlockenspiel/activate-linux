# Arguments for activate-linux

Activate linux takes command line arguments for custom text and scaling.

There are 5 different options for command line arguments: a custom main message, scale percent, custom main and secondary message, all 3 options, or all 3 options and custom color.

Scaling is used to display the message correctly on screens of different resolutions. 100% is based on 1080p. It also affects the offset from the corner of the screen
so is not reccomended to be changed unless you are not using a 1080p screen.

Color is formatted in "r-g-b-a", with each number being a decimal from 0 to 1. Default is a light grey color.

### Custom Main Message

```
./bin/activate_linux (message)
```

### Custom Scaling

```
./bin/activate_linux (scale)  
```

note that scale is a percent represented as a floating point number, 1 being 100%, 2 being 200%, and 0.5 being 50%

### Custom Main and secondary message

```
./bin/activate_linux (main) (secondary)
```

### All 3 options

```
./bin/activate_linux (main) (secondary) (scale)
```

### All 3 options and custom color

```
./bin/activate_linux (main) (secondary) (scale) (color)
```