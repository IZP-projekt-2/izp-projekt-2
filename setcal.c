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
    REL = 82, // Ord value of R. Set containing relations.
    num = -2,  // "Constant" set. TODO explain this.
    BOL = -3

} SetType;

typedef struct
{
    SetType type;    // Type of given set. Elements are treated differently
                     // based on what type of set they belong to.
    char **elements; // Pointers to elements in univerzum.
    unsigned len;    // Number of elements.
} Set;

int set_init(Set *set, SetType type, char **elements, unsigned len);
int set_create(Set *set, SetType type, unsigned len);
int set_add_element(Set *set, char *element);
void set_print(Set *set);

/******************************************************************************/

typedef enum
{
    def_univerzum = uni,
    def_set = els,
    def_relation = REL,
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
bool compare_strings(char *str1, char *str2);

/******************************************************************************/
int set_add(Set *set, char element[]);
void print_bool(bool b);
bool is_rel_el_in_set(Set * rel, Set * set, bool is_second);

// Set functions

Set * set_union(Set *set1, Set *set2, Set *union_set);
Set * set_intersect(Set *set1, Set *set2, Set *intersect_set);
Set * set_difference(Set *args[]);
Set * set_complement(Set * args[]);
Set * set_empty(Set * args[]);
Set * set_subseteq(Set * args[]);
Set * set_subset(Set * args[]);
Set * set_equal(Set * args[]);
Set * set_card(Set * args[]);


// Relation functions
Set * relation_reflexive(Set * args[]);
Set * relation_symmetric(Set * args[]);
Set * relation_antisymmetric(Set * args[]);
Set * relation_transitive(Set * args[]);
Set * relation_function(Set * args[]);
Set * relation_domain(Set * args[]);
Set * relation_codomain(Set * args[]);
Set * relation_injective(Set * args[]);
Set * relation_surjective(Set * args[]);
Set * relation_bijective(Set * args[]);

// Closures
Set * closure_ref(Set * args[]);
Set * closure_sym(Set * args[]);
Set * closure_trans(Set * args[]);

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
    if (set_init(&set, REL, set_elements, index))
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
    for (int i = 0; i < (int)uni->len; i++)
    {
        printf("%s ", uni->elements[i]);
    }
}

// Create an empty set
int set_create(Set *set, SetType type, unsigned len)
{
    set->type = type;
    set->elements = NULL;
    set->len = len;

    return 0;
}

// TODO: check relation repetition
int set_init(Set *set, SetType type, char **elements, unsigned len)
{
    set->type = type;
    set->elements = malloc(sizeof(char **) * len);
    set->len = 0;

    for (int index = 0; index < len; index++)
        if (set_add_element(set, elements[index])) // TODO: this line throws an error if size of `elements` is less than `len`
            return 1;

    return 0;
}

// Adds element to a set. Returns 0 of element was added succesfully, 1 if
// elements was already contained.
// TODO: check relation repetition
int set_add_element(Set *set, char *element)
{
    if (set->type == REL)
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
        else if (set->type == REL)
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

/*********************************************************************/

int set_add(Set *set, char element[]) {
    char ** tmp = realloc(set->elements, (set->len + 1) * sizeof(char *)); 
    if(tmp == NULL) {
        fprintf(stderr,"Memory allocation failed");
        return 1;
    }
    set->elements = tmp;
    set->elements[set->len] = element; //univerzum_get_element(uni, element);
    set->len++;
    return 0;
}


//prints the bool value
void print_bool(bool b) {
    printf(b ? "true\n" : "false\n");
}

/**
 * @brief Prints the union of two sets
 *
 * @param set1 1.set
 * @param set2 2.set
 * @param union_set a set that will be returned as a union of set 1 and 2
 * @return Set* of union of set 1 and 2
 */
Set * set_union(Set *set1, Set *set2, Set *union_set) {
    set_init(union_set, 'S', NULL, 0);
    //*union_set = *set1;
    for(unsigned i = 0; i < set1->len; i++) {
        set_add(union_set, set1->elements[i]);
    }
    //goes through the second set and if an element from it is not in the first set, adds it to the union 
    for(unsigned i = 0; i < set2->len; i++) {
        bool el_in_both = false;
        unsigned j = 0;
        for (; j < set1->len; j++)
        {
            if (set2->elements[i] == set1->elements[j])
            {
                el_in_both = true;
                break;
            }
        }
        if(!el_in_both) {
            set_add(union_set, set2->elements[i]);
        }
    }
    return union_set;
}

/**
 * @brief Prints the intersect of two sets
 *
 * @param set1 1.set
 * @param set2 2.set
 * @param uni Universe containing elements of the two sets
 */
Set * set_intersect(Set * set1, Set * set2, Set * intersect_set) {
    set_init(intersect_set, 'S', NULL, 0);
    //*intersect_set = *set1;
    /*for(unsigned i = 0; i < set1->len; i++) {
        set_add(intersect_set, set1->elements[i]);
    }*/
    for(unsigned i = 0; i < set1->len; i++) {
        for(unsigned j = 0; j < set2->len; j++) {
            if(set1->elements[i] == set2->elements[j]) {
                set_add(intersect_set, set1->elements[i]);
                break;
            }
        }
    }
    return intersect_set;
}


// TODO: delete this
bool compare_strings(char* str1, char* str2)
{
    for (int i = 0; str1[i] != '\0' || str2[i] != '\0'; i++)
    {
        if (str1[i] != str2[i])
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Returns pointer to difference of 2 sets
 *
 * @param set1 1.set
 * @param set2 2.set
 * @param uni Universe containing elements of the two sets
 */
Set * set_difference(Set * args[]) 
{
    Set *set1 = args[0];
    Set *set2 = args[1];
    Set *result = malloc(sizeof(int));
    set_init(result, 'S', NULL, 0);

    // if element is found in intersection, dont add it into set
    // if found is false, it should be added cause it's not in intersection of 2 sets and it's in set 1
    for (int i = 0; i < (int)set1->len; i++)
    {
        bool found = false;

        for (int j = 0; j < (int)set2->len; j++)
        {
            if (set1->elements[i] == set2->elements[j])
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            set_add(result, set1->elements[i]);
        }
    }
    set_print(result);
    printf("\n");

    return result;
}

/**
 * @brief Returns pointer to complement of 1.set
 *
 * @param set1 1.set
 * @param uni Universe
 */
Set * set_complement(Set * args[]) 
{
    Set *set1 = args[0];
    Set *result = malloc(sizeof(int));
    set_init(result, 'S', NULL, 0);
  
    // If element from 1.set IS NOT found in univerzum,
    // it is added to result
    for(int i = 0; i < (int)univerzum.len; i++) 
    {
        bool found = false;

        for (int j = 0; j < (int)set1->len; j++)
        {
            if(univerzum.elements[i] == set1->elements[j]) 
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            set_add(result, univerzum.elements[i]);
        }
    }

    set_print(result);
    printf("\n");

    return result;
}

/**
 * @brief Finds if relation is reflexive
 * 
 * @param args array of arguments, args[0] is the relation
 * @return Set* of type bool, where the bool value is saved
 */
Set * relation_reflexive(Set * args[]) {
    
    Set * rel = args[0];
    Set * result = malloc(sizeof(int));
    if(result == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return NULL;
    }
    result->type = BOL;

    int diff_elements = 0;
    bool is_different;
    int refl_pair = 0;
    
    //finds out how many different elements are in the relation
    for(unsigned i = 0; i < rel->len; i++) {
        is_different = true;
        for(unsigned j = i + 1; j < rel->len; j++) {
            if(strcmp(rel->elements[i], rel->elements[j]) == 0) {
                is_different = false;
                break;
            }
        }
        if(is_different == true) {
            diff_elements++;
        }
    }
    //finds the number of pairs of the same element
    for(unsigned i = 0; i < rel->len; i += 2) {
        if(strcmp(rel->elements[i], rel->elements[i + 1]) == 0) {
            refl_pair++;
        }
    }
    result->len = (int)(diff_elements == refl_pair);
    return result;
}

/**
 * @brief Finds if relation is symmetric
 * 
 * @param args array of arguments, args[0] is the relation
 * @return Set* of type bool, where the bool value is saved
 */
Set * relation_symmetric(Set * args[]) {

    Set * rel = args[0];
    Set * result = malloc(sizeof(int));
    if(result == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return NULL;
    }
    result->type = BOL;

    for(unsigned i = 0; i < rel->len; i += 2) {
        bool is_symm = false;
        //for each pair tries to find its symmetric pair
        for(unsigned j = 0; j < rel->len; j +=2) {
            if(strcmp(rel->elements[i], rel->elements[j + 1]) == 0
            && strcmp(rel->elements[i + 1], rel->elements[j]) == 0) {
                is_symm = true;
                break; 
            }
        }
        if(!is_symm) {
            result->len = (int)(false);
            return result;
        }
    }
    result->len = (int)(true);
    return result;
}

/**
 * @brief Finds if relation is antisymmetric
 * 
 * @param args array of arguments, args[0] is the relation
 * @return Set* of type bool, where the bool value is saved
 */
Set * relation_antisymmetric(Set * args[]) {
    
    Set * rel = args[0];
    Set * result = malloc(sizeof(int));
    if(result == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return NULL;
    }
    result->type = BOL;

    for(unsigned i = 0; i < rel->len; i += 2) {
        bool is_anti = true;
        //if a pair has two same elements it ignores the pair
        if(strcmp(rel->elements[i], rel->elements[i + 1]) == 0) {
            continue;
        }
        //for each pair tries to find its symmetric pair
        for(unsigned j = 0; j < rel->len; j +=2) {
            if(strcmp(rel->elements[i], rel->elements[j + 1]) == 0
            && strcmp(rel->elements[i + 1], rel->elements[j]) == 0) {
                is_anti = false;
                break; 
            }
        }
        if(!is_anti) {
            result->len = (int)(false);
            return result;
        }
    }
    result->len = (int)(true);
    return result;
}

/**
 * @brief Finds if relation is transitive
 * 
 * @param args array of arguments, args[0] is the relation
 * @return Set* of type bool, where the bool value is saved
 */
Set * relation_transitive(Set * args[]) {
    
    Set * rel = args[0];
    Set * result = malloc(sizeof(int));
    if(result == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return NULL;
    }
    result->type = BOL;

    for(unsigned i = 0; i < rel->len; i += 2) {
        bool is_tran = false;
        if(strcmp(rel->elements[i], rel->elements[i + 1]) == 0) {
            continue;
        }
        //tries to find if there is a pair j with the same first element 
        //as the second element of the pair i, if there isn't, 
        //pair i is transitive
        for(unsigned j = 0; j < rel->len; j += 2) {
            if(strcmp(rel->elements[i + 1],rel->elements[j]) == 0) {
                //tries to find pair k, that is the transitive closure of 
                //pairs i and j
                for(unsigned k = 0; k < rel->len; k += 2) {
                    if(strcmp(rel->elements[i],rel->elements[k]) == 0 
                    && strcmp(rel->elements[j + 1],rel->elements[k + 1]) == 0){
                        is_tran = true;
                        break;    
                    }
                }
                if(!is_tran) {
                    result->len = (int)(false);
                    return result;
                }
            }
        }
    }
    result->len = (int)(true);
    return result;  
}

/**
 * @brief Finds if relation is a function
 * 
 * @param args array of arguments, args[0] is the relation
 * @return Set* of type bool, where the bool value is saved
 */
Set * relation_function(Set * args[]) {

    Set * rel = args[0];
    Set * result = malloc(sizeof(int));
    if(result == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return NULL;
    }
    result->type = BOL;

    for(unsigned i = 0; i < rel->len; i+=2) {
        for(unsigned j = i + 2; j < rel->len; j += 2) {
            if(strcmp(rel->elements[i],rel->elements[j]) == 0) {
                result->len = (int)(false);
                return result;
            }
        }
    }
    result->len = (int)(true);
    return result;
}

/**
 * @brief Finds a domain of a relation and returns it
 * 
 * @param args array of arguments, args[0] is the relation
 * @return Set* containing the domain
 */
Set * relation_domain(Set * args[]) {
    
    Set * rel = args[0];
    Set * result = malloc(sizeof(int));
    if(result == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return NULL;
    }
    result->type = els;
    result->len = 0;
    result->elements = NULL;

    bool is_once;
    for(unsigned i = 0; i < rel->len; i += 2) {
        is_once = true;
        for(unsigned j = 0; j < result->len; j++) {
            if(strcmp(rel->elements[i],result->elements[j]) == 0) {
                is_once = false;
                break;
            }
        }
        if(is_once == true) {
            if(set_add(result, rel->elements[i]) == 1)
                return NULL;    
        }
    }
    return result;
}

/**
 * @brief Finds a codomain of a relation and returns it
 * 
 * @param args array of arguments, args[0] is the relation
 * @return Set* containing the codomain
 */
Set * relation_codomain(Set * args[]) {
    
    Set * rel = args[0];
    Set * result = malloc(sizeof(int));
    if(result == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return NULL;
    }
    result->type = els;
    result->len = 0;
    result->elements = NULL;

    bool is_once;
    for(unsigned i = 0; i < rel->len; i += 2) {
        is_once = true;
        for(unsigned j = 0; j < result->len; j++) {
            if(strcmp(rel->elements[i + 1],result->elements[j]) == 0) {
                is_once = false;
                break;
            }
        }
        if(is_once == true) {
            if(set_add(result, rel->elements[i + 1]) == 1)
                return NULL;    
        }
    }
    return result;
}


/**
 * @brief Finds if either every first or second element from a relation is in a set
 * 
 * @param rel 
 * @param set 
 * @param is_second decides if to check first or second el. in the relation
 * @return true if all el. on 1. or 2. position of the relation are in set
 * @return false if one or more elements are in relation and not in the set
 */
bool is_rel_el_in_set(Set * rel, Set * set, bool is_second) {
    
    bool in_set;
    for(unsigned i = is_second; i < rel->len; i += 2) {
        in_set = false;
        for(unsigned j = 0; j < set->len; j++) {
            if(strcmp(rel->elements[i],set->elements[j]) == 0) {
                in_set = true;
                break;
            }
        }
        if(!in_set) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Finds if relation is injective
 * 
 * @param args array of arguments, args[0] is the relation, args[1,2]
 *             are the 2 sets
 * @return Set* of type bool, where the bool value is saved
 */
Set * relation_injective(Set * args[]) {
    
    Set * rel = args[0];
    Set * set1 = args[1];
    Set * set2 = args[2];
    Set * result = malloc(sizeof(int));
    if(result == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return NULL;
    }
    result->type = BOL;

    if(!is_rel_el_in_set(rel, set1, false) || !is_rel_el_in_set(rel, set2, true)) {
        fprintf(stderr, "Error, an element in the relation is not in a set");
        return NULL;
    }
 
    if(set1->len > set2->len || rel->len/2 != set1->len ) {
        result->len = (int)(false);
        return result;
    }
    for (unsigned i = 0; i < rel->len; i += 2) {
        for (unsigned j = i + 2; j < rel->len; j += 2) {
            if(strcmp(rel->elements[i],rel->elements[j]) == 0 
            || strcmp(rel->elements[i + 1],rel->elements[j + 1]) == 0) {
                result->len = (int)(false);
                return result;
            }
        }
    }
    result->len = (int)(true);
    return result;
}

/**
 * @brief Finds if relation is surjective
 * 
 * @param args array of arguments, args[0] is the relation, args[1,2]
 *             are the 2 sets
 * @return Set* of type bool, where the bool value is saved
 */
Set * relation_surjective(Set * args[]) {
    
    Set * rel = args[0];
    Set * set1 = args[1];
    Set * set2 = args[2];
    Set * result = malloc(sizeof(int));
    if(result == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return NULL;
    }
    result->type = BOL;

    if(!is_rel_el_in_set(rel, set1, false) || !is_rel_el_in_set(rel, set2, true)) {
        fprintf(stderr, "Error, an element in the relation is not in a set");
        return NULL;
    }

    if(set1->len < set2->len || rel->len/2 != set1->len ) {
        result->len = (int)(false);
        return result;
    }

    unsigned diff_codomain = 0;
    bool is_different;
    //counts the number of different second elements
    for(unsigned i = 0; i < rel->len; i += 2) {
        is_different = true;
        for(unsigned j = i + 2; j < rel->len; j += 2) {
            if(strcmp(rel->elements[i + 1], rel->elements[j + 1]) == 0) {
                is_different = false;
                break;
            }
        }
        if(is_different == true) {
            diff_codomain++;
        }
    }
    
    //if this if were false, there would be some second element not used 
    if(diff_codomain != set2->len) {
        result->len = (int)(false);
        return result;
    }
    
    //checks if no two first elements are the same
    for (unsigned i = 0; i < rel->len; i += 2) {
        for (unsigned j = i + 2; j < rel->len; j += 2) {
            if(strcmp(rel->elements[i],rel->elements[j]) == 0) {
                result->len = (int)(false);
                return result;
            }
        }
    }
    result->len = (int)(true);
    return result;
}

/**
 * @brief Finds if relation is bijective
 * 
 * @param args array of arguments, args[0] is the relation, args[1,2]
 *             are the 2 sets
 * @return Set* of type bool, where the bool value is saved
 */
Set * relation_bijective(Set * args[]) {
    
    Set * rel = args[0];
    Set * set1 = args[1];
    Set * set2 = args[2];
    Set * result = malloc(sizeof(int));
    if(result == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return NULL;
    }
    result->type = BOL;

    if(!is_rel_el_in_set(rel, set1, false) || !is_rel_el_in_set(rel, set2, true)) {
        fprintf(stderr, "Error, an element in the relation is not in a set");
        return NULL;
    }
    
    if(set1->len != set2->len || rel->len/2 != set1->len) {
        result->len = (int)(false);
        return result;
    }
    for (unsigned i = 0; i < rel->len; i += 2) {
        for (unsigned j = i + 2; j < rel->len; j += 2) {
            if(strcmp(rel->elements[i],rel->elements[j]) == 0
            || strcmp(rel->elements[i + 1],rel->elements[j + 1]) == 0){
                result->len = (int)(false);
                return result;
            }
        }
    }
    result->len = (int)(true);
    return result;
}

/**
 * @brief Finds and returns the reflexive closure of a relation
 * 
 * @param args array of arguments, args[0] is the relation
 * @return Set* containing the closure
 */
Set * closure_ref(Set * args[]) {
    
    Set * rel = args[0];
    Set * result = malloc(sizeof(int));
    if(result == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return NULL;
    }
    result->type = REL;
    result->len = 0;
    result->elements = NULL;
    
    //check if is not already reflexive
    Set * is_refl_set = relation_reflexive(&rel);
    if(is_refl_set->len == true) {
        free(is_refl_set);
        return rel;
    }
    free(is_refl_set);

    for(unsigned i = 0; i < rel->len; i++) {
        if(set_add(result,rel->elements[i]) == 1)
            return NULL;
    }
    bool first_refl;
    bool second_refl;
    for(unsigned i = 0; i < result->len; i += 2) {
        first_refl = false;
        second_refl = false;
        for(unsigned j = 0; j < result->len; j += 2) {
            //checks if there is a reflexive pair for the first element
            if(strcmp(result->elements[i],result->elements[j]) == 0
            &&strcmp(result->elements[i],result->elements[j + 1]) == 0) {
                first_refl = true;
            }
            //checks if there is a reflexive pair for the second element
            if(strcmp(result->elements[i + 1],result->elements[j]) == 0
            &&strcmp(result->elements[i + 1],result->elements[j + 1]) == 0) {
                second_refl = true;
            }
            if(first_refl == true && second_refl == true) {
                break;
            }
        }
        if(first_refl == false) {
            if(set_add(result, result->elements[i]) == 1)
                return NULL;
            if(set_add(result, result->elements[i]) == 1)
                return NULL;
        }
        if(second_refl == false) {
            if(set_add(result, result->elements[i + 1]) == 1)
                return NULL;
            if(set_add(result, result->elements[i + 1]) == 1)
                return NULL;
        }
    }
    return result;
}

/**
 * @brief Finds and returns the symmetric closure of a relation
 * 
 * @param args array of arguments, args[0] is the relation
 * @return Set* containing the closure
 */
Set * closure_sym(Set * args[]) {

    Set * rel = args[0];
    Set * result = malloc(sizeof(int));
    if(result == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return NULL;
    }
    result->type = REL;
    result->len = 0;
    result->elements = NULL;

    //check if is not already symmetric
    Set * is_symm_set = relation_symmetric(&rel);
    if(is_symm_set->len == true) {
        free(is_symm_set);
        return rel;
    }
    free(is_symm_set);
    
    for(unsigned i = 0; i < rel->len; i++) {
        if(set_add(result,rel->elements[i]) == 1)
            return NULL;
    }

    bool is_symm;
    for(unsigned i = 0; i < result->len; i += 2) {
        is_symm = false;
        for(unsigned j = 0; j < result->len; j += 2) {
            if(strcmp(result->elements[i],result->elements[j + 1]) == 0
            && strcmp(result->elements[i + 1],result->elements[j]) == 0) {
                is_symm = true;
                break;
            }
        }
        if(!is_symm) {
            if(set_add(result, result->elements[i + 1]) == 1)
                return NULL;
            if(set_add(result, result->elements[i]) == 1)
                return NULL;
        }
    }
    return result;    
}

/**
 * @brief Finds and returns the transitive closure of a relation
 * 
 * @param args array of arguments, args[0] is the relation
 * @return Set* containing the closure
 */
Set * closure_trans(Set * args[]) {
    
    Set * rel = args[0];
    Set * result = malloc(sizeof(int));
    if(result == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return NULL;
    }
    result->type = REL;
    result->len = 0;
    result->elements = NULL;

    //check if is not already reflexive
    Set * is_trans_Set = relation_transitive(&rel);
    if(is_trans_Set->len == true) {
        free(is_trans_Set);
        return rel;
    }
    free(is_trans_Set);

    for(unsigned i = 0; i < rel->len; i++) {
        if(set_add(result,rel->elements[i]) == 1)
            return NULL;
    }

    for(unsigned i = 0; i < result->len; i += 2) {
        bool is_tran = false;
        if(strcmp(result->elements[i], result->elements[i + 1]) == 0) {
            continue;
        }
        //tries to find if there is a pair j with the same first element 
        //as the second element of the pair i, if there isn't, 
        //pair i is transitive
        for(unsigned j = 0; j < result->len; j += 2) {
            if(strcmp(result->elements[i + 1],result->elements[j]) == 0) {
                //tries to find pair k, that is the transitive closure of 
                //pairs i and j
                for(unsigned k = 0; k < result->len; k += 2) {
                    if(strcmp(result->elements[i],result->elements[k]) == 0 
                    && strcmp(result->elements[j + 1],result->elements[k + 1]) == 0){
                        is_tran = true;
                        break;    
                    }
                }
                if(!is_tran) {
                    if(set_add(result,result->elements[i]) == 1)
                        return NULL;
                    if(set_add(result,result->elements[j + 1]) == 1)
                        return NULL;
                }
            }
        }
    }
    return result;
}

/*
 * @brief Returns if set is empty
 *
 * @param set1 1.set
 * @param uni Universe
 */
Set * set_empty(Set * args[]) 
{
    Set *set1 = args[0];
    Set *result = malloc(sizeof(int));
    
    if ((int)set1->len <= 0)
    {
        printf("set is empty"); // TODO: delete this
        set_create(result, BOL, 1);
    }
    else
    {
        printf("set is not empty"); // TODO: delete this
        set_create(result, BOL, 0);
    }
    
    return result;
}

/**
 * @brief Returns length of set
 *
 * @param set1 1.set
 */
Set * set_card(Set * args[]) 
{
    Set *set1 = args[0];
    Set *result = malloc(sizeof(int));
    set_create(result, num, set1->len); 
    return result;
}

/**
 * @brief Determines if set1 is sub-set or equal to set2
 *
 * @param set1 1.set
 * @param set2 2.set
 */
Set * set_subseteq(Set * args[]) 
{
    Set *set1 = args[0];
    Set *set2 = args[1];
    Set *result = malloc(sizeof(int));
    set_init(result, BOL, NULL, 0);

    // if element is found in intersection, dont add it into set
    // if found is false, it should be added cause it's not in intersection of 2 sets and it's in set 1
    for (int i = 0; i < (int)set1->len; i++)
    {
        bool found = false;

        for (int j = 0; j < (int)set2->len; j++)
        {
            if (set1->elements[i] == set2->elements[j])
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            printf("is not subset or equal"); // TODO: delete this
            result->len = 0;
            return result;
        }
    }

    printf("is subset or equal"); // TODO: delete this
    result->len = 1;
    return result;
}

/**
 * @brief Determines if set1 is sub-set of set2
 *
 * @param set1 1.set
 * @param set2 2.set
 * @param uni Universe containing elements of the two sets
 */
Set * set_subset(Set * args[]) 
{
    Set *set1 = args[0];
    Set *set2 = args[1];
    Set *result = malloc(sizeof(int));
    set_init(result, BOL, NULL, 0);

    int same_elements = 0;

    // if element is found in intersection, dont add it into set
    // if found is false, it should be added cause it's not in intersection of 2 sets and it's in set 1
    for (int i = 0; i < (int)set1->len; i++)
    {
        bool found = false;

        for (int j = 0; j < (int)set2->len; j++)
        {
            if (set1->elements[i] == set2->elements[j])
            {
                found = true;
                same_elements++;
                break;
            }
        }
        if (!found)
        {
            printf("is not subset"); // TODO: delete this
            result->len = 0;
            return result;
        }
    }

    if (same_elements == (int)set2->len)
    {
        // sets are equal
        // (all elements is set1 is in set2 and they have same length)
        printf("is not subset"); // TODO: delete this
        result->len = 0;
    }
    else
    {
        printf("is subset"); // TODO: delete this
        result->len = 1;
    }
    return result;
}

/**
 * @brief Determines if set1 is equal to set2
 *
 * @param set1 1.set
 * @param set2 2.set
 * @param uni Universe containing elements of the two sets
 */
Set * set_equal(Set * args[]) 
{
    Set *set1 = args[0];
    Set *set2 = args[1];
    Set *result = malloc(sizeof(int));
    set_init(result, BOL, NULL, 0);


    int same_elements = 0;

    // if element is found in intersection, dont add it into set
    // if found is false, it should be added cause it's not in intersection of 2 sets and it's in set 1
    for (int i = 0; i < (int)set1->len; i++)
    {
        bool found = false;

        for (int j = 0; j < (int)set2->len; j++)
        {
            if (set1->elements[i] == set2->elements[j])
            {
                found = true;
                same_elements++;
                break;
            }
        }
        if (!found)
        {
            printf("is not equal"); // TODO: delete this
            result->len = 0;
            return result;
        }
    }

    if (same_elements == (int)set2->len)
    {
        // sets are equal
        // (all elements is set1 is in set2 and they have same length)
        printf("is equal"); // TODO: delete this
        result->len = 1;
    }
    else
    {
        printf("is not equal"); // TODO: delete this
        result->len = 0;
    }
    return result;
}

