objects = ../set/set.o ../commands/commands.o lines.o test.o

.PHONY: clean
.SILENT: $(objects)

test_set: compile
	@ -./test
	@ $(MAKE) clean

compile: $(objects)
	@ cc -o test $(objects) 

clean: 
	@ -rm $(objects) test

$(objects): lines.h