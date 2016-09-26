#include <stdlib.h>
#include <string.h>

#include "phonebook_opt.h"

int hash(char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

/* FILL YOUR OWN IMPLEMENTATION HERE!! */
entry *findName(char lastName[], entry *pHead)
{
    while(pHead != NULL)
    {
        if (strcmp(lastName, pHead->lastName) == 0)
            return pHead;
        pHead = pHead->pNext;
    }
    return NULL;
}

entry *append(char lastName[], entry *e)
{
    e->pNext = (entry *) malloc(sizeof(entry));
    strcpy(e->pNext->lastName, lastName);
    e = e->pNext;
    e->pNext = NULL;

    return e;
}
