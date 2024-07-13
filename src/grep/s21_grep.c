#define _GNU_SOURCE

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  char* flags;
  int len_flags;
  int mem_flags;
} arg_take;

void get_flags(arg_take* arg, char* flags) {
  if (flags == NULL) {
    printf("error");
    return;
  }
  int n = strlen(flags);
  if (arg->len_flags == 0) {
    arg->flags = malloc(1024 * sizeof(char));
    arg->mem_flags = 1024;
  }
  if (arg->mem_flags < arg->len_flags + n) {
    arg->flags = realloc(arg->flags, arg->mem_flags * 2 * sizeof(char));
  }
  if (arg->len_flags != 0) {
    strcat(arg->flags + arg->len_flags, "|");
    arg->len_flags++;
  }
  arg->len_flags += sprintf(arg->flags + arg->len_flags, "(%s)", flags);
}

void get_regex_file(arg_take* arg, char* fp) {
  FILE* f = fopen(fp, "r");
  if (f == NULL) {
    if (!arg->s) perror(fp);
    exit(1);
  }
  char* line = NULL;
  size_t memlen = 0;
  ssize_t read = getline(&line, &memlen, f);
  while (read != -1 && read != EOF) {
    if (line[read - 1] == '\n') line[read - 1] = '\0';
    get_flags(arg, line);
    read = getline(&line, &memlen, f);
  }
  free(line);
  fclose(f);
}

arg_take add_arg(int argc, char* argv[]) {
  arg_take arg = {0};
  int opt;
  while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (opt) {
      case 'e':
        arg.e = 1;
        get_flags(&arg, optarg);
        break;
      case 'i':
        arg.i = REG_ICASE;
        break;
      case 'v':
        arg.v = 1;
        break;
      case 'c':
        arg.c = 1;
        break;
      case 'l':
        arg.c = 1;
        arg.l = 1;
        break;
      case 'n':
        arg.n = 1;
        break;
      case 'h':
        arg.h = 1;
        break;
      case 's':
        arg.s = 1;
        break;
      case 'f':
        arg.f = 1;
        get_regex_file(&arg, optarg);
        break;
      case 'o':
        arg.o = 1;
        break;
    }
  }
  if (arg.len_flags == 0) {
    get_flags(&arg, argv[optind]);
    optind++;
  }
  if (argc - optind == 1) {
    arg.h = 1;
  }
  return arg;
}

void output_line(char* line, int n) {
  for (int i = 0; i < n; i++) {
    putchar(line[i]);
  }
  if (line[n - 1] != '\n') putchar('\n');
}

void output_match(regex_t* re, char* line) {
  regmatch_t match;
  int offset = 0;
  while (1) {
    int result = regexec(re, line + offset, 1, &match, 0);
    if (result != 0) {
      break;
    }
    for (int i = match.rm_so; i < match.rm_eo; i++) {
      putchar(line[offset + i]);
    }
    putchar('\n');
    offset += match.rm_eo;
  }
}

void get_file(arg_take arg, char* path, regex_t* reg) {
  FILE* f = fopen(path, "r");
  if (f == NULL) {
    if (!arg.s) perror(path);
    exit(1);
  }
  char* line = NULL;
  size_t memlen = 0;
  ssize_t read = getline(&line, &memlen, f);
  int line_count = 1;
  int c = 0;
  while (read != -1 && read != EOF) {
    int result = regexec(reg, line, 0, NULL, 0);
    if ((result == 0 && !arg.v) || (arg.v && result != 0)) {
      if (!arg.c && !arg.l) {
        if (!arg.h) printf("%s:", path);
        if (arg.n) printf("%d:", line_count);
        if (arg.o) {
          output_match(reg, line);
        } else {
          output_line(line, read);
        }
      }
      c++;
    }
    read = getline(&line, &memlen, f);
    line_count++;
  }
  free(line);
  if (arg.c && !arg.l) {
    if (!arg.h) printf("%s:", path);
    printf("%d\n", c);
  }
  if (arg.l && c > 0) printf("%s\n", path);
  fclose(f);
}

void output(arg_take arg, int argc, char** argv) {
  regex_t re;
  if (arg.flags == NULL) {
    printf("error");
    return;
  }
  int error = regcomp(&re, arg.flags, REG_EXTENDED | arg.i);
  if (error) perror("Error");
  for (int i = optind; i < argc; i++) {
    get_file(arg, argv[i], &re);
  }
  regfree(&re);
}

int main(int argc, char** argv) {
  arg_take arg = add_arg(argc, argv);
  output(arg, argc, argv);
  free(arg.flags);
  return 0;
}
