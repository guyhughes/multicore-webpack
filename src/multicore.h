#pragma once
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SEARCH_ARGV(needle) search_argv(argc, argv, (needle));

#define LOG(msg)                                                               \
  fprintf(stderr,                                                              \
          ANSI_COLOR_CYAN "[multicore-webpack]" ANSI_COLOR_RESET " %s\n",      \
          msg);
#define LOG_FATAL(msg)                                                         \
  fprintf(stderr,                                                              \
          ANSI_COLOR_CYAN "[multicore-webpack]" ANSI_COLOR_RED                 \
                          " fatal: %s (%s:%d)\n" ANSI_COLOR_RESET,             \
          msg,                                                                 \
          __FILE__,                                                            \
          __LINE__);
#define DIE(errnum)                                                            \
  {                                                                            \
    LOG_FATAL(strerror((errnum)));                                             \
    exit(EXIT_FAILURE);                                                        \
  }

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"
