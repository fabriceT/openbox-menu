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
previously commented.

## SVG support

Openbox can display SVG icons since version 3.5.1. SVG support in openbox-menu
has to be activated in Makefile by uncommenting the following line

   CFLAGS+=-DWITH_SVG

SVG suport will be activated by default in the future.

# Settings

## Default menu file

If no menu file specified in the command line, openbox-menu will use
"applications.menu" as default menu filename. If $XDG_MENU_PREFIX is set,
openbox-menu uses its content to prefix "applications.menu".

## User menu file

Openbox-menu looks up for a valid menu file in /etc/xdg/menus directory.
In order to make it looks up somewhere else, set up the $XDG_CONFIG_DIRS
variable (don't suffix pathnames with "menus", it's automatically added).

For example, if $XDG_CONFIG_DIRS is set like this:

  export XDG_CONFIG_DIRS="$HOME/.config:/etc/xdg"

And openbox-menu is called this way:

  openbox-menu my.menu

Openbox-menu will check for "my.menu" (or "my.menu" prefixed by the value
of $XDG_MENU_PREFIX) in "$HOME/.config/menus/" directory and, if the file
doesn't exist here, in "/etc/xdg/menus/" directory.
