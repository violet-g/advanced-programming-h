#ifndef _MLIST_H_
#define _MLIST_H_

#include "mentry.h"

typedef struct mlist MList;

extern int ml_verbose;  /* if true, prints diagnostics on stderr */

/* ml_create - created a new mailing list */
MList *ml_create(void);

/* ml_add - adds a new MEntry to the list;
 * returns 1 if successful, 0 if error (malloc)
 * returns 1 if it is a duplicate */
int ml_add(MList **ml, MEntry *me);

/* ml_lookup - looks for MEntry in the list, returns matching entry or NULL */
MEntry *ml_lookup(MList *ml, MEntry *me);

/* ml_destroy - destroy the mailing list */
void ml_destroy(MList *ml);

#endif
