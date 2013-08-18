Openbox-menu is a pipemenu for Openbox window manager. It provides a dynamic
menu listing installed applications. Most of the work is done by the LXDE
library menu-cache.

# Compilation.

Type `make` then, with superuser privileges, `make install` to install it.
openbox-menu is installed in /usr/local/bin directory by default.type
`make install DESTDIR=/usr` to install it to /usr/bin/directory.

## Compile openbox-menu without icons ##

Openbox-menu shows icons before categories name and applications name (in
fact, the menu and the icons are displayed by Openbox; Openbox-menu
only outputs text content). You can remove icon support by editing the
Makefile and commenting the following line (add a # in the begining)

   CFLAGS+=-DWITH_ICONS

If errors occur while compiling, remove gtk+-2.0 from lines above the one
you have previously commented.

## SVG support

Openbox can displays SVG icons since version 3.5.1. SVG support in openbox-menu
has to be activated in Makefile by decommenting the following line

   CFLAGS+=-DWITH_SVG
 
It will be activated by default in the future.

