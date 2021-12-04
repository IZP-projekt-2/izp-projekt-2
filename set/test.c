#include "set.h"
#include <assert.h>

void test_uni()
{
    char *elements[] = {
        "abc",
        "def",
        "ghi",
    };

    char *ext_els[] = {
        "foo",
        "bar",
    };

    univerzum = set_ctor(uni, elements, 3);

    assert(univerzum != NULL);
    assert(univerzum->type == uni);
    assert(univerzum->len == 3);

    char *abc = set_get_element(univerzum, "abc");
    char *def = set_get_element(univerzum, "def");
    char *ghi = set_get_element(univerzum, "ghi");
    char *foo = set_get_element(univerzum, "foo");

    assert(abc != NULL);
    assert(def != NULL);
    assert(ghi != NULL);

    assert(!strcmp(abc, "abc"));
    assert(!strcmp(def, "def"));
    assert(!strcmp(ghi, "ghi"));

    assert(foo == NULL);

    assert(!set_add_elements(univerzum, ext_els, 2));
    assert(univerzum->len == 5);

    abc = set_get_element(univerzum, "abc");
    def = set_get_element(univerzum, "def");
    ghi = set_get_element(univerzum, "ghi");
    foo = set_get_element(univerzum, "foo");

    assert(abc != NULL);
    assert(def != NULL);
    assert(ghi != NULL);
    assert(foo != NULL);

    assert(!strcmp(abc, "abc"));
    assert(!strcmp(def, "def"));
    assert(!strcmp(ghi, "ghi"));
    assert(!strcmp(foo, "foo"));

    assert(set_add_elements(univerzum, ext_els, 2));
}

void test_elements()
{
    char *uni_elements[] = {
        "abc",
        "def",
        "ghi",
        "foo",
        "xyz",
    };

    char *succ_els[] = {
        "abc",
        "foo",
        "def",
    };

    char *ext_succ[] = {
        "xyz",
        "ghi",
    };

    char *ext_fail[] = {
        "bar",
        "ghi",
    };

    char *f1_els[] = {
        "abc",
        "foo",
        "def",
        "bar",
    };

    char *f2_els[] = {
        "abc",
        "foo",
        "def",
        "foo",
    };

    univerzum = set_ctor(uni, uni_elements, 5);
    Set *test_set = set_ctor(els, succ_els, 3);
    Set *f1_set = set_ctor(els, f1_els, 4);
    Set *f2_set = set_ctor(els, f2_els, 4);

    assert(test_set != NULL);
    assert(f1_set == NULL);
    assert(f2_set == NULL);

    char *abc = set_get_element(test_set, "abc");
    char *abc_uni = set_get_element(univerzum, "abc");
    char *def = set_get_element(test_set, "def");
    char *ghi = set_get_element(test_set, "ghi");
    char *foo = set_get_element(test_set, "foo");

    assert(abc_uni != NULL);
    assert(abc != NULL);
    assert(def != NULL);
    assert(ghi == NULL);
    assert(foo != NULL);

    assert(!strcmp(abc, "abc"));
    assert(!strcmp(def, "def"));
    assert(!strcmp(foo, "foo"));

    assert(abc == abc_uni);

    assert(!set_add_elements(test_set, ext_succ, 2));
    assert(set_add_elements(test_set, ext_fail, 3));

    abc = set_get_element(test_set, "abc");
    def = set_get_element(test_set, "def");
    ghi = set_get_element(test_set, "ghi");
    foo = set_get_element(test_set, "foo");

    assert(abc != NULL);
    assert(def != NULL);
    assert(ghi != NULL);
    assert(foo != NULL);

    assert(abc == abc_uni);
}

void test_rels()
{
    char *uni_elements[] = {
        "abc",
        "def",
        "ghi",
        "foo",
        "xyz",
    };

    char *succ_els[] = {
        "abc",
        "foo",
        "def",
        "foo",
        "abc",
        "def",
    };

    char *f1_els[] = {
        "abc",
        "foo",
        "def",
        "foo",
        "abc",
        "foo",
    };

    char *f2_els[] = {
        "abc",
        "foo",
        "def",
        "foo",
        "abc",
    };

    char *f3_els[] = {
        "abc",
        "doesntexist",
    };

    univerzum = set_ctor(uni, uni_elements, 5);
    Set *test_set = set_ctor(rel, succ_els, 6);
    Set *f1_set = set_ctor(rel, f1_els, 6);
    Set *f2_set = set_ctor(rel, f2_els, 5);
    Set *f3_set = set_ctor(rel, f3_els, 2);

    set_print(univerzum, stdout);
    set_print(test_set, stdout);

    assert(test_set != NULL);
    assert(f1_set == NULL);
    assert(f2_set == NULL);
    assert(f3_set == NULL);
}

void test_constant_elements()
{
    Set *num_val = const_set_ctor(num, 42);
    Set *false_val = const_set_ctor(bol, false);
    Set *true_val = const_set_ctor(bol, true);
    Set *fail_val = const_set_ctor(els, 43);

    assert(num_val->len == 42);
    assert(false_val->len == false);
    assert(true_val->len == true);
    assert(fail_val == NULL);

    set_print(num_val, stdout);
    set_print(false_val, stdout);
    set_print(true_val, stdout);
}

int main()
{
    test_uni();
    test_elements();
    test_rels();
    test_constant_elements();
}
