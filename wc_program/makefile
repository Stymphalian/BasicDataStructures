CC=gcc
CFLAGS=-c -std=c99 -Wall
INFLAGS=-Iinclude
LDFLAGS=-L.


OBJDIR:=objdir
SOURCES=structures.c misc.c output.c parser.c main.c
OBJECTS=$(addprefix $(OBJDIR)\, $(SOURCES:.c=.o) )

EXECUTABLE=wc noin

#-g  allow for debug mode 
#-E  macro expanded



all: $(SOURCES) $(EXECUTABLE)

$(OBJDIR)\\%.o: %.c
	$(CC) $(CFLAGS) $(INFLAGS) $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)








# Setup objects directory
$(OBJECTS): | $(OBJDIR)
$(OBJDIR):
	mkdir $(OBJDIR)

# clean methods
clean:
	rmdir /s /q $(OBJDIR) 2> nul
