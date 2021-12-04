#include "set.h"

/**
 * Checks if given type is of a "constant" set.
 *
 * @param type Type to be checked.
 * @return Bool.
 */

bool is_constant_type(SetType type)
{
    return type == num || type == bol;
}

/**
 * Creates a set of a given type with initial elements on a heap. On error
 * prints to a stderr and returns NULL. Cannot create "constant" sets - for that
 * use const_set_ctor.
 *
 * @param type Type of a set.
 * @param init_elements List of strings (elements) or NULL.
 * @param init_len Length of init_elements.
 * @return Pointer to set on a heap.
 */
Set *set_ctor(SetType type, char *init_elements[], int init_len)
{
    if (is_constant_type(type))
    {
        fprintf(stderr,
                "Tried to create \"costant\" set using set_ctor. \
                Please use const_set_ctor instead.\n");

        return NULL;
    }

    else if (type != uni && univerzum == NULL)
    {
        fprintf(stderr, "Creating set without univerzum.\n");
        return NULL;
    }

    Set *heap_pointer = malloc(sizeof(Set));

    if (heap_pointer == NULL)
    {
        fprintf(stderr, "Malloc failed.\n");
        return NULL;
    }

    heap_pointer->len = 0;
    heap_pointer->type = type;
    heap_pointer->elements = NULL;

    if (init_len && set_add_elements(heap_pointer, init_elements, init_len))
    {
        free(heap_pointer);
        return NULL;
    }

    return heap_pointer;
}

/**
 * Creates "constant" set on a heap with a given value.
 *
 * @param type Type of a set.
 * @param value Sets value.
 * @return Pointer to heap.
 */
Set *const_set_ctor(SetType type, int value)
{
    if (!is_constant_type(type))
    {
        fprintf(stderr, "Tried to create constant set of non-constant type\n");
        return NULL;
    }

    Set *heap_pointer = malloc(sizeof(Set));

    if (heap_pointer == NULL)
    {
        fprintf(stderr, "Malloc failed.\n");
        return NULL;
    }

    heap_pointer->elements = NULL;
    heap_pointer->type = type;
    heap_pointer->len = value;

    return heap_pointer;
}

/**
 * Tries to find element in a set (univerzum).
 *
 * @param set Set to be searched.
 * @param element String representing searched element.
 * @return Pointer to a found element stored in univerzum or NULL if element
 * is not contained.
 */
char *set_get_element(Set *set, char element[])
{
    if (set->type == uni)
        for (int i = 0; i < set->len; i++)
            if (!strcmp(set->elements[i], element))
                return set->elements[i];

    if (set->type == els)
    {
        char *pointer = set_get_element(univerzum, element);

        if (pointer == NULL)
            return NULL;

        // Should be returned only if its contained in a set
        for (int i = 0; i < set->len; i++)
            if (set->elements[i] == pointer)
                return set->elements[i];
    }

    return NULL;
}

bool set_contains_relation(Set *set, char *first, char *second)
{
    if (set->type != rel)
    {
        fprintf(stderr, "Can check for relation presence only \
                        in a set of relations.\n");
        return 1;
    }

    for (int i = 1; i < set->len; i += 2)
        if (set->elements[i - 1] == first && set->elements[i] == second)
            return true;

    return false;
}

int set_add_elements(Set *set, char *elements[], int len)
{
    if (is_constant_type(set->type))
    {
        fprintf(stderr, "Cannot add elements to \"constant\" sets.\n");
        return 1;
    }

    if (set->type == rel && len % 2)
    {
        fprintf(stderr,
                "Cannot add odd number of elements to a set of relations.\n");
        return 1;
    }

    int new_len = set->len + len;
    char **new_el_pointer = realloc(set->elements,
                                    sizeof(char **) * (new_len + 1));

    if (new_el_pointer == NULL)
    {
        fprintf(stderr, "Reallocating memory for new set elements failed.\n");
        return 1;
    }

    set->elements = new_el_pointer;

    for (int index = 0; index < len; index++)
    {
        char *element = elements[index];

        if (set->type != uni)
        {
            element = set_get_element(univerzum, element);

            if (element == NULL)
            {
                fprintf(stderr, "Element '%s' isn't defined in univerzum.\n",
                        elements[index]);
                return 1;
            }
        }

        if (set->type == els || set->type == uni)
            if (set_get_element(set, element) != NULL)
            {
                fprintf(stderr, "Element is already contained.\n");
                return 1;
            }

        if (set->type == rel && (index % 2))
            if (set_contains_relation(set,
                                      set->elements[set->len - 1], element))
            {
                fprintf(stderr, "Duplicate relation definition.\n");
                return 1;
            }

        if (set->type == uni)
        {
            char *str_heap_pointer = malloc(strlen(element) + 1);

            if (str_heap_pointer == NULL)
            {
                fprintf(stderr, "Allocating memory to store new univerzum \
                                element failed.\n");
                return 1;
            }

            strcpy(str_heap_pointer, element);
            element = str_heap_pointer;
        }

        set->elements[(set->len++)] = element;
    }

    return 0;
}

void set_print(Set *set, FILE *where)
{
    if (set->type == num)
        fprintf(where, "%d", set->len);

    else if (set->type == bol)
        fprintf(where, "(%s)", set->len ? "true" : "false");

    else
    {
        fprintf(where, "%c ", set->type);
        if (set->type == rel)
            for (int i = 0; i < set->len; i += 2)
                fprintf(where, "(%s %s) ",
                        set->elements[i],
                        set->elements[i + 1]);
        else
            for (int i = 0; i < set->len; i++)
                fprintf(where, "%s ", set->elements[i]);
    }

    fprintf(where, "\n");
}

void set_dtor(Set *set)
{
    if (set != NULL)
    {
        if (set->elements != NULL)
            free(set->elements);

        free(set);
    }
}