// libc.h

#ifndef LIBC_H
#define LIBC_H

int strcmp(const char *str1, const char *str2) {
  int i;
  for (i = 0; str1[i] || str2[i]; i++) {
    if (!str1[i]) {
      return -1;
    }
    if (!str2[i]) {
      return 1;
    }
    if (str1[i] < str2[i]) {
      return -1;
    }
    if (str1[i] > str2[i]) {
      return 1;
    }
  }
  return 0;
}

int strlen(const char *str) {
  int cnt = 0;
  while (*str != 0) {
    ++str;
    ++cnt;
  }
  return cnt;
}

#endif  // LIBC_H
