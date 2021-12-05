#ifndef COMMANDS_H
#define COMMANDS_H

#include "../set/set.h"
#define MAX_COMMAND_ARGS 3

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
    {"subseteq", NULL, {elements, elements, non}},
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
    {"select", NULL, {elements, number, non}},
    {NULL, NULL, {non}},
};

#endif /* COMMANDS_H */