#include "loading.h"

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