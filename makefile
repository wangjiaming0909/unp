CC = g++
FLAGS = -Wall -Wextra -std=c++11 -c -O0 -g2 -ggdb -Wfatal-errors
HEADERS = include/unp.h

SOURCES = 
OBJECTS = 
TARGET = a.out
LIBS = 

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

$(OBJECTS):$(SOURCES) $(HEADERS)
	$(CC) $(FLAGS) $(SOURCES) 

clean:FORCE
	rm -f *.o 
	rm -f *.out
	@echo ok..
FORCE:
