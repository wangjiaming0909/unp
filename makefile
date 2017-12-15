CC = g++
FLAGS = -Wall -Wextra -std=c++11 -c -O0 -g2 -ggdb -Wfatal-errors
SOURCES = main.cpp \
		  tcpserv.cpp \
		  lib/str_echo.cpp 
HEADERS = include/unp.h \
		  include/config.h \
		  include/headers.h
OBJECTS = main.o \
		  tcpserv.o \
		  str_echo.o
TARGET = a.out
LIBS = 

all: $(TARGET)

$(target):$(objects) 
	@echo linking...
#	g++ -O1 $^ -L. -la -o $@ 
	g++  $^ -o $@  $(LIB)
	@echo ok...
	@mv *.o build/

$(OBJECTS):$(SOURCES) $(HEADERS)
	$(CC) $(FLAGS) $(SOURCES) 

clean:FORCE
	rm -f build/*.o 
	rm -f *.out
	@echo ok..
FORCE:
