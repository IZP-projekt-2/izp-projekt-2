#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ELEMENT_MAX_SIZE 30
#define MAX_LINES 1000
#define UNIVERZUM_ALLOC 1024 // How many 'pointer sizes' are allocated when
                             // expanding univerzum size.
                             //
                             // Note: for smaller sizes was getting segmentation
                             // fault when trying to realloc 5+ times. With 1024
                             // tested up to 70k+ elements.

typedef struct univerzum
{
    unsigned len;    // Number of elements.
    unsigned size;   // How much space was allocated for elements (number of
                     // possible pointers, not size in bytes).
    char **elements; // Dynamically allocated array of elements represented as
                     // strings.
} Univerzum;

void univerzum_ctor(Univerzum *uni);
void univerzum_dtor(Univerzum *uni);
void univerzum_expand(Univerzum *uni);
int univerzum_add_element(Univerzum *uni, char element[]);
char *univerzum_get_element(Univerzum *uni, char element[]);

void print_univerzum(Univerzum *uni);

/******************************************************************************/

typedef enum
{
    uni = 85, // Ord value of U. Set containing all elements of univerzum.
    els = 83, // Ord value of S. Set containing elements of univezum.
    rel = 82, // Ord value of R. Set containing relations.
    cst = 0,  // "Constant" set. TODO explain this.
} SetType;

typedef struct
{
    SetType type;    // Type of given set. Elements are treated differently
                     // based on what type of set they belong to.
    char **elements; // Pointers to elements in univerzum.
    unsigned len;    // Number of elements.
} Set;

int set_init(Set *set, SetType type, char **elements, unsigned len);
int set_add_element(Set *set, char *element);
void set_print(Set *set);
  

/******************************************************************************/

typedef enum
{
    def_univerzum = uni,
    def_set = els,
    def_relation = rel,
    exe_command = 67 // ord value of C.
} Operation;

typedef struct set
{
    Operation operation;
    Set *related_set; // pro U - množina všech prvků
                      // pro S R - definovaný set
                      // pro C (prémiové řešení) - co je výsledek
                      // Možnost - budou dva speciální sety které budou
                      // reprezentovat TRUE a FALSE, nemusí se pak dělat další
                      // proměná tady aby se dalo zjistit jestli je to T/F
                      // operace

    // Command command;

    unsigned first_argument;  // pro U S R bude 0, jinak číslo řádku (0vý řádek neexistuje)
    unsigned second_argument; // ---------||--------, pro příkazy co berou jeden taky 0
    unsigned parameter;       // Nko z prémiového řešení
} Line;

void line_ctor(Line *line);
void line_dtor(Line *line);
void line_link_set(Line *line, Set *set);

/******************************************************************************/

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

/******************************************************************************/

static Line lines[MAX_LINES + 1]; // Lines are 1-indexed
static Univerzum univerzum;
static OperationParser operation_parsers[] = {
    {def_univerzum, &parse_univerzum},
    {def_set, &parse_set},
    {def_relation, &parse_relation},
    {0, NULL}}; // "null" element of array - used to stop iteration.

/******************************************************************************/

FILE *open_input_file(int argc, char **argv);
void read_file(FILE *input_file);
int load_word(FILE *input, char *target, unsigned *len, unsigned maxlen);
bool is_letter(char ch);

int main(int argc, char **argv)
{
    univerzum_ctor(&univerzum);
    FILE *input_file = open_input_file(argc, argv);
    read_file(input_file);

    fclose(input_file);
}

void read_file(FILE *input_file)
{
    for (int line_index = 1; line_index <= MAX_LINES; line_index++)
    {
        Line line;
        line_ctor(&line);
        int parsing_res = parse_line(input_file, &line);

        if (parsing_res == EOF)
            break;

        else if (parsing_res)
        {
            fprintf(stderr, "Preceeding error occured on line %d.\n",
                    line_index);
            exit(1);
        }

        set_print(line.related_set);
        printf("\n");
        lines[line_index] = line;
    }
}

/**
 * @brief Loads next word (string containing letters of english alphabet) from
 * an input file.
 *
 * TODO: maxlen doesn't work - abort trap: 6
 *
 * @param input stream to be continued reading
 * @param target where the loaded element terminated with '\\0'is stored
 * @param len where the lenght of loaded word is stored
 * @param maxlen maximal length of an element, not counting the '\\0' char
 * @return int-parsed char following loaded word. If loading is stopped due to
 * reaching maxlen then the first overreaching char is returned
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
 * @brief Checks if character is letter of english alphabet.
 *
 * @param ch char to be checked.
 * @return bool is valid.
 */
bool is_letter(char ch)
{
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
}

/**
 * @brief Opens input file based on program arguments. Exits program on invalid
 * arguments or when the file is not found.
 *
 * @param argc
 * @param argv
 * @return FILE* pointer to input file.
 */
FILE *open_input_file(int argc, char **argv)
{
    FILE *input_file = NULL;

    if (argc != 2)
    {
        fprintf(stderr, "Invalid number of arguments\n");
        exit(1);
    }

    input_file = fopen(argv[1], "r");

    if (input_file == NULL)
    {
        fprintf(stderr, "Cannot open file '%s'\n", argv[1]);
        exit(1);
    }

    return input_file;
}

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

    for (int i = 0; operation_parsers[i].operation != 0; i++)
        if ((Operation)character == operation_parsers[i].operation)
            parser = operation_parsers[i].parser;

    if (parser == NULL)
    {
        fprintf(stderr, "Cannot process operation '%c'.\n", character);
        return 1;
    }

    return (*parser)(input, target);
}

int parse_univerzum(FILE *input, Line *target)
{
    unsigned len;
    char last_char, element[ELEMENT_MAX_SIZE + 1];

    while (1)
    {
        last_char = load_word(input, element, &len, ELEMENT_MAX_SIZE);
        univerzum_add_element(&univerzum, element);

        if (len == 0)
        {
            fprintf(stderr, "Elements separated by more than one space.\n");
            return 1;
        }

        else if (last_char == EOF || last_char == '\n')
            break;

        else if (len != ELEMENT_MAX_SIZE && last_char != ' ')
        {
            fprintf(stderr, "Element contains invalid characters\n");
            return 1;
        }

        else if (last_char != ' ')
        {
            fprintf(stderr, "Element exceeds maximal lenght (%d).\n",
                    ELEMENT_MAX_SIZE);
            return 1;
        }
    }

    Set all_elemets;
    set_init(&all_elemets, uni, univerzum.elements, univerzum.len);
    line_link_set(target, &all_elemets);

    return 0;
}

int parse_set(FILE *input, Line *target)
{
    unsigned len, index = 0;
    char last_char, element[ELEMENT_MAX_SIZE + 1];
    char *set_elements[univerzum.len];

    while (1)
    {
        last_char = load_word(input, element, &len, ELEMENT_MAX_SIZE);
        set_elements[index] = univerzum_get_element(&univerzum, element);

        if (len == 0)
        {
            fprintf(stderr, "Elements separated by more than one space.\n");
            return 1;
        }

        else if (set_elements[index++] == NULL ||
                 (last_char != ' ' && last_char != EOF && last_char != '\n'))
        {
            fprintf(stderr, "Element isn't defined in univezum.\n");
            return 1;
        }

        else if (last_char == EOF || last_char == '\n')
            break;
    }

    Set set;
    if (set_init(&set, els, set_elements, index))
    {
        fprintf(stderr, "Element repeated in set definition.\n");
        return 1;
    }
    line_link_set(target, &set);

    return 0;
}

int parse_relation(FILE *input, Line *target)
{
    unsigned len, index = 0;
    char last_char, element[ELEMENT_MAX_SIZE + 1];
    char *set_elements[univerzum.len * univerzum.len];
    bool first = true;

    while (1)
    {
        // fprintf(stderr, "%d ", index);

        if (first)
        {
            if (fgetc(input) != '(')
            {
                fprintf(stderr, "Opening bracket expected.\n");
                return 1;
            }
        }

        last_char = load_word(input, element, &len, ELEMENT_MAX_SIZE);
        set_elements[index] = univerzum_get_element(&univerzum, element);

        if (len == 0)
        {
            if (first)
                fprintf(stderr, "Unexpected space after '('.\n");
            else
                fprintf(stderr, "Elements separated by more than one space.\n");
            return 1;
        }

        else if (set_elements[index++] == NULL ||
                 (last_char != ' ' &&
                  last_char != EOF &&
                  last_char != '\n' &&
                  last_char != ')'))
        {
            fprintf(stderr, "Element isn't defined in univezum.\n");
            return 1;
        }

        else if (first && last_char == ')')
        {
            fprintf(stderr, "')' after first element in a relation.\n");
            return 1;
        }

        else if (!first && last_char != ')')
        {
            fprintf(stderr, "')' expected after second element in relation.\n");
            return 1;
        }

        else if (last_char == EOF || last_char == '\n')
        {
            fprintf(stderr, "Line ended unexpectedly.\n");
            return 1;
        }

        if (!first && ((last_char = fgetc(input)) == EOF || last_char == '\n'))
            break;

        first = !first;
    }

    Set set;
    if (set_init(&set, rel, set_elements, index))
    {
        fprintf(stderr, "Element repeated in set definition.\n");
        return 1;
    }
    line_link_set(target, &set);

    return 0;
}

// Initializes univezum values.
void univerzum_ctor(Univerzum *uni)
{
    uni->len = 0;
    uni->size = 0;
    univerzum_expand(uni);
}

// Destructs uiverzum.
void univerzum_dtor(Univerzum *uni)
{
    for (unsigned i = 0; i < uni->len; i++)
        free(uni->elements[i]);

    free(uni->elements);
}

// Expands the allocated memory of univerzum.
void univerzum_expand(Univerzum *uni)
{
    uni->size += UNIVERZUM_ALLOC;
    uni->elements = realloc(uni->elements, uni->size * sizeof(char *));

    if (uni->elements == NULL)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
}

// Adds string to the univerzum. Returns 0 of element was added succesfuly, 1 if
// elements was already contained or the element was invalid (reserved keyword).
//
// TODO: Pořešit tohle:
//     "...Prvky univerza nesmí obsahovat identifikátory příkazů a klíčová slova
//      true a false... "
int univerzum_add_element(Univerzum *uni, char element[])
{
    if (univerzum_get_element(uni, element) != NULL)
        return 1;

    int el_size = strlen(element) + 1;
    char **element_p = &(uni->elements[(uni->len)++]);
    *element_p = malloc(el_size * sizeof(char));

    if (*element_p == NULL)
    {
        fprintf(stderr, "Memory allocation failed.");
        exit(1);
    }

    if (uni->len >= uni->size)
        univerzum_expand(uni);

    for (int i = 0; i <= el_size; i++)
        (*element_p)[i] = element[i];

    return 0;
}

// Returns pointer to given element in univerzum. If no such element is
// contained, returns NULL instead.
char *univerzum_get_element(Univerzum *uni, char element[])
{
    for (int i = 0; i < (int)uni->len; i++)
        if (!strcmp(uni->elements[i], element))
            return uni->elements[i];
    return NULL;
}

// Displays univerzum in format
// Univerzum: <1st element> <2nd element> ...
void print_univerzum(Univerzum *uni)
{
    printf("\nUniverzum: ");
    for(int i = 0; i < (int)uni->len; i++)
    {
        printf("%s ", uni->elements[i]);
    }
}

// TODO: check relation repetition
int set_init(Set *set, SetType type, char **elements, unsigned len)
{
    set->type = type;
    set->elements = malloc(sizeof(char **) * len);
    set->len = 0;

    for (int index = 0; index < len; index++)
        if (set_add_element(set, elements[index]))
            return 1;

    return 0;
}

// Adds element to a set. Returns 0 of element was added succesfully, 1 if
// elements was already contained.
// TODO: check relation repetition
int set_add_element(Set *set, char *element)
{
    if (set->type == rel)
        // TODO
        (void)42;

    else
        for (int i = 0; i < set->len; i++)
            if (element == set->elements[i])
                return 1;

    set->elements[set->len++] = element;
    return 0;
}

void set_print(Set *set)
{
    printf("%c ", (char)set->type);

    for (int i = 0; i < set->len; i++)
    {
        if (set->type == els || set->type == uni)
            printf("%s ", set->elements[i]);
        else if (set->type == rel)
        {
            printf("(%s %s) ", set->elements[i], set->elements[i + 1]);
            i++;
        }
    }
}

void line_ctor(Line *line)
{
    line->first_argument = 0;
    line->second_argument = 0;
    line->parameter = 0;
    line->operation = 0;
    line->related_set = NULL;
}

void line_dtor(Line *line)
{
    if (line->related_set != NULL)
        free(line->related_set);
}

void line_link_set(Line *line, Set *set)
{
    line->related_set = malloc(sizeof(*set));
    *(line->related_set) = *set;
}