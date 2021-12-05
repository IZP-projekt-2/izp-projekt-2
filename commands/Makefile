objects = ../set/set.o commands.o

.PHONY: clean
.SILENT: $(objects)

compile: $(objects)
	@ cc -o test $(objects) 

clean: 
	@ -rm $(objects) test

$(objects): commands.h