#include "lines.h"

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

    for (int i = 0; i < MAX_COMMAND_ARGS + 1; i++)
        heap_pointer->args[i] = 0;

    for (int i = 0; i < MAX_COMMAND_ARGS; i++)
        heap_pointer->expected_args[i] = non;

    return heap_pointer;
}

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

    if (param && result->type != bol)
    {
        fprintf(stderr, "Too many arguments. \
                        Non-bool returning commands don't support param\n");
        return NULL;
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