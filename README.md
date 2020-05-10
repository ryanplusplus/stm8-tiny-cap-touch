# stm8-tiny-cap-touch
```
+-------------------+
| Paper             | To aluminum ground plane
|       +--------+  +--+-------------+
|       |  Alum. |  |  R (150K)      |
|       |  foil  +-----+--------+    +
|       |        |  |           |   GND
|       +--------+  |           +
|                   |          PC3
+-------------------+
```

## Setup
- Copy `tools/Linux/udev/70-st-link.rules` to `/etc/udev/` and run `udevadm control --reload-rules`.

## Use
### Compile
```shell
make -f <target>.mk
```

### Clean
```shell
make -f <target>.mk clean
```

### Erase via SWIM
```shell
make -f <target>.mk erase
```

### Flash via SWIM
```shell
make -f <target>.mk upload
```
