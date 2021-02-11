CC ?= gcc
PKG_CONFIG ?= pkg-config

CFLAGS ?= -O2 -g
CFLAGS += -Wall $(shell $(PKG_CONFIG) --cflags glib-2.0 gtk+-2.0 libmenu-cache)
LIBS += $(shell $(PKG_CONFIG) --libs glib-2.0 gtk+-2.0 libmenu-cache)

#-DG_DISABLE_DEPRECATED

# set to 0 to turn off icons in menus
WITH_ICONS=1

ifeq ($(WITH_ICONS),1)
CFLAGS += -DWITH_ICONS
endif

# set to 0 to turn off svg icons in menus
# Check SVG support with '$ ldd /usr/bin/openbox | grep svg', librsvg must appear..
WITH_SVG=0

ifeq ($(WITH_SVG),1)
CFLAGS += -DWITH_SVG
endif

prefix = /usr/local
DESTDIR ?= $(prefix)
BINDIR= ${DESTDIR}/bin

SRC= $(wildcard src/*.c)
OBJ= $(SRC:.c=.o)

all: $(OBJ) check openbox-menu

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


openbox-menu: $(OBJ)
	$(CC) $(OBJ) -o openbox-menu $(LDFLAGS) $(LIBS)

.PHONY: clean install install-strip doc changelog check xmllint

clean:
	@rm -f $(OBJ) $(TEST_OBJ) openbox-menu check
	@rm -rf doc

install:
	@install -Dm 755 openbox-menu $(BINDIR)/openbox-menu

install-strip:
	@strip -s openbox-menu
	@install -Dm 755 openbox-menu $(BINDIR)/openbox-menu

doc:
	robodoc --src . --doc doc/ --multidoc --index --html --cmode

xmllint: openbox-menu
	./openbox-menu > test.xml
	xmllint test.xml
	rm test.xml

changelog:
	@hg log --style changelog > ChangeLog
