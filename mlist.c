#include "mlist.h"
#include <stdio.h>
#include <stdlib.h>

#define BUCKETSIZE 20
#define BUCKETNUM 10

MList *ml_expand(MList *ml);

int ml_verbose = 0;

typedef struct bucket {
  MEntry *entries[BUCKETSIZE];
    int num_of_entries;
} Bucket;

struct mlist {
  Bucket *buckets;
  int num_of_buckets;
};

MList *ml_create(void) {

  if (ml_verbose) {
    fprintf(stderr, "mlist: creating mailing list\n");
  }
  
  /* allocate memory for new mailing list */
  MList *mlist = (MList *)malloc(sizeof(Bucket)*BUCKETNUM);
  
  if (mlist == NULL) {
    puts("Could not allocate memory for the initial mailing list.");
    return 0;
  }

  /* assign props to the mailing list */
  mlist -> num_of_buckets = BUCKETNUM;
  mlist -> buckets = (Bucket *)malloc(sizeof(Bucket)*BUCKETNUM);
  int bucketNum;
  for (bucketNum = 0; bucketNum < mlist -> num_of_buckets; bucketNum++) {
    /* assign initial value of 0 entries in each bucket */
    mlist -> buckets[bucketNum].num_of_entries = 0;
  }
  return mlist;
}

int ml_add(MList **ml, MEntry *me) {

  if (ml_verbose) {
    fprintf(stderr, "mlist: ml_add() entered\n");
  }

  MList *p = *ml;
  
  /* find the bucket where the the entry should be stored in */
  unsigned long key = me_hash(me, p -> num_of_buckets);
  Bucket *bucket = (p -> buckets) + key;
  if ((bucket -> num_of_entries) < BUCKETSIZE) {
    bucket -> entries[bucket -> num_of_entries++] = me;
    return 1;
  }

  /* if there is not enough space in the list, expand it */
  MList *expanded_ml = ml_expand(p);
  if (expanded_ml == NULL) {
    puts("Could not allocate memory for a larger table.");
    return 0;
  }

  /* rehash and store entry in the larger table */
  unsigned long new_key = me_hash(me, expanded_ml -> num_of_buckets);
  Bucket *new_bucket = expanded_ml -> buckets + new_key;
  new_bucket -> entries[new_bucket -> num_of_entries++] = me;
  
  /* move table pointer to the larger hash table */
  *ml = expanded_ml;
  return 1;
}

MEntry *ml_lookup(MList *ml, MEntry *me) {

  if (ml_verbose) {
    fprintf(stderr, "mlist: ml_lookup() entered\n");
  }
  
  /* find the bucket where the entry should be (if in the hash table) */
  unsigned long key = me_hash(me, ml -> num_of_buckets);
  Bucket *bucket = ml -> buckets + key;

  /* loop through bucket entries and compare with the given one */
  int entryNum;
  for (entryNum = 0; entryNum < bucket -> num_of_entries; entryNum++) {
    MEntry *entry = bucket -> entries[entryNum];
    /* if the entry is in the table, return it */
    if (me_compare(entry, me) == 0) {
      return entry;
    }
  }
  return NULL;
}

void ml_destroy(MList *ml) {

  if (ml_verbose) {
    fprintf(stderr, "mlist: ml_destroy() entered\n");
  }

  int bucketNum, entryNum;

  /* loop through each bucket and destroy all entries */
  for (bucketNum = 0; bucketNum < ml -> num_of_buckets; bucketNum++) {

    /* find next bucket by moving the pointer */
    Bucket *bucket = ml -> buckets + bucketNum;

    for (entryNum = 0; entryNum < (bucket -> num_of_entries); entryNum++) {
      /* find next entry, again using pointer arithmetic */  
      MEntry *me = *(bucket -> entries + entryNum);

      me_destroy(me);
    }
  }
  free(ml -> buckets);
  free(ml);
  
}

/* custom function used to directly add entry to hash table */
void ml_add_to_table(MList *ml, MEntry *me, unsigned long hash) {
  Bucket *bucket = ml -> buckets + hash;
  *(bucket -> entries + (bucket -> num_of_entries)) = me;
  bucket -> num_of_entries += 1;
}

/* function used to resize the current mailing list */
MList *ml_expand(MList *ml) {

  if (ml_verbose) {
    puts("Expanding table.");
  }

  /* create a new hash table twice as big as the previous one */
  MList *exp_mlist = (MList *)malloc(sizeof(MList));
  
  /* assign its props */
  exp_mlist -> buckets = (Bucket *) malloc(sizeof(Bucket) * ((ml -> num_of_buckets)*2));
  exp_mlist -> num_of_buckets = (ml -> num_of_buckets)*2;
  int bucketNum;
  for (bucketNum = 0; bucketNum < exp_mlist -> num_of_buckets; bucketNum++) {
    Bucket *bucket = exp_mlist -> buckets + bucketNum;
    bucket -> num_of_entries = 0;
  }
  
  /* loop through all buckets and entries from the smaller hash table */
  int i, j;
  for (i = 0; i < ml -> num_of_buckets; i++) {
    Bucket *bucket = ml -> buckets + i;
    for (j = 0; j < (bucket -> num_of_entries); j++) {
      MEntry *entry = *(bucket -> entries + j);
      
      /* rehash entry and store it in the larger table */
      int hash = me_hash(entry, exp_mlist -> num_of_buckets);
      Bucket *bucket2 = exp_mlist -> buckets + hash;
      *(bucket2 -> entries + (bucket2 -> num_of_entries)) = entry;
      bucket2 -> num_of_entries += 1;
    }
  }
  /* free all memory allocated for the previous hash table */
  free(ml -> buckets);
  free(ml);

  return exp_mlist;
}
