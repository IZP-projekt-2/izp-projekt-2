#ifndef LINES_H
#define LINES_H

#include "../set/set.h"
#include "../commands/commands.h"

#define MAX_LINES 1000

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

#endif /* LINES_H */