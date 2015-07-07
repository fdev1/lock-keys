
# set this to 1 to enable an autostart option
# on the settings dialog
ENABLE_AUTOSTART=1

DESTDIR=/
PREFIX=/usr/local
ICONS_DIR=$(PREFIX)/share/icons

CC=gcc
CFLAGS=-O2 -Wall
EXECUTABLE=lock-keys
SOURCES=main.c settings.c overlay.c about.c
OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):  $(OBJECTS)
	$(CC) `pkg-config --libs gtk+-3.0` -o $@ $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS)  `pkg-config --cflags gtk+-3.0` \
		-DLK_TRAY_ICON_ON=\"$(ICONS_DIR)/lock-keys/tray_on.png\" \
		-DLK_TRAY_ICON_OFF=\"$(ICONS_DIR)/lock-keys/tray_off.png\" \
		-DLK_BIN_PATH=\"$(PREFIX)/bin/$(EXECUTABLE)\" \
		-DENABLE_AUTOSTART=$(ENABLE_AUTOSTART) -c $< -o $@
	
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
	
install:
	mkdir -p $(DESTDIR)/$(PREFIX)/bin
	mkdir -p $(DESTDIR)/$(ICONS_DIR)/lock-keys
	cp tray_on.png $(DESTDIR)/$(ICONS_DIR)/lock-keys/tray_on.png
	cp tray_off.png $(DESTDIR)/$(ICONS_DIR)/lock-keys/tray_off.png
	rm -f $(DESTDIR)/$(PREFIX)/bin/$(EXECUTABLE)
	cp $(EXECUTABLE) $(DESTDIR)/$(PREFIX)/bin
	
uninstall:
	rm -rf $(DESTDIR)/$(ICONS_DIR)/lock-keys
	rm -f $(DESTDIR)/$(PREFIX)/bin/$(EXECUTABLE)

