/**
 * miniunit.h
 *
 * Copyright (c) 2017 Hayato Takenaka
 *
 * Licensed under the MIT license:
 * https://opensource.org/licenses/mit-license.php
**/
#pragma once

#ifdef _MSC_VER
#  ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#  endif
#endif

//------------------------------------------------------------------------------
// public
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdbool.h>
#ifdef _MSC_VER
#else
#  include <pthread.h>
#endif

#define test_case(title, proc) do {                                            \
  miniunit_preproc((title));                                                   \
  double prev_time = miniunit_get_seconds();                                   \
  (proc);                                                                      \
  miniunit_log(miniunit.colors.none, "  Elapsed time %.6f sec",                \
    miniunit_get_seconds() - prev_time                                         \
  );                                                                           \
} while (false)

#define expect(description, condition) do {                                    \
  bool satisfied = (condition);                                                \
  miniunit_lock();                                                             \
  {                                                                            \
    miniunit.item_count++;                                                     \
    if (satisfied) {                                                           \
      miniunit_log(miniunit.colors.green,                                      \
        "  (%d-%d) [Passed] %s",                                               \
        miniunit.case_count, miniunit.item_count, (description)                \
      );                                                                       \
    } else {                                                                   \
      miniunit_log(miniunit.colors.red,                                        \
        "  (%d-%d) [Failed] %s (" #condition ") [%s:%d]",                      \
        miniunit.case_count, miniunit.item_count, (description),               \
        __FILE__, __LINE__                                                     \
      );                                                                       \
    }                                                                          \
  }                                                                            \
  miniunit_unlock();                                                           \
  if (!satisfied) { exit(1); }                                                 \
} while (false)

//------------------------------------------------------------------------------
// private
//------------------------------------------------------------------------------
typedef struct {
  // constants
  struct {
    int none, black, red, green, yellow, blue, magenta, cyan, white;
  } colors;
  char esc[9][8];
  // internals
  struct lock_t {
#ifdef _MSC_VER
    // TODO
#else
    pthread_mutex_t mutex;
#endif
  } lock;
  bool colorable;
  int case_count;
  char case_name[1024];
  int item_count;
} miniunit_t;

extern void miniunit_preproc(const char *title);
extern void miniunit_lock();
extern void miniunit_unlock();
extern double miniunit_get_seconds();
extern void miniunit_log(int color, const char *format, ...);
extern miniunit_t miniunit;

#ifdef MINIUNIT_MAIN
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#ifdef _MSC_VER
#  pragma comment(lib, "kernel32.lib")
#  include <windows.h>
#else
#  include <time.h>
#  include <unistd.h>
#  include <pthread.h>
#endif

miniunit_t miniunit = {
  { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
  {
    "\x1b[0m", "\x1b[30m", "\x1b[31m", "\x1b[32m", "\x1b[33m",
    "\x1b[34m", "\x1b[35m", "\x1b[36m", "\x1b[37m"
  },
#ifdef _MSC_VER
  // TODO
  {}
#else
  { PTHREAD_MUTEX_INITIALIZER }
#endif
};

void miniunit_preproc(const char *case_name)
{
  miniunit_lock();
  {
    miniunit.case_count++;
    miniunit.item_count = 0;
    strcpy(miniunit.case_name, case_name);
    miniunit_log(miniunit.colors.cyan, "(%d) %s", miniunit.case_count, case_name);
  }
  miniunit_unlock();
}

void miniunit_lock()
{
#ifdef _MSC_VER
  // TODO
#else
  pthread_mutex_lock(&miniunit.lock.mutex);
#endif
}

void miniunit_unlock()
{
#ifdef _MSC_VER
  // TODO
#else
  pthread_mutex_unlock(&miniunit.lock.mutex);
#endif
}

void miniunit_log(int color, const char *format, ...)
{
  char message[4096];
  va_list args;
  va_start(args, format);
  vsprintf(message, format, args);
  va_end(args);
#ifdef _MSC_VER
  // TODO color support for windows
  puts(message);
#else
  if (isatty(fileno(stdout))) {
    printf("%s%s%s\n", miniunit.esc[color], message, miniunit.esc[0]);
  } else {
    puts(message);
  }
#endif
  fflush(stdout);
}

double miniunit_get_seconds()
{
#ifdef _MSC_VER
  LARGE_INTEGER freq, now;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&now);
  return (double) now.QuadPart / freq.QuadPart;
#else
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return now.tv_sec + now.tv_nsec * 1e-9;
#endif
}

#endif

