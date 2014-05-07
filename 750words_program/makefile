CC=gcc
CFLAGS=-c -std=c99 -Wall
INFLAGS=-Iinclude
LDFLAGS=-L.


OBJDIR:=objdir
SOURCES= misc.c structures.c inireader.c parser.c output.c  main.c
OBJECTS=$(addprefix $(OBJDIR)\, $(SOURCES:.c=.o) )

EXECUTABLE=noin 750words

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
