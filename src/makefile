target = build/unp
objects = $(patsubst %.c,%.o,$(wildcard *.c))



$(target):$(objects) *.h
	@echo linking...
	gcc $^ -o $@
	@echo ok...
	rm  $(objects)

$(objects):%.o:%.c
	@echo compiling...
	gcc -c $< -o $@ -g 
	

clean:FORCE
	$(RM) $(target) $(objects)
FORCE:
