LIBS= `pkg-config --libs glib-2.0 gtk+-2.0 libmenu-cache`
CFLAGS+= -Wall -g `pkg-config --cflags glib-2.0 gtk+-2.0 libmenu-cache`

SRC= $(shell ls *.c 2> /dev/null)
OBJ= $(SRC:.c=.o)

all: $(OBJ) openbox-menu

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

openbox-menu: $(OBJ)
	$(CC) $(OBJ) -o openbox-menu $(LDFLAGS) $(LIBS)

.PHONY: clean doc

clean:
	@rm *.o openbox-menu
	@rm -rf doc

doc:
	robodoc --src . --doc doc/ --multidoc --index --html --cmode
