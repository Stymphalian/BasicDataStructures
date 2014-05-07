
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "List.h"

List* linked_list_create(int typeSize, list_elem_destroy eDestroy);
void linked_list_destroy(List* list);

#endif
