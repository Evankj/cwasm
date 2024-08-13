#ifndef STR_H
#define STR_H
#include "../mem/arena.h"
#include "../mem/mem.h"
#include "bool.h"

static i32 strlen(char *str) {
  i32 len = 0;
  char c = str[0];
  while (c != '\0') {
    len += 1;
    c = str[len];
  }
  return len;
}

static char *strcat(char *str1, char *str2, ArenaAllocator *arena) {
  i32 str1Len = strlen(str1);
  i32 str2Len = strlen(str2);
  i32 len = str1Len + str2Len + 1;

  char *newStr = (char *)arena->allocate(arena, len);
  
  for (int i = 0; i < str1Len; i++) {
    newStr[i] = str1[i];
  }
  for (int i = 0; i < str2Len; i++) {
    newStr[str1Len+i] = str2[i];
  }
  newStr[len-1] = '\0';

  return newStr;
}

static boolean strcmp(char *str1, char *str2) {
  i32 str1Len = strlen(str1);
  i32 str2Len = strlen(str2);

  if (str1Len != str2Len) return false;
  
  for (int i = 0; i < str1Len; i++) {
    if (str1[i] != str2[i]) return false;
  }

  return true;
}

#endif
