# Comment the line above if you don't want icons, it makes menu to appear faster.
#DISPLAY_ICONS= 1

CFLAGS= -Wall -O -g

ifdef DISPLAY_ICONS
 LIBS= `pkg-config --libs glib-2.0 gtk+-2.0 libmenu-cache`
 CFLAGS+= `pkg-config --cflags glib-2.0 gtk+-2.0 libmenu-cache` -DCAN_I_HAZ_ICONS
else
 LIBS= `pkg-config --libs glib-2.0 libmenu-cache`
 CFLAGS+= `pkg-config --cflags glib-2.0 libmenu-cache`
endif


SRC= $(shell ls *.c 2> /dev/null)
OBJ= $(SRC:.c=.o)

all: $(OBJ) openbox-menu

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

openbox-menu: $(OBJ)
	gcc $(LIBS) $(OBJ) -o openbox-menu
	strip -s openbox-menu

.PHONY: clean

clean:
	@rm *.o openbox-menu
