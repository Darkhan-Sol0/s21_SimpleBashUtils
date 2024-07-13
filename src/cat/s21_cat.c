#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void output(FILE *fptr, char flags[], int *numer) {
  char c = fgetc(fptr);
  char lac = '\n';
  while (c != EOF) {
    if (flags[2] != 0 && lac == '\n' && flags[0] == 0) {
      printf("%6.0d\t", *numer);
      *numer += 1;
    }
    if (flags[0] != 0 && lac == '\n' && lac != c) {
      printf("%6.0d\t", *numer);
      *numer += 1;
    }
    if (flags[4] != 0 && c == '\t') {
      c = '\0';
      printf("^I");
    }
    if (flags[5] != 0) {
      if (c < 0) {
        printf("M-");
        if (c + 256 >= 128 + 32) {
          if (c + 256 < 128 + 127) {
            c = c + 256 - 128;
          } else {
            printf("^?");
            c = '\0';
          }
        } else {
          printf("^");
          c = c + 256 - 128 + 64;
        }
      } else if (c > 0 && c < 32 && c != '\n' && c != '\t') {
        printf("^");
        c = c + 64;
      }
    }
    if (c != '\0') printf("%c", c);
    lac = c;
    c = fgetc(fptr);
    if (flags[3] != 0 && lac == '\n' && lac == c) {
      if (flags[2] != 0 && lac == '\n' && flags[0] == 0) {
        printf("%6.0d\t", *numer);
        *numer += 1;
      }
      printf("%c", c);
      while (c == '\n') {
        c = fgetc(fptr);
      }
    }
    if (flags[1] != 0 && c == '\n') {
      printf("$");
    }
  }
}

void getfile(int argc, char *argv[], char flags[]) {
  int numer = 1;
  FILE *fptr;
  for (int i = 0; i < argc; i++) {
    if (!strstr(argv[i], "-") && i != 0) {
      fptr = fopen(argv[i], "r");
      if (fptr == NULL) {
        printf("%s: No such file or directory\n", argv[i]);
      } else {
        output(fptr, flags, &numer);
        fclose(fptr);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  const char *short_options = "beEnstTv";
  char flags[] = {0, 0, 0, 0, 0, 0, '\n'};
  const struct option long_options[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {NULL, 0, NULL, 0}};
  opterr = 0;
  int errinput = 0;
  int rez;
  int option_index;
  while ((rez = getopt_long(argc, argv, short_options, long_options,
                            &option_index)) != -1) {
    switch (rez) {
      case 'b':
        flags[0] = rez;
        break;
      case 'e':
        flags[1] = rez;
        flags[5] = rez;
        break;
      case 'E':
        flags[1] = rez;
        break;
      case 'n':
        flags[2] = rez;
        break;
      case 's':
        flags[3] = rez;
        break;
      case 't':
        flags[4] = rez;
        flags[5] = rez;
        break;
      case 'T':
        flags[4] = rez;
        break;
      case 'v':
        flags[5] = rez;
        break;
      case '?':
        printf("error: invalid flags\n");
        errinput = 1;
        exit(0);
        break;
    }
  }
  if (errinput == 0) {
    getfile(argc, argv, flags);
  }
  return 0;
}