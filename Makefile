#CFLAGS=-c -g -Wall -pg
CFLAGS=-g -Wall

LDFLAGS=-lSDL2 -lSDL2_ttf

SOURCES=doommenu.c

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=doommenu

CC=gcc
CFLAGS+=-D_GNU_SOURCE=1

all: 
	$(MAKE) main
	$(MAKE) cleanobjs


main: $(SOURCES) $(EXECUTABLE)


$(EXECUTABLE): $(OBJECTS) 
	cp $(OBJECTS) $(EXECUTABLE)

.c.o:
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 

cleanobjs:
	rm -f $(OBJECTS)

clean:
	-rm -f doommenu *.o

