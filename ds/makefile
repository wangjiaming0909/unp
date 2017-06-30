target = build/ds
objects = $(patsubst %.cc,%.o,$(wildcard *.cc))

all:$(target)

$(target):$(objects) *.h
	@echo linking...
	g++ $^ -o $@
	@echo ok...
	rm  $(objects)

$(objects):%.o:%.cc
	@echo compiling...
	g++ -c $< -o $@ -g 
	

clean:FORCE
	$(RM) $(target) $(objects)
FORCE:
