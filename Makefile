CFLAGS = -std=c99 -Wall -Wextra -Werror
test_dirs = set loading lines parsing

.PHONY: test $(test_dirs)

run: setcal
	@./setcal input.txt

test: $(test_dirs)
	
$(test_dirs): 
	@ echo "--- Running test for $@ --- \n"
	@ -$(MAKE) -C $@ CFLAGS="$(CFLAGS)"
	@ echo "\n--- Test completed ---\n\n"

compile: setcal.c

include $(addsufix /Makefile $(test_dirs))

