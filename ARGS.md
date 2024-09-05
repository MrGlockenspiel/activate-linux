# Arguments for activate-linux

Activate linux takes command line arguments for all customizable options.

Each option has own long name (e.g. `-t` and `--text-title`). Readability or shortness, you choose.

Scaling is used to display the message correctly on screens of different resolutions. 100% is based on 1080p. It also affects the offset from the corner of the screen
so is not recommended to be changed unless you are not using a 1080p screen.

Color is formatted in "r-g-b-a", with each number being a decimal from 0 to 1. Default is a light grey color.

## Text

### Custom Main Message
```console
./activate-linux -t "Main Message"
./activate-linux --text-title "Main Message"
```

### Custom Secondary Message
```console
./activate-linux -m "Secondary Message"
./activate-linux --text-message "Secondary Message"
```

### Run with a text preset
```console
./activate-linux -p "bsd"
./activate-linux --text-preset "bsd"
```

## Appearance

### Custom Font
```console
./activate-linux -f "Ubuntu"
./activate-linux --text-font "Ubuntu"
```

### Enable Bold Text
```console
./activate-linux -b
./activate-linux --text-bold
```

### Enable Italics
```console
./activate-linux -i
./activate-linux --text-italic
```

### Custom Color (r-g-b-a)
Default color is `1-1-1-0.35` (dimmed white)
```console
./activate-linux -c 0.1-0.1-0.1-0.1
./activate-linux --text-color 0.1-0.1-0.1-0.1
```

## Size and position

Note that message always appears in the top left corner of the overlay.

### Custom overlay width
```console
# Default width = 340px
./activate-linux -x 340
./activate-linux --overlay-width 340
```

### Custom overlay height
```console
# Default height = 120px
./activate-linux -y 120
./activate-linux --overlay-height 120
```

### Custom Scaling
```console
./activate-linux -s 1.5
./activate-linux --scale 1.5
```

## Other

### Skip compositor (only works for compliant compositors)
```console
./activate-linux -w
./activate-linux --bypass-compositor
```

### Run as daemon
```console
./activate-linux -d
./activate-linux --daemonize
```

### Kill running activate-linux instance (currently Windows-only)
Useful to kill running background daemon (started with `-d` option)
```console
./activate-linux -K
./activate-linux --kill-running
```

### Add verbose level (useful for debugging, shows events received by backend)
```console
# Show only errors
./activate-linux
# Show errors + warnings
./activate-linux -v
./activate-linux --verbose
# Show errors + warnings + information messages
./activate-linux -vv
# Show errors + warnings + information messages + debug info
./activate-linux -vvv
```

### Be quiet, don't spam console
```console
./activate-linux -q
./activate-linux --quiet
```

### List predefined presets
```console
./activate-linux -l
./activate-linux --text-preset-list
```

### Run as an external [Gamescope](https://github.com/Plagman/gamescope) overlay
```console
./activate-linux -G
./activate-linux --gamescope
```

### Force fake transparency rendering using the shaping extension on X11
```console
./activate-linux -S
./activate-linux --force-xshape
```

### Use a configuration file
[Config example](example.cfg)
```console
./activate-linux -C ~/config.cfg
./activate-linux --config-file ~/config.cfg
```

### Show embedded help
```console
./activate-linux -?
./activate-linux -h
./activate-linux --help
```
