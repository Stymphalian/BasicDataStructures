CC=gcc
CFLAGS=-c -ansi -Wall
INFLAGS=-Iinclude 
LDFLAGS=-L.


OBJDIR:=objdir
SOURCES=main.c md5.c my_getline.c
OBJECTS=$(addprefix $(OBJDIR)\, $(SOURCES:.c=.o) )

EXECUTABLE=md5

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
