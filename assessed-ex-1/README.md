###DYNAMIC HASH TABLE

This project represents a dynamic hash table implemented in C - coursework for one of my level 3 courses.


It processes mailing lists, getting them as stdin from the command line and finds potential duplicates.

Example:
```
Potential duplicate
===================
Meldrew, Margaret
1 Happenstance Place, Epping, London
N11 3SR
==========
Meldrew, Victor
1 Happenstance Place, London
N11 3SR
```

All entries need to have the same structure:

```
--surname--, --firstnames--
--housenum-- --address--
--postcode--
```

The hash table starts from very small number of buckets, but resizes automatically when using larger mailing lists.

Compile with:
```
gcc -o prog mentry.c mlist.c finddupl.c
```

Execute with:
```
./prog < S.txt
```
