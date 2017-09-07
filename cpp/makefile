target = build/cpp
objects = $(patsubst %.cc,%.o,$(wildcard *.cc))

all:$(target)

$(target):$(objects) *.h
	@echo linking...
	g++ -O1 $^ -o $@
	@echo ok...
	rm  $(objects)

$(objects):%.o:%.cc
#	@echo compiling...
	g++ -std=c++11 -c -O1  $< -o $@ -g 
	

clean:FORCE
	$(RM) $(target) $(objects)
FORCE:
