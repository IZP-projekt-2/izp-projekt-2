#ifndef SET_H
#define SET_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

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

Set *univerzum; /** @todo change to static once its in a single file */

bool is_constant_type(SetType type);

Set *set_ctor(SetType type, char *init_elements[], int init_len);
Set *const_set_ctor(SetType type, int value);
char *set_get_element(Set *set, char element[]);
bool set_contains_relation(Set *set, char *first, char *second);
int set_add_elements(Set *set, char *elements[], int len);
void set_print(Set *, FILE *where);
void set_dtor(Set *set);

#endif /* SET_H */