
ICONS_DIR=/usr/share/icons
USE_KDE=0
USE_KDE4=1

CC=gcc
CFLAGS=-O2 -Wall
DEST=/usr/local/bin
EXECUTABLE=lock-keys
SOURCES=main.c settings.c overlay.c
OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):  $(OBJECTS)
	$(CC) `pkg-config --libs gtk+-3.0` -o $@ $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS)  `pkg-config --cflags gtk+-3.0` \
		-DLK_TRAY_ICON_ON=\"$(ICONS_DIR)/lock-keys/tray_on.png\" \
		-DLK_TRAY_ICON_OFF=\"$(ICONS_DIR)/lock-keys/tray_off.png\" \
		-DLK_BIN_PATH=\"$(DEST)/$(EXECUTABLE)\" \
		-DLK_USE_KDE=$(USE_KDE) \
		-DLK_USE_KDE4=$(USE_KDE4) -c $< -o $@
	
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
	
install:
	mkdir -p $(ICONS_DIR)/lock-keys
	cp tray_on.png $(ICONS_DIR)/lock-keys/tray_on.png
	cp tray_off.png $(ICONS_DIR)/lock-keys/tray_off.png
	rm -f $(DEST)/$(EXECUTABLE)
	cp $(EXECUTABLE) $(DEST)
	
uninstall:
	rm -rf $(ICONS_DIR)/lock-keys
	rm -f $(DEST)/$(EXECUTABLE)

