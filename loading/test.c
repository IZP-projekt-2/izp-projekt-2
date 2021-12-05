#include "loading.h"
#include <assert.h>

int main(int argc, char **argv)
{
    FILE *input = open_input_file(argc, argv);

    univerzum = set_ctor(uni);
    black_listed = set_ctor(uni);

    Set *set1 = set_ctor(els);
    Set *set_empty = set_ctor(els);
    Set *set2 = set_ctor(els);
    Set *rel1 = set_ctor(rel);
    Set *rel_empty = set_ctor(rel);
    Set *rel2 = set_ctor(rel);
    Set *rel3 = set_ctor(rel);

    assert(input != NULL);

    assert(!load_set_elements(univerzum, input));
    assert(univerzum->len == 27);

    assert(!load_set_elements(set1, input));
    assert(!load_set_elements(set_empty, input));
    assert(set_empty->len == 0);
    assert(!load_set_elements(set2, input));

    assert(!load_relations(rel1, input));
    assert(!load_relations(rel_empty, input));
    assert(rel_empty->len == 0);
    assert(!load_relations(rel2, input));
    assert(load_relations(rel3, input));

    fclose(input);
}