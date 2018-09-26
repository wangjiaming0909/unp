CC = g++
FLAGS = -Wall -Wextra -std=c++11 -c -O0 -g2 -ggdb -Wfatal-errors
HEADERS = include/unp.h \
		  include/config.h \
		  include/headers.h
SOURCES = tcpserv.cpp \
		  lib/str_echo.cpp \
		  daytimetcpcli.cpp \
		  daytimetcpserv.cpp \
		  tcpcli01.cpp
OBJECTS = tcpserv.o \
		  str_echo.o \
		  daytimetcpcli.o \
		  daytimetcpserv.o \
		  tcpcli01.o
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
