#include "mlist.h"
#include <stdio.h>
#include <stdlib.h>

#define BUCKETSIZE 2
#define BUCKETNUM 1

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
  MList *mlist = (MList *)malloc(sizeof(Bucket)*BUCKETNUM);

  if ((mlist) != NULL) {
    mlist -> num_of_buckets = BUCKETNUM;
    mlist -> buckets = (Bucket *)malloc(sizeof(Bucket)*BUCKETNUM);
    for (int i = 0; i < mlist -> num_of_buckets; i++) {
      mlist -> buckets[i].num_of_entries = 0;
    }
  }
  return mlist;
}

int ml_add(MList **ml, MEntry *me) {
  MList *p = *ml;
	if (ml_lookup(*ml, me) != NULL) {
    puts("Entry is already in the mailing list.");
		return 1;
	}

  unsigned long key = me_hash(me, p -> num_of_buckets);
  Bucket *bucket = (p -> buckets) + key;
  if ((bucket -> num_of_entries) < BUCKETSIZE) {
    bucket -> entries[bucket -> num_of_entries] = me;
    bucket -> num_of_entries += 1;
  } else {
    MList *expanded_ml = ml_expand(p);
    if (expanded_ml == NULL) {
      return 0;
    }
    unsigned long key = me_hash(me, expanded_ml -> num_of_buckets);
    Bucket *bucket = expanded_ml -> buckets + key;
    bucket -> entries[bucket -> num_of_entries] = me;
    bucket -> num_of_entries += 1;
    *ml = expanded_ml;

  }
  return 1;
}

MEntry *ml_lookup(MList *ml, MEntry *me) {
  if (ml_verbose) {
		fprintf(stderr, "mlist: ml_lookup() entered\n");
	}

  unsigned long key = me_hash(me, ml -> num_of_buckets);

  Bucket *bucket = ml -> buckets + key;
  for (int i = 0; i < (bucket -> num_of_entries); i++) {
    MEntry *entry = bucket -> entries[i];
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

  if (ml != NULL) {
    for (int i = 0; i < ml -> num_of_buckets; i++) {
      Bucket *bucket = ml -> buckets + i;
      for (int j = 0; j < (bucket -> num_of_entries); j++) {
        MEntry *me = *(bucket -> entries + j);
        me_destroy(me);
      }
    }
    free(ml -> buckets);
    free(ml);
  }
}

void ml_add_to_table(MList *ml, MEntry *me, unsigned long hash) {
  Bucket *bucket = ml -> buckets + hash;
  *(bucket -> entries + (bucket -> num_of_entries)) = me;
  bucket -> num_of_entries += 1;
}

MList *ml_expand(MList *ml) {
  MList *exp_mlist = (MList *)malloc(sizeof(MList));

  if ((exp_mlist) != NULL) {
    exp_mlist -> buckets = (Bucket *)malloc(sizeof(Bucket) * ((ml -> num_of_buckets)*2));
    exp_mlist -> num_of_buckets = (ml -> num_of_buckets)*2;
    for (int i = 0; i < exp_mlist -> num_of_buckets; i++) {
      Bucket *bucket = exp_mlist -> buckets + i;
      bucket -> num_of_entries = 0;
    }
  }

  for (int i = 0; i < ml -> num_of_buckets; i++) {
    Bucket *bucket = ml -> buckets + i;
    for (int j = 0; j < (bucket -> num_of_entries); j++) {
      MEntry *entry = *(bucket -> entries + j);
      Bucket *bucket2 = exp_mlist -> buckets + me_hash(entry, exp_mlist -> num_of_buckets);
      *(bucket2 -> entries + (bucket2 -> num_of_entries)) = entry;
      bucket2 -> num_of_entries += 1;

    }
  }
  free(ml -> buckets);
  free(ml);
  return exp_mlist;
}
