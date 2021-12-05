#ifndef LOADING_H
#define LOADING_H

#include "../set/set.h"

#define ELEMENT_MAX_SIZE 30

bool is_letter(char ch);
bool is_numeral(char ch);
bool is_ending_line(char ch);
bool is_separator(char ch);

FILE *open_input_file(int argc, char **argv);

int load_word(FILE *input, char *target, unsigned *len, unsigned maxlen);
int load_number(FILE *input, int *target);
int load_set_elements(Set *set, FILE *input);
int load_relations(Set *relation_set, FILE *input);

#endif /* LOADING_H */