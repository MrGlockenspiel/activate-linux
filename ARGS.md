# Arguments for activate-linux

Activate linux takes command line arguments for all customizable options.

Scaling is used to display the message correctly on screens of different resolutions. 100% is based on 1080p. It also affects the offset from the corner of the screen
so is not recommended to be changed unless you are not using a 1080p screen.

Color is formatted in "r-g-b-a", with each number being a decimal from 0 to 1. Default is a light grey color.

## Text

### Custom Main Message
```console
./activate-linux -t "Main Message"
```

### Custom Secondary Message
```console
./activate-linux -m "Secondary Message"
```

## Appearance

### Custom Font
```console
./activate-linux -f "Ubuntu"
```

### Enable Bold Text
```console
./activate-linux -b
```

### Enable Italics
```console
./activate-linux -i
```

### Custom Color (r-g-b-a)
```console
./activate-linux -c 0.1-0.1-0.1-0.1
```

## Size and position

### Custom overlay width
```console
# Default width = 340px
./activate-linux -x 340
```

### Custom overlay height
```console
# Default height = 120px
./activate-linux -y 120
```

### Custom Scaling
```console
./activate-linux -s 1.5
```

### Move overlay horizontally
```console
# 42 pixels left
./activate-linux -H 42
# 42 pixels right
./activate-linux -H -42
```

### Move overlay vertically
```console
# 42 pixels bottom
./activate-linux -V 42
# 42 pixels up
./activate-linux -V -42
```

## Other

### Skip compositor (only works for compliant compositors)
```console
./activate-linux -w
```

### Run as daemon
```console
./activate-linux -d
```

### Verbose output (useful for debugging, shows received X events)
```console
./activate-linux -v
```
