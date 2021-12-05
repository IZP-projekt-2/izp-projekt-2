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

int main(int argc, char **argv)
{
    black_listed = set_ctor(uni);
    univerzum = set_ctor(uni);

    char *black_listed_words[] = {
        "true",
        "false",
    };

    set_add_elements(black_listed, black_listed_words, 2);

    for (int i = 0; commands[i].name != NULL; i++)
        set_add_elements(black_listed, &commands[i].name, 1);

    FILE *input = open_input_file(argc, argv);

    if (input == NULL)
        return 1;

    int res = parse_file(input);
    fclose(input);

    if (res)
        return res;

    for (int i = 1;; i++)
    {
        executed_next = 0;

        Line *line = lines[i];
        if (line == NULL)
            break;

        Set *set = line_get_set(line);

        if (set == NULL)
            return 1;

        set_print(set, stdout);

        if (executed_next)
            i = executed_next - 1; // i++
    }

    set_dtor(black_listed);
    lines_dtor();
}

/**
 * Checks if given type is of a "constant" set.
 *
 * @param type Type to be checked.
 * @return Bool.
 */

bool is_constant_type(SetType type)
{
    return type == num || type == bol;
}

/**
 * Creates a set of a given type with initial elements on a heap. On error
 * prints to a stderr and returns NULL. Cannot create "constant" sets - for that
 * use const_set_ctor.
 *
 * @todo add 'black listed' elements - command names + true/false
 *
 * @param type Type of a set.
 * @param init_elements List of strings (elements) or NULL.
 * @param init_len Length of init_elements.
 * @return Pointer to set on a heap.
 */
Set *set_ctor(SetType type)
{
    if (is_constant_type(type))
    {
        fprintf(stderr,
                "Tried to create \"costant\" set using set_ctor. \
                Please use const_set_ctor instead.\n");

        return NULL;
    }

    else if (type != uni && univerzum == NULL)
    {
        fprintf(stderr, "Creating set without univerzum.\n");
        return NULL;
    }

    Set *heap_pointer = malloc(sizeof(Set));

    if (heap_pointer == NULL)
    {
        fprintf(stderr, "Malloc failed.\n");
        return NULL;
    }

    heap_pointer->len = 0;
    heap_pointer->type = type;
    heap_pointer->elements = NULL;

    return heap_pointer;
}

/**
 * Creates "constant" set on a heap with a given value.
 *
 * @param type Type of a set.
 * @param value Sets value.
 * @return Pointer to heap.
 */
Set *const_set_ctor(SetType type, int value)
{
    if (!is_constant_type(type))
    {
        fprintf(stderr, "Tried to create constant set of non-constant type\n");
        return NULL;
    }

    Set *heap_pointer = malloc(sizeof(Set));

    if (heap_pointer == NULL)
    {
        fprintf(stderr, "Malloc failed.\n");
        return NULL;
    }

    heap_pointer->elements = NULL;
    heap_pointer->type = type;
    heap_pointer->len = value;

    return heap_pointer;
}

/**
 * Tries to find element in a set (univerzum).
 *
 * @param set Set to be searched.
 * @param element String representing searched element.
 * @return Pointer to a found element stored in univerzum or NULL if element
 * is not contained.
 */
char *set_get_element(Set *set, char element[])
{
    if (set->type == uni)
        for (int i = 0; i < set->len; i++)
            if (!strcmp(set->elements[i], element))
                return set->elements[i];

    if (set->type == els)
    {
        char *pointer = set_get_element(univerzum, element);

        if (pointer == NULL)
            return NULL;

        // Should be returned only if its contained in a set
        for (int i = 0; i < set->len; i++)
            if (set->elements[i] == pointer)
                return set->elements[i];
    }

    return NULL;
}

/**
 * Checks if relation is already defined in a set.
 *
 * @param set Set to be searched.
 * @param first String representing first element of a relation.
 * @param second String representing second element of a relation.
 * @return Bool.
 */
bool set_contains_relation(Set *set, char *first, char *second)
{
    if (set->type != rel)
    {
        fprintf(stderr, "Can check for relation presence only \
                        in a set of relations.\n");
        return 1;
    }

    for (int i = 1; i < set->len; i += 2)
        if (set->elements[i - 1] == first && set->elements[i] == second)
            return true;

    return false;
}

/**
 * Adds elements to a set. If given set is set of relations then expects odd
 * number of elements.
 *
 * @param set Set to be added to.
 * @param elements List of elements.
 * @param len Number of elements to be added.
 * @return 0 if addinf was succesful. On any errors prints to a stderr and
 * returns 1.
 */
int set_add_elements(Set *set, char *elements[], int len)
{
    if (is_constant_type(set->type))
    {
        fprintf(stderr, "Cannot add elements to \"constant\" sets.\n");
        return 1;
    }

    if (set->type == rel && len % 2)
    {
        fprintf(stderr,
                "Cannot add odd number of elements to a set of relations.\n");
        return 1;
    }

    int new_len = set->len + len;
    char **new_el_pointer = realloc(set->elements,
                                    sizeof(char **) * (new_len + 1));

    if (new_el_pointer == NULL)
    {
        fprintf(stderr, "Reallocating memory for new set elements failed.\n");
        return 1;
    }

    set->elements = new_el_pointer;

    for (int index = 0; index < len; index++)
    {
        char *element = elements[index];

        if (set->type != uni)
        {
            element = set_get_element(univerzum, element);

            if (element == NULL)
            {
                fprintf(stderr, "Element '%s' isn't defined in univerzum.\n",
                        elements[index]);
                return 1;
            }
        }

        if (set->type == els || set->type == uni)
            if (set_get_element(set, element) != NULL)
            {
                fprintf(stderr, "Element is already contained.\n");
                return 1;
            }

        if (set->type == rel && (index % 2))
            if (set_contains_relation(set,
                                      set->elements[set->len - 1], element))
            {
                fprintf(stderr, "Duplicate relation definition.\n");
                return 1;
            }

        if (set->type == uni)
        {
            if (set_get_element(black_listed, element) != NULL)
            {
                fprintf(stderr,
                        "\"%s\" cannot be used as an element.\n", element);
                return 1;
            }

            char *str_heap_pointer = malloc(strlen(element) + 1);

            if (str_heap_pointer == NULL)
            {
                fprintf(stderr, "Allocating memory to store new univerzum \
                                element failed.\n");
                return 1;
            }

            strcpy(str_heap_pointer, element);
            element = str_heap_pointer;
        }

        set->elements[(set->len++)] = element;
    }

    return 0;
}

/**
 * Prints set to a stream.
 *
 * @param set Set to be printed.
 * @param where Stream where the set is printed.
 */
void set_print(Set *set, FILE *where)
{
    if (set->type == num)
        fprintf(where, "%d", set->len);

    else if (set->type == bol)
        fprintf(where, "(%s)", set->len ? "true" : "false");

    else
    {
        fprintf(where, "%c ", set->type);
        if (set->type == rel)
            for (int i = 0; i < set->len; i += 2)
                fprintf(where, "(%s %s) ",
                        set->elements[i],
                        set->elements[i + 1]);
        else
            for (int i = 0; i < set->len; i++)
                fprintf(where, "%s ", set->elements[i]);
    }

    fprintf(where, "\n");
}

/**
 * Set destructor.
 *
 * @param set Pointer to set to be destructed.
 */
void set_dtor(Set *set)
{
    if (set != NULL)
        free(set->elements);

    free(set);
}

/**
 * Creates new Line with given operation. On error prints to stderr and returns
 * NULL.
 *
 * @param operation Line operation identifier.
 * @return Pointer to Line struct on a heap or NULL on error.
 */
Line *line_ctor(Operation operation)
{
    Line *heap_pointer = malloc(sizeof(Line));

    if (heap_pointer == NULL)
    {
        fprintf(stderr, "Memory allocation failed when creating new line.\n");
        return NULL;
    }

    heap_pointer->operation = operation;
    heap_pointer->related_set = NULL;
    heap_pointer->command = NULL;
    heap_pointer->expected_args = NULL;

    for (int i = 0; i < MAX_COMMAND_ARGS + 1; i++)
        heap_pointer->args[i] = 0;

    return heap_pointer;
}

/**
 * Gets set from a line. If line contains command executes that command and
 * returns execution result. If any errors occur returns NULL.
 *
 * @param line Line cointaining wanted set.
 * @return Pointer to a set.
 */
Set *line_get_set(Line *line)
{
    if (line == NULL)
    {
        fprintf(stderr, "Line doesn't exist.\n");
        return NULL;
    }

    if (line->related_set != NULL)
        return line->related_set;

    else if (line->operation == exe_command)
        return line_exec(line);

    fprintf(stderr, "Empty Line object.\n");
    return NULL;
}

/**
 * Executes command on a line and assigns result as related_set. If line doesn't
 * contain a command, or any errors occur, prints to stderr and return NULL.
 *
 * @param line Line to be executed.
 * @return Pointer to resulting set.
 */
Set *line_exec(Line *line)
{
    if (line == NULL)
    {
        fprintf(stderr, "Line isn't defined.\n");
        return NULL;
    }

    if (line->operation != exe_command)
    {
        fprintf(stderr, "Trying to execute non-command line.\n");
        return NULL;
    }

    if (line->command == NULL)
    {
        fprintf(stderr, "Line wasn't assigned to a command yet.\n");
        return NULL;
    }

    unsigned param = 0; /** @todo make use of a param*/

    Set *line_args[MAX_COMMAND_ARGS];
    for (int i = 0; i < MAX_COMMAND_ARGS; i++)
        line_args[i] = NULL;

    if (eval_args(line->args, line->expected_args, line_args, &param))
    {
        discard_args(line_args);
        return NULL;
    }

    Set *result = line->command(line_args);

    if (param)
    {

        if (result->type != bol)
        {
            fprintf(stderr, "Too many arguments. \
                        Non-bool returning commands don't support param\n");
            return NULL;
        }

        if (!result->len)
            executed_next = param;
    }

    discard_args(line_args);
    return result;
}

/**
 * Line destructor.
 *
 * @param line Pointer to line to be destructed.
 */
void line_dtor(Line *line)
{
    if (line != NULL)
        set_dtor(line->related_set);

    free(line);
}

/**
 * Initializes list of all file lines.
 */
void lines_init()
{
    for (int i = 0; i <= MAX_LINES + 1; i++)
        lines[i] = NULL;
}

/**
 * Destructs all loaded lines.
 */
void lines_dtor()
{
    for (int i = 0; i <= MAX_LINES + 1; i++)
    {
        if (lines[i] == NULL && i != 0) // 0th line is always NULL
            break;

        line_dtor(lines[i]);
    }
}

/**
 * Destructs all "constant" sets from arglist. (They are not connected to any
 * line so after command execution they would stay in memory wihtout any pointer
 * pointing to them).
 *
 * @param args Arglist to be destructed.
 */
void discard_args(Set *args[])
{
    for (int i = 0; i < MAX_COMMAND_ARGS; i++)
        if (args[i] != NULL && is_constant_type(args[i]->type))
            set_dtor(args[i]);
}

/**
 * Validates all arugments so they are the same type as an Expected and  turns
 * them into sets of given type.
 *
 * @param arglist List if intigers loaded as command argumenst.
 * @param expected Expected types of arguments.
 * @param target Pointer to list of set pointer where the result is stored.
 * @param param Pointer to where param should be stored.
 * @return Returns 0 if all goes smoothly, else prints to stderr and returns 1.
 */
int eval_args(unsigned arglist[],
              CommandArgs expected,
              Set *target[],
              unsigned *param)
{
    if (expected == NULL)
    {
        fprintf(stderr, "Argument patter not specified.\n");
        return 1;
    }

    int i = 0;

    for (i = 0; i < MAX_COMMAND_ARGS; i++)
    {
        unsigned arg = arglist[i];
        CommandArgumentType expected_arg = expected[i];

        if (expected_arg == non)
            break;

        else if (expected_arg == num)
            target[i] = const_set_ctor(num, arg);

        else if (expected_arg == els || expected_arg == rel)
        {
            if (arg > MAX_LINES)
            {
                fprintf(stderr, "Argument too large (%d).\n", arg);
                return 1;
            }

            Set *set = line_get_set(lines[arg]);

            if (set == NULL)
                return 1;

            if ((CommandArgumentType)set->type != expected_arg)
                if (!(set->type == uni && expected_arg == elements))
                {
                    fprintf(stderr,
                            "Set on line %d isn't of an expected type.\n", arg);
                    return 1;
                }

            target[i] = set;
        }

        else
        {
            fprintf(stderr, "Unsupported argument type.\n");
            return 1;
        }
    }

    *param = arglist[i];

    if (i <= MAX_COMMAND_ARGS && arglist[i + 1] != 0)
    {
        fprintf(stderr, "Too many arguments.\n");
        return 1;
    }

    return 0;
}

/**
 * Checks if character is letter of en english alphabet.
 *
 * @param ch Char to be checked.
 * @return Bool is valid.
 */
bool is_letter(char ch)
{
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
}

/**
 * Checks if character is a numeral.
 *
 * @param ch Char to be checked.
 * @return Bool is valid.
 */
bool is_numeral(char ch)
{
    return '0' <= ch && ch <= '9';
}

/**
 * Checks if character valid line ending character ('\\n' or EOF).
 *
 * @param ch Char to be checked.
 * @return Bool is valid.
 */
bool is_ending_line(char ch)
{
    return ch == '\n' || ch == EOF;
}

/**
 * Checks if character valid elements separating character ('\\n', ' ' or EOF ).
 *
 * @param ch Char to be checked.
 * @return Bool is valid.
 */
bool is_separator(char ch)
{
    return is_ending_line(ch) || ch == ' ';
}

/**
 * Opens input file based on program arguments for reading. Returns NULL on
 * invalid arguments or when the file is not found and prints to stderr.
 *
 * @param argc Programs argc.
 * @param argv Programs argv.
 * @return Pointer to an input file.
 */
FILE *open_input_file(int argc, char **argv)
{
    FILE *input_file = NULL;

    if (argc != 2)
    {
        fprintf(stderr, "Invalid number of program arguments.\n");
        return NULL;
    }

    input_file = fopen(argv[1], "r");

    if (input_file == NULL)
        fprintf(stderr, "Cannot open file '%s'\n", argv[1]);

    return input_file;
}

/**
 * Loads next word (string containing letters of english alphabet) from an input
 * stream.
 *
 * @param input Stream to be continued reading.
 * @param target Where the loaded element terminated with '\\0'is stored.
 * @param len Where the length of loaded word is stored.
 * @param maxlen Maximal length of an element, not counting the '\\0' char.
 * @return Int-parsed char following the loaded word. If loading is stopped due
 * to reaching maxlen then the first overreaching char is returned.
 */
int load_word(FILE *input, char *target, unsigned *len, unsigned maxlen)
{
    char character;
    unsigned index = 0;

    for (;; index++)
    {
        if (!is_letter(character = fgetc(input)) || index == maxlen)
            break;

        target[index] = character;
    }

    target[index] = '\0';
    *len = index;
    return character;
}

/**
 * Loads positive intiger number from an input stream. If reading fails returns,
 * the loaded value is 0.
 *
 * @param input Stream to be continued reading.
 * @param target Where the loaded number is stored.
 * @return Int-parsed char following the loaded number.
 */
int load_number(FILE *input, int *target)
{
    char character;
    int value = 0;

    while (is_numeral(character = fgetc(input)))
    {
        value *= 10;
        value += (int)character - '0';
    }

    *target = value;
    return character;
}

/**
 * Loads elements to a set from given input stream.
 *
 * @param set Pointer to a target set.
 * @param input Input stream.
 * @return 0 if loading was succesful, else prints to stderr and returns 1.
 */
int load_set_elements(Set *set, FILE *input)
{
    unsigned el_len = 0;
    char last_char, element[ELEMENT_MAX_SIZE + 1];

    do
    {
        last_char = load_word(input, element, &el_len, ELEMENT_MAX_SIZE);

        if (el_len == 0)
        {
            if (is_ending_line(last_char))
                break;

            else if (last_char == ' ')
                fprintf(stderr, "Elements separated by more than one space.\n");

            else
                fprintf(stderr, "Unexpected character found.\n");

            return 1;
        }

        else if (el_len == ELEMENT_MAX_SIZE && is_letter(last_char))
        {
            fprintf(stderr, "Element exceeds maximal lenght (%d).\n",
                    ELEMENT_MAX_SIZE);
            return 1;
        }

        else if (!is_separator(last_char))
        {
            fprintf(stderr, "Ivalid character in element definition.\n");
            return 1;
        }

        char *elements[] = {element};
        if (set_add_elements(set, elements, 1))
            return 1;

    } while (!is_ending_line(last_char));

    return 0;
}

/**
 * Loads relations to a set from given input stream.
 *
 * @param set Pointer to a target set.
 * @param input Input stream.
 * @return 0 if loading was succesful, else prints to stderr and returns 1.
 */
int load_relations(Set *relation_set, FILE *input)
{
    unsigned el_len;
    char last_char;

    char first_element[ELEMENT_MAX_SIZE + 1];
    char second_element[ELEMENT_MAX_SIZE + 1];

    do
    {
        if ((last_char = fgetc(input)) != '(')
        {
            if (is_ending_line(last_char))
                break;

            fprintf(stderr, "Relation definition starts with '('.\n");
            return 1;
        }

        last_char = load_word(input, first_element, &el_len, ELEMENT_MAX_SIZE);

        if (el_len == 0)
        {
            fprintf(stderr, "Expected element after '('\n");
            return 1;
        }

        else if (el_len == ELEMENT_MAX_SIZE && is_letter(last_char))
        {
            fprintf(stderr, "Element exceeds maximal lenght (%d).\n",
                    ELEMENT_MAX_SIZE);
            return 1;
        }

        else if (last_char != ' ')
        {
            fprintf(stderr, "Expected space after '%s'.\n", first_element);
            return 1;
        }

        last_char = load_word(input, second_element, &el_len, ELEMENT_MAX_SIZE);

        if (el_len == 0)
        {
            if (last_char == ' ')
                fprintf(stderr, "Elements separated by more than one space.\n");

            else
                fprintf(stderr, "Unexpected character found.\n");

            return 1;
        }

        else if (el_len == ELEMENT_MAX_SIZE && is_letter(last_char))
        {
            fprintf(stderr, "Element exceeds maximal lenght (%d).\n",
                    ELEMENT_MAX_SIZE);
            return 1;
        }

        else if (last_char != ')')
        {
            fprintf(stderr,
                    "Expected ')' after second element in an relation.\n");
            return 1;
        }

        if (!is_separator(last_char = fgetc(input)))
        {
            fprintf(stderr,
                    "Unxepected character after relation definition.\n");
            return 1;
        }

        char *elements[] = {first_element, second_element};
        if (set_add_elements(relation_set, elements, 2))
            return 1;
    } while (!is_ending_line(last_char));
    return 0;
}

OperationParser parsers[] = {
    {def_univerzum, &parse_univerzum},
    {def_set, &parse_set},
    {def_relation, &parse_relation},
    {exe_command, &parse_command},
    {0, NULL}};

/**
 * @brief Reads one line from an input file and parses it to Line struct.
 *
 * @param input stream to be read.
 * @param target ponter to Line struct where the result should be stored.
 * @return 0 when parsing was succesful, EOF when reaching end of file, non-zero
 * value when parsing fails.
 */
int parse_line(FILE *input, Line *target)
{
    LineParser parser = NULL;
    char character = fgetc(input);

    if (character == EOF)
        return character;

    if (fgetc(input) != ' ')
    {
        fprintf(stderr, "Expected space after first character.\n");
        return 1;
    }

    for (int i = 0; parsers[i].operation != 0; i++)
        if ((Operation)character == parsers[i].operation)
            parser = parsers[i].parser;

    if (parser == NULL)
    {
        fprintf(stderr, "Cannot process operation '%c'.\n", character);
        return 1;
    }

    return (*parser)(input, target);
}

int parse_univerzum(FILE *input, Line *target)
{
    univerzum = set_ctor(uni);

    if (load_set_elements(univerzum, input))
        return 1;

    target->related_set = univerzum;
    target->operation = def_univerzum;
    return 0;
}

int parse_set(FILE *input, Line *target)
{
    Set *set = set_ctor(els);

    if (set == NULL)
        return 1;

    if (load_set_elements(set, input))
        return 1;

    target->related_set = set;
    target->operation = def_set;
    return 0;
}

int parse_relation(FILE *input, Line *target)
{
    Set *relation_set = set_ctor(rel);

    if (relation_set == NULL)
        return 1;

    if (load_relations(relation_set, input))
        return 1;

    target->related_set = relation_set;
    target->operation = def_relation;

    return 0;
}

int parse_command(FILE *input, Line *target)
{
    unsigned len;
    char command_name[ELEMENT_MAX_SIZE + 1];
    char last_char = load_word(input, command_name, &len, ELEMENT_MAX_SIZE);

    for (int i = 0; commands[i].name != NULL; i++)
        if (!strcmp(command_name, commands[i].name))
        {
            target->command = commands[i].command;
            target->expected_args = commands[i].expected_args;
        }

    if (target->command == NULL)
    {
        fprintf(stderr, "Cannot proccess command \"%s\".\n", command_name);
        return 1;
    }
    else if (last_char != ' ')
    {
        fprintf(stderr, "Expected space after command name.\n");
        return 1;
    }

    for (int i = 0;; i++)
    {
        int value;
        char last_char = load_number(input, &value);

        if (value <= 0)
        {
            if (i == 0)
                fprintf(stderr, "No command arguments specified.\n");
            else if (last_char == '\n' || last_char == EOF) // Space at the end
                                                            // of the line
                                                            // doesn't crash
                break;

            fprintf(stderr, "Invalid argument.\n");
            return 1;
        }

        target->args[i] = value;

        if (last_char == EOF || last_char == '\n')
            break;

        if (last_char != ' ')
        {
            fprintf(stderr, "Unexpected character as command parameter.\n");
            return 1;
        }

        if (i >= MAX_COMMAND_ARGS - 1)
        {
            last_char = load_number(input, &value);

            if (value == 0)
            {
                if (last_char == '\n' || last_char == EOF)
                    break;

                else
                    fprintf(stderr,
                            "Unexpected character after command arguments.\n");
            }

            else
                fprintf(stderr, "Too many arguments.\n");
            return 1;
        }
    }

    target->operation = exe_command;
    return 0;
}

int parse_file(FILE *file)
{
    lines_init();

    for (int line_index = 1;; line_index++)
    {
        Line *line = line_ctor(0);
        int res = parse_line(file, line);

        if (res)
        {
            line_dtor(line);
            if (res == EOF)
                break;
            return res;
        }

        if (line_index > MAX_LINES)
        {
            fprintf(stderr, "Too many lines in input file (max: %d).\n",
                    MAX_LINES);
            return 1;
        }

        lines[line_index] = line;
    }

    return 0;
}

/******************************************************************************/
/** Commands ******************************************************************/
/******************************************************************************/

Set *intersect(Set *args[])
{
    Set *set1 = args[0];
    Set *set2 = args[1];
    Set *intersect_set = set_ctor(els);

    for (int i = 0; i < set1->len; i++)
    {
        char *element = set_get_element(set2, set1->elements[i]);
        if (element != NULL)
            set_add_elements(intersect_set, &element, 1);
    }

    return intersect_set;
}

Set *complement(Set *args[])
{
    Set *set = args[0];
    Set *res = set_ctor(els);

    for (int i = 0; i < univerzum->len; i++)
        if (set_get_element(set, univerzum->elements[i]) == NULL)
            set_add_elements(res, &(univerzum->elements[i]), 1);

    return res;
}

Set *subseteq(Set *args[])
{
    Set *set1 = args[0];
    Set *set2 = args[1];

    for (int i = 0; i < set1->len; i++)
        if (set_get_element(set2, set1->elements[i]) == NULL)
            return const_set_ctor(bol, false);

    return const_set_ctor(bol, true);
}
