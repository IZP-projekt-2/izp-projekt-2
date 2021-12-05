#include "parsing.h"

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