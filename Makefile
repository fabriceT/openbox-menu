LIBS= `pkg-config --libs glib-2.0 gtk+-2.0 libmenu-cache`
CFLAGS= -Wall -O -g `pkg-config --cflags glib-2.0 gtk+-2.0 libmenu-cache`

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
