CC=g++
CFLAGS=-c -ansi -Wall
INFLAGS=-Iinclude
LDFLAGS=-L.


OBJDIR:=objdir
SOURCES=main.cpp
OBJECTS=$(addprefix $(OBJDIR)\, $(SOURCES:.cpp=.o) )

EXECUTABLE=noin

#-g  allow for debug mode 
#-E  macro expanded






all: $(SOURCES) $(EXECUTABLE)

$(OBJDIR)\\%.o: %.cpp
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
