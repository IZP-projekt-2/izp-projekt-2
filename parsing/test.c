#include "parsing.h"

// "null" element of array - used to stop iteration.

int main(int argc, char **argv)
{
    FILE *input = open_input_file(argc, argv);
    parse_file(input);
    fclose(input);

    for (int i = 1;; i++)
    {
        Line *line = lines[i];
        if (line == NULL)
            break;

        Set *set = line_get_set(line);

        if (set != NULL)
            set_print(set, stdout);
    }

    lines_dtor();
}
