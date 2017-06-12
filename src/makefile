target = unp
objects = build/$(patsubst %.c,%.o,$(wildcard *.c))

$(target):$(objects)
	gcc $^ -o $@

$(objects):*.c *.h
	gcc -c $< -o $@ -g

clean:FORCE
	$(RM) $(target) $(objects)
FORCE:
