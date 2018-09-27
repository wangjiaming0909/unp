CC = g++
FLAGS = -Wall -Wextra -std=c++11 -c -O0 -g2 -ggdb -Wfatal-errors
HEADERS = include/unp.h

SOURCES = main.cpp
OBJECTS = main.o
TARGET = a.out
LIBS = 
INCLUDES = /boost_1_68_0

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) objects/$(OBJECTS) -o $(TARGET)

$(OBJECTS):$(SOURCES) $(HEADERS)
	$(CC) $(FLAGS) $(SOURCES) -I $(INCLUDES)
	mv ./*.o ./objects/

clean:FORCE
	rm -f ./objects/*.o 
	rm -f *.out
	@echo ok..
FORCE:
