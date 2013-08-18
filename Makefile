LIBS= `pkg-config --libs glib-2.0 gtk+-2.0 libmenu-cache`
CFLAGS+= -g -Wall `pkg-config --cflags glib-2.0 gtk+-2.0 libmenu-cache`
CC=gcc
#-DG_DISABLE_DEPRECATED

# Comment this line if you don't want icons to appear in menu
CFLAGS+=-DWITH_ICONS
# Uncomment this line if Openbox can display SVG icons 
# Check SVG support with '$ ldd /usr/bin/openbox | grep svg', librsvg must appear..
# CFLAGS+=-DWITH_SVG

prefix= /usr/local
DESTDIR ?= $(prefix)
BINDIR= ${DESTDIR}/bin

SRC= $(shell ls *.c 2> /dev/null)
OBJ= $(SRC:.c=.o)

all: $(OBJ) openbox-menu

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

openbox-menu: $(OBJ)
	$(CC) $(OBJ) -o openbox-menu $(LDFLAGS) $(LIBS)

.PHONY: clean install doc changelog check

clean:
	@rm -f *.o openbox-menu
	@rm -rf doc

install:
	@strip -s openbox-menu
	@install -Dm 755 openbox-menu $(BINDIR)/openbox-menu

doc:
	robodoc --src . --doc doc/ --multidoc --index --html --cmode

check: openbox-menu
	./openbox-menu > test.xml 
	xmllint test.xml
	rm test.xml

changelog:
	@hg log --style changelog > ChangeLog
