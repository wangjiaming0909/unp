target = ./a.out
objects = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
LIB =  #-lpthread
INCLUDE = ./include/#/boost_1_65_1/

all:$(target)

$(target):$(objects) 
	@echo linking...
#	g++ -O1 $^ -L. -la -o $@ 
	g++  $^ -o $@  $(LIB)
	@echo ok...
	@mv *.o build/

$(objects):%.o:%.cpp
# @echo compiling...
	@rm -f *.out
	g++ -Wall -Wextra -std=c++11 -c -O0  $< -o $@ -g2 -ggdb -Wfatal-errors
	

clean:FORCE
	rm -f build/*.o 
	rm -f *.out
	@echo ok..
FORCE: