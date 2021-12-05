#ifndef PARSERS_H
#define PARSERS_H

#include "../set/set.h"
#include "../lines/lines.h"
#include "../loading/loading.h"

#define ELEMENT_MAX_SIZE 30

/**
 * @brief Signature of an LineParser function - function that can parse line
 * from an input file into Line structure. Program first determines what parser
 * to use based on the first letter of each line, then calls the parser. Parser
 * handles reading from the file on its own, continues reading the line after
 * first two chars (char that determines the parser and ' ' after it) untill
 * reaching end of the line. Returns 0 when parsing was succesful, otherwise
 * prints into stderr and returns 1.
 */
typedef int (*LineParser)(FILE *input, Line *target);

/**
 * @brief Links LineParser to an operation specificator.
 */
typedef struct operation_parser
{
    Operation operation;
    LineParser parser;
} OperationParser;

int parse_line(FILE *input, Line *target);
int parse_univerzum(FILE *input, Line *target);
int parse_set(FILE *input, Line *target);
int parse_relation(FILE *input, Line *target);
int parse_command(FILE *input, Line *target);

int parse_file(FILE *file);

#endif /* PARSERS_H */