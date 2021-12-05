#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_COMMAND_ARGS 3
#define MAX_LINES 1000
#define ELEMENT_MAX_SIZE 30

/**
 * Represent different types of sets. Elements and operations with them are done
 * differently based on type. There are two "constant" sets - they represent
 * value instead of a set.
 *
 * @note Constant sets are used to pass values from and to commands. This way,
 * the function executing command can do it no matter what argument types
 * command expects. Similarly, it can handle the returned value without having
 * to know if the command returns bool value, relations, or set of elements -
 * - that's concer of a function printing the result, or a command that the
 * result is passed to.
 */
typedef enum set_type
{
    uni = 85, // Ord value of U. Univerzum set.
    els = 83, // Ord value of S. Set containing elements of a univezum.
    rel = 82, // Ord value of R. Set containing relations.
    num = -2, // Numerical set - represents an intiger value.
    bol = -3, // Boolean set - True/False values.

    /**
     * @note using -2 and -3 so when comparing it to loaded character
     * doesn't yield true for any valid character or EOF.
     */

} SetType;

typedef struct set
{
    SetType type;    // Set type
    char **elements; // Elements of a set
    /**
     * @note Implementations:
     *  Univerzum - as list of strings.
     *  Relations/sets - as pointers to strings in univerzum.
     *  Constant sets - is NULL
     */

    int len; // Number of elements. For constant sets stores value.
} Set;

Set *black_listed; // Set containing all unallowed elements.
Set *univerzum;    // Univerzum of a program.

bool is_constant_type(SetType type);

Set *set_ctor(SetType type);
Set *const_set_ctor(SetType type, int value);
char *set_get_element(Set *set, char element[]);
bool set_contains_relation(Set *set, char *first, char *second);
int set_add_elements(Set *set, char *elements[], int len);
void set_print(Set *, FILE *where);
void set_dtor(Set *set);

typedef enum arg_type
{
    elements = els,
    relations = rel,
    number = num,

    non = 0 // Terminates argument definition. If there is still one more
            // argument specified, then its stored as a parameter. If command
            // returns 'bol' set then its used as N parameter, else error is
            // raised.
} CommandArgumentType;

typedef CommandArgumentType Arglist[MAX_COMMAND_ARGS + 1];
typedef CommandArgumentType *CommandArgs;
typedef Set *(*Command)(Set *args[]);

Set *intersect(Set *args[]);
Set *complement(Set *args[]);
Set *subseteq(Set *args[]);

typedef struct name_command
{
    char *name;
    Command command;
    Arglist expected_args;
} NameCommand;

static NameCommand commands[] = {
    /** @todo link all commands */

    // Sets of element commands
    {"empty", NULL, {elements, non}},
    {"card", NULL, {elements, non}},
    {"complement", &complement, {elements, non}},
    {"union", NULL, {elements, elements, non}},
    {"intersect", &intersect, {elements, elements, non}},
    {"minus", NULL, {elements, elements, non}},
    {"subseteq", &subseteq, {elements, elements, non}},
    {"subset", NULL, {elements, elements, non}},
    {"equals", NULL, {elements, elements, non}},

    // Sets of relations commands
    {"reflexive", NULL, {relations, non}},
    {"symmetric", NULL, {relations, non}},
    {"antisymmetric", NULL, {relations, non}},
    {"transitive", NULL, {relations, non}},
    {"function", NULL, {relations, non}},
    {"domain", NULL, {relations, non}},
    {"codomain", NULL, {relations, non}},
    {"injective", NULL, {relations, elements, elements, non}},
    {"surjective", NULL, {relations, elements, elements, non}},
    {"bijective", NULL, {relations, elements, elements, non}},

    // Premium commands
    {"closure_ref", NULL, {relations, non}},
    {"closure_sym", NULL, {relations, non}},
    {"closure_trans", NULL, {relations, non}},
    // {"select", NULL, {elements, number, non}},
    {NULL, NULL, {non}},
};

typedef enum
{
    def_univerzum = uni,
    def_set = els,
    def_relation = rel,
    exe_command = 67 // Ord value of C.
} Operation;

typedef struct line
{
    Operation operation;
    Set *related_set;
    Command command;
    CommandArgs expected_args;
    unsigned args[MAX_COMMAND_ARGS + 1]; // + 1 for possible param.
                                         // 0 is used as 'faulty' or NULL value,
                                         // since it cannot be used properly in
                                         // any usecase - 0th line doesn't exist
} Line;

unsigned executed_next;
Line *lines[MAX_LINES + 2]; /** @todo Change to 'static' */

Line *line_ctor(Operation operation);
Set *line_get_set(Line *line); // If not asociated try to get it.
Set *line_exec(Line *line);
void line_dtor(Line *line);

void lines_init();
void lines_dtor();

void discard_args(Set *args[]);
int eval_args(unsigned arglist[],
              CommandArgs expected,
              Set *target[],
              unsigned *param);

bool is_letter(char ch);
bool is_numeral(char ch);
bool is_ending_line(char ch);
bool is_separator(char ch);

FILE *open_input_file(int argc, char **argv);

int load_word(FILE *input, char *target, unsigned *len, unsigned maxlen);
int load_number(FILE *input, int *target);
int load_set_elements(Set *set, FILE *input);
int load_relations(Set *relation_set, FILE *input);

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