#include "parsing.h"

int main(int argc, char **argv)
{
    black_listed = set_ctor(uni);

    FILE *input = open_input_file(argc, argv);
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

    lines_dtor();
}
