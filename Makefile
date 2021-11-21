run: setcal
	$ ./setcal input.txt

compile: setcal.c
	$ gcc -std=c99 -Wall -Wextra -Werror setcal.c -o setcal