#include "lines.h"
#include <assert.h>

int main()
{
    lines_init();

    char *uni_elements[] = {
        "abc",
        "def",
        "ghi",
        "foo",
        "bar",
    };

    char *s1els[] = {
        "abc",
        "def",
    };

    char *s2els[] = {
        "def",
        "foo",
        "bar",
    };

    black_listed = set_ctor(uni);
    univerzum = set_ctor(uni);
    Set *set1 = set_ctor(els);
    Set *set2 = set_ctor(els);

    set_add_elements(univerzum, uni_elements, 5);
    set_add_elements(set1, s1els, 2);
    set_add_elements(set2, s2els, 3);

    lines[1] = line_ctor(def_univerzum);
    lines[2] = line_ctor(def_set);
    lines[3] = line_ctor(def_set);
    lines[4] = line_ctor(exe_command);

    lines[1]->related_set = univerzum;
    lines[2]->related_set = set1;
    lines[3]->related_set = set2;

    assert(lines[2]->related_set->len == 2);
    assert(lines[3]->related_set->len == 3);

    Arglist com_args = {elements, elements, non};
    // unsigned args[] = {2, 3};

    lines[4]->command = &intersect;
    lines[4]->expected_args = com_args;
    // lines[4]->expected_args[0] = elements;
    // lines[4]->expected_args[1] = elements;
    // lines[4]->expected_args[2] = non;

    lines[4]->args[0] = 1;
    lines[4]->args[1] = 3;

    Set *res = line_exec(lines[4]);

    if (res != NULL)
        set_print(res, stderr);

    lines_dtor();
    return 0;
}