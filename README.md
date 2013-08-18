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

# Installation of the menu in Openbox

This is done by editing `menu.xml`, it's located in ~/.config/openbox.

First, you have to define your menu by adding this line:


