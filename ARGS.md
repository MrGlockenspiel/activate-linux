# Arguments for activate-linux

Activate linux takes command line arguments for all customizable options.

Scaling is used to display the message correctly on screens of different resolutions. 100% is based on 1080p. It also affects the offset from the corner of the screen
so is not reccomended to be changed unless you are not using a 1080p screen.

Color is formatted in "r-g-b-a", with each number being a decimal from 0 to 1. Default is a light grey color.

### Custom Main Message

```console
./activate-linux -t "Main Message"
```

### Custom Secondary Message

```console
./activate-linux -m "Secondary Message"
```

### Custom Scaling

```console
./activate-linux -s 1.5
```

### Custom Font

```console
./activate-linux -f "Ubuntu"
```

### Custom Color

```console
./activate-linux -c 0.1-0.1-0.1-0.1
```

### Enable Bold Text

```console
./activate-linux -b
```

### Enable Italics

```console
./activate-linux -i
```

### Run as daemon
```console
./activate-linux -d
```

### Skip compositor (only works for compliant compositors)
```console
./activate-linux -w
```