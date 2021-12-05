objects = ../set/set.o ../commands/commands.o ../lines/lines.o ../loading/loading.o parsing.o test.o

.PHONY: clean
.SILENT: $(objects)

test_set: compile
	@ -./test input.txt
	@ $(MAKE) clean

compile: $(objects)
	@ cc -o test $(objects) 

clean: 
	@ -rm $(objects) test

$(objects): parsing.h