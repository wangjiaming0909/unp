CC = g++
FLAGS = -Wall -Wextra -std=c++11 -c -O0 -g2 -ggdb -Wfatal-errors
#HEADERS = include/unp.h

SOURCEDIR = ./server
BUILDDIR = ./build
OBJDIR = $(BUILDDIR)/obj

MKDIR = mkdir -p
RM = rm -rf

SOURCES=$(shell find $(SOURCEDIR) -type f -name '*.cpp')
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES) )


TARGET = $(BUILDDIR)/a.out
LIBS = 
INCLUDES = /boost_1_68_0
LDFLAGS = 

all: $(BUILDDIR) $(TARGET)

$(BUILDDIR):
	$(MKDIR) $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ 

$(OBJECTS): $(OBJDIR)/%.o : $(SOURCEDIR)/%.cpp
	@$(MKDIR) $(dir $@)
	$(CC) $(FLAGS) $< -o $@ -I $(INCLUDES)

clean:FORCE
	$(RM) $(OBJECTS) $(TARGET)
FORCE:
