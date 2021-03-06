#include "mentry.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_LINE_LENGTH 128

void lower(char *str);
void str_clean(char *str);
char *strcat_address(MEntry *me);

MEntry *me_get(FILE *fd) {

  MEntry *entry = (MEntry*)malloc(sizeof(MEntry));

  char *buffer_name = malloc(MAX_LINE_LENGTH);
  char *buffer_address = malloc(MAX_LINE_LENGTH);
  char *postcode = malloc(MAX_LINE_LENGTH);
  char *full_address = malloc(MAX_LINE_LENGTH*3);
  char *surname = malloc(MAX_LINE_LENGTH);
  int house_number = 0;
  
  /* read entry information from file */	
  fgets(buffer_name, MAX_LINE_LENGTH, fd);
  fgets(buffer_address, MAX_LINE_LENGTH, fd);
  fgets(postcode, MAX_LINE_LENGTH, fd);

  /* if the end of the file is reached, free allocated memory and return */
  if (feof(fd)) {
    free(buffer_name);
    free(buffer_address);
    free(postcode);
    free(full_address);
    free(surname);
    free(entry);
    return NULL;
  }

  /* store data in full address */
  strcpy(full_address, buffer_name);
  strcat(full_address, buffer_address);
  strcat(full_address, postcode);

  /* process surname */
  sscanf(buffer_name, "%s", surname);
  surname[strlen(surname)-1] = '\0';
  lower(surname);

  /* process postcode */
  postcode[strlen(postcode)-1] = '\0';
  str_clean(postcode);
  lower(postcode);

  /* process house number */
  if (isdigit(buffer_address[0])) {
    sscanf(buffer_address, "%d", &house_number);
  }
  
  /* assign entry props */
  entry -> surname = surname;
  entry -> postcode = postcode;
  entry -> house_number = house_number;
  entry -> full_address = full_address;
  
  /* free allocated memory for buffers */
  free(buffer_name);
  free(buffer_address);

  return entry;
}

/* custom function to turn all characters from a string to lowercase */
void lower(char *str) {
  while(strcmp(str, "\0") != 0) {
    *str = tolower(*str);
    str++;
  }
}

/* custom funtion to remove all non-alphanumerical chars from a string */
void str_clean(char *str) {
  int i = 0;
  int j = 0;
  char c;
  while ((c = str[i++]) != '\0') {
    if (isalnum(c)) {
      str[j++] = c;
    }
  }
  str[j] = '\0';
}

/* custom function to create a hash string for a given entry */ 
char *strcat_address(MEntry *me) {
  
  /* cast house number from int to char array */
  char house_number[10];
  sprintf(house_number, "%d", me -> house_number);

  char *string = malloc(
    strlen(me -> surname) +
    strlen(me -> postcode) +
    strlen(house_number) +
    1
  );

  strcpy(string, me -> surname);
  strcat(string, me -> postcode);
  strcat(string, house_number);
  return string;
}

unsigned long me_hash(MEntry *me, unsigned long size) {
  char *string = strcat_address(me);
  unsigned long hashval = 0;
  int i;
  for (i = 0; string[i] != '\0'; i++) {
    hashval = string[i] + hashval*31;
  }
  
  /* free memory allocated for the hashstring in strcat_address() */
  free(string);
  return hashval % size;
}

void me_print(MEntry *me, FILE *fd) {
  fprintf(fd, "%s", me -> full_address);
}

int me_compare(MEntry *me1, MEntry *me2) {
  /* create hash strings */
  char *strme1 = strcat_address(me1);
  char *strme2 = strcat_address(me2);

  /* compare them */
  int result = strcmp(strme1, strme2);
  
  /* free memory allocated for the hashstrings in strcat_address() */
  free(strme1);
  free(strme2);
  return result;
}

void me_destroy(MEntry *me) {
  free(me -> surname);
  free(me -> postcode);
  free(me -> full_address);
  free(me);
}
