/**
 * miniunit.h
 *
 * Copyright (c) 2017 Hayato Takenaka
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 *
 * author: Hayato Takenaka (https://urin.github.io)
**/
#pragma once

#ifdef _WIN32
#  define MINIUNIT_CONFIG_NO_COLOR
#  ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#  endif
#endif

//------------------------------------------------------------------------------
// public
//------------------------------------------------------------------------------
#include <stdlib.h>

extern const char *test_case(const char *case_name);

#define expect(description, condition) do {                                    \
  miniunit.item_count++;                                                       \
  if (condition) {                                                             \
    miniunit_log(miniunit.colors.green, "  (%d-%d) OK %s",                     \
      miniunit.case_count, miniunit.item_count, (description)                  \
    );                                                                         \
  } else {                                                                     \
    miniunit_log(miniunit.colors.red,                                          \
      "  (%d-%d) NG %s (" #condition ") [%s:%d]",                              \
      miniunit.case_count, miniunit.item_count, (description),                 \
      __FILE__, __LINE__                                                       \
    );                                                                         \
    exit(1);                                                                   \
  }                                                                            \
} while(false)

//------------------------------------------------------------------------------
// private
//------------------------------------------------------------------------------
struct miniunit_t {
  // constants
  struct {
    int none, black, red, green, yellow, blue, magenta, cyan, white;
  } colors;
  char esc[9][8];
  // internals
  int case_count;
  char case_name[1024];
  int item_count;
};

extern struct miniunit_t miniunit;
extern void miniunit_log();

#ifdef MINIUNIT_CONFIG_MAIN
#  include <stdio.h>
#  include <string.h>
#  include <stdarg.h>
#  ifdef _WIN32
#    pragma comment(lib, "kernel32.lib")
#    include <windows.h>
#  else
#    include <time.h>
#  endif

struct miniunit_t miniunit = {
  { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
  { "\x1b[0m", "\x1b[30m", "\x1b[31m", "\x1b[32m", "\x1b[33m", "\x1b[34m", "\x1b[35m", "\x1b[36m", "\x1b[37m" }
};

void miniunit_log(int color, const char *format, ...)
{
  char message[1024];
  va_list args;
  va_start(args, format);
  vsprintf(message, format, args);
  va_end(args);
#  ifdef MINIUNIT_CONFIG_NO_COLOR
  puts(message);
#  else
#    ifdef _WIN32
  // TODO color support for windows
#    else
  printf("%s%s%s\n", miniunit.esc[color], message, miniunit.esc[0]);
#    endif
#  endif
  fflush(stdout);
}

static double miniunit_get_seconds()
{
#  ifdef _WIN32
  LARGE_INTEGER freq, now;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&now);
  return (double) now.QuadPart / freq.QuadPart;
#  else
  timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return now.tv_sec + now.tv_nsec * 1e-9;
#  endif
}

const char *test_case(const char *case_name)
{
  static bool has_prev_case;
  static double prev_time;
  double now = miniunit_get_seconds();
  if (has_prev_case) {
    miniunit_log(miniunit.colors.none, "  Elapsed time %.3lfsec.", now - prev_time);
  } else {
    has_prev_case = true;
  }
  prev_time = now;
  miniunit.case_count++;
  miniunit.item_count = 0;
  strcpy(miniunit.case_name, case_name);
  miniunit_log(miniunit.colors.cyan, "(%d) %s", miniunit.case_count, case_name);
  return case_name;
}
#endif

