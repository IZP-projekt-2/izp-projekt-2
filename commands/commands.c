#include "commands.h"

Set *intersect(Set *args[])
{
    Set *set1 = args[0];
    Set *set2 = args[1];
    Set *intersect_set = set_ctor(els, NULL, 0);

    for (int i = 0; i < set1->len; i++)
    {
        char *element = set_get_element(set2, set1->elements[i]);
        if (element != NULL)
            set_add_elements(intersect_set, &element, 1);
    }

    return intersect_set;
}

// bool compare_strings(char *str1, char *str2)
// {
//     for (int i = 0; str1[i] != '\0' || str2[i] != '\0'; i++)
//     {
//         if (str1[i] != str2[i])
//         {
//             return false;
//         }
//     }
//     return true;
// }

// /**
//  * @brief Returns pointer to difference of 2 sets
//  *
//  * @param set1 1.set
//  * @param set2 2.set
//  * @param uni Universe containing elements of the two sets
//  */
// Set *set_difference(Set *set1, Set *set2, Univerzum *uni, Set *result)
// {
//     if (result == NULL)
//     {
//         set_init(&result, 'S', NULL, 0);
//     }

//     // if element is found in intersection, dont add it into set
//     // if found is false, it should be added cause it's not in intersection of 2 sets and it's in set 1
//     for (int i = 0; i < (int)set1->len; i++)
//     {
//         bool found = false;

//         for (int j = 0; j < (int)set2->len; j++)
//         {
//             if (set1->elements[i] == set2->elements[j])
//             {
//                 found = true;
//                 break;
//             }
//         }
//         if (!found)
//         {
//             set_add(&result, uni, set1->elements[i]);
//         }
//     }

//     set_print(&result);
//     printf("\n");

//     return &result;
// }

// /**
//  * @brief Returns pointer to complement of 1.set
//  *
//  * @param set1 1.set
//  * @param uni Universe
//  */
// Set *set_complement(Set *set1, Univerzum *uni, Set *result)
// {
//     if (result == NULL)
//     {
//         set_init(&result, 'S', NULL, 0);
//     }

//     // If element from 1.set IS NOT found in univerzum,
//     // it is added to result
//     for (int i = 0; i < (int)uni->len; i++)
//     {
//         bool found = false;

//         for (int j = 0; j < (int)set1->len; j++)
//         {
//             if (uni->elements[i] == set1->elements[j])
//             {
//                 found = true;
//                 break;
//             }
//         }
//         if (!found)
//         {
//             set_add(&result, uni, uni->elements[i]);
//         }
//     }

//     set_print(&result);
//     printf("\n");

//     return &result;
// }

// /**
//  * @brief Returns if set is empty
//  *
//  * @param set1 1.set
//  * @param uni Universe
//  */
// Set *set_empty(Set *set1, Univerzum *uni, Set *result)
// {

//     if ((int)set1->len <= 0)
//     {
//         printf("set is empty"); // TODO: delete this
//         set_init(&result, bol, NULL, 0);
//     }
//     else
//     {
//         printf("set is not empty"); // TODO: delete this
//         set_init(&result, bol, NULL, 1);
//     }

//     return &result;
// }

// /**
//  * @brief Returns length of set
//  *
//  * @param set1 1.set
//  * @param uni Universe
//  */
// Set *set_card(Set *set1, Univerzum *uni, Set *result)
// {
//     printf("lenght of set1 is: %d", set1->len); // TODO: delete this

//     set_init(&result, num, NULL, set1->len);
//     return &result;
// }

// /**
//  * @brief Determines if set1 is sub-set or equal to set2
//  *
//  * @param set1 1.set
//  * @param set2 2.set
//  * @param uni Universe containing elements of the two sets
//  */
// Set *set_subseteq(Set *set1, Set *set2, Univerzum *uni, Set *result)
// {

//     // if element is found in intersection, dont add it into set
//     // if found is false, it should be added cause it's not in intersection of 2 sets and it's in set 1
//     for (int i = 0; i < (int)set1->len; i++)
//     {
//         bool found = false;

//         for (int j = 0; j < (int)set2->len; j++)
//         {
//             if (set1->elements[i] == set2->elements[j])
//             {
//                 found = true;
//                 break;
//             }
//         }
//         if (!found)
//         {
//             printf("is not subset or equal"); // TODO: delete this
//             set_init(&result, bol, NULL, 0);
//             return &result;
//         }
//     }

//     printf("is subset or equal"); // TODO: delete this
//     set_init(&result, bol, NULL, 1);
//     return &result;
// }

// /**
//  * @brief Determines if set1 is sub-set of set2
//  *
//  * @param set1 1.set
//  * @param set2 2.set
//  * @param uni Universe containing elements of the two sets
//  */
// Set *set_subset(Set *set1, Set *set2, Univerzum *uni, Set *result)
// {
//     int same_elements = 0;

//     // if element is found in intersection, dont add it into set
//     // if found is false, it should be added cause it's not in intersection of 2 sets and it's in set 1
//     for (int i = 0; i < (int)set1->len; i++)
//     {
//         bool found = false;

//         for (int j = 0; j < (int)set2->len; j++)
//         {
//             if (set1->elements[i] == set2->elements[j])
//             {
//                 found = true;
//                 same_elements++;
//                 break;
//             }
//         }
//         if (!found)
//         {
//             printf("is not subset"); // TODO: delete this
//             set_init(&result, bol, NULL, 0);
//             return &result;
//         }
//     }

//     if (same_elements == (int)set2->len)
//     {
//         // sets are equal
//         // (all elements is set1 is in set2 and they have same length)
//         printf("is not subset"); // TODO: delete this
//         set_init(&result, bol, NULL, 0);
//     }
//     else
//     {
//         printf("is subset"); // TODO: delete this
//         set_init(&result, bol, NULL, 1);
//     }
//     return &result;
// }

// /**
//  * @brief Determines if set1 is equal to set2
//  *
//  * @param set1 1.set
//  * @param set2 2.set
//  * @param uni Universe containing elements of the two sets
//  */
// Set *set_equal(Set *set1, Set *set2, Univerzum *uni, Set *result)
// {
//     int same_elements = 0;

//     // if element is found in intersection, dont add it into set
//     // if found is false, it should be added cause it's not in intersection of 2 sets and it's in set 1
//     for (int i = 0; i < (int)set1->len; i++)
//     {
//         bool found = false;

//         for (int j = 0; j < (int)set2->len; j++)
//         {
//             if (set1->elements[i] == set2->elements[j])
//             {
//                 found = true;
//                 same_elements++;
//                 break;
//             }
//         }
//         if (!found)
//         {
//             printf("is not equal"); // TODO: delete this
//             set_init(&result, bol, NULL, 0);
//             return &result;
//         }
//     }

//     if (same_elements == (int)set2->len)
//     {
//         // sets are equal
//         // (all elements is set1 is in set2 and they have same length)
//         printf("is equal"); // TODO: delete this
//         set_init(&result, bol, NULL, 1);
//     }
//     else
//     {
//         printf("is not equal"); // TODO: delete this
//         set_init(&result, bol, NULL, 0);
//     }
//     return &result;
// }