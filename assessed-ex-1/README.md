##DYNAMIC HASH TABLE

This project represents a dynamic hash table implemented in C - coursework for one of my third level courses.

It processes mailing lists, getting them as stdin from the command line and finds potential duplicates.
Example:

Potential duplicate
/===================
Meldrew, Margaret\n
1 Happenstance Place, Epping, London\n
N11 3SR\n
/==========
Meldrew, Victor\n
1 Happenstance Place, London\n
N11 3SR\n

All entries need to have the same structure:
--surname--, --firstnames--\n
--housenum-- --address--\n
--postcode--\n

The hash table starts from very small number of buckets, but resizes automatically when using larger mailing lists.
