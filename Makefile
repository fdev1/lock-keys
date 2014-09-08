
CC=gcc
LD=gcc
EXECUTABLE=lock-keys
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):  $(OBJECTS)
	$(LD) `pkg-config --libs gtk+-3.0` `pkg-config --libs gtk+-x11-3.0` `pkg-config --libs x11` -o $@ $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS)  `pkg-config --cflags gtk+-3.0` `pkg-config --cflags gtk+-x11-3.0` -I/usr/include/X11 -c $< -o $@
	
clean:
	rm $(OBJECTS) lock-keys
	