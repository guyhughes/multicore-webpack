#include "multicore.h"

/* prototypes */
int
parse_to_contiguous_null_terminated_fields(char* begin,
                                           const char* restrict sep,
                                           char*** tokens);
int
fork_exec_webpack(char* config_name, char** argv);
char*
search_argv(int argc, char** argv, const char* needle);

int
main(int argc, char** argv)
{
  int ret;

  if (argc < 3 || memcmp(argv[2], ":::", 4)) {
    DIE(EINVAL);
  }

/*
 *   [> skip past parsed args and progname <]
 *   argc -= 3;
 *   argv += 3;
 * 
 *   SEARCH_ARGV can be called in this section 
 * 
 *   [> go back 3 spaces to use this for exec with the first 3 pointers overriden <]
 *   argc += 3;
 *   argv -= 3;
 */

  char* spec = argv[1];
  char** groups = NULL;
  int n_groups;
  pid_t* pids;
  n_groups = parse_to_contiguous_null_terminated_fields(spec, ",", &groups);
  pids = calloc(n_groups, sizeof(pid_t));

  for (int i = 0; i < n_groups; ++i) {
    char* config = groups[i];
    ret = fork_exec_webpack(config, argv);
    if (ret == -1) {
      /* error in exec, child process */
      DIE(errno);
    }
    /* parent process only */
    pids[i] = (pid_t)ret;
  }

  int status = EXIT_SUCCESS;
  for (;;) {
    int pid;
    if ((pid = waitpid(0, &status, WNOHANG)) == -1) {
      perror(NULL);
      exit(EXIT_FAILURE);
    }
    if (pid == 0) {
      continue;
    }
    if (status != 0) {
      signal(SIGQUIT, SIG_IGN);
      kill(-getpid(), SIGQUIT);
      for (int i = 0; i < n_groups; ++i)
        kill(pids[i], SIGTERM);
      LOG_FATAL("webpack failed");
      return EXIT_FAILURE;
    }
    if (errno == ECHILD) {
      LOG("all done");
      break;
    }
    sleep(2);
  }

  free(pids);
  return EXIT_SUCCESS;
}

int
fork_exec_webpack(char* config_name, char** argv)
{
  int pid;

  pid = fork();
  if (pid == -1) {
    DIE(errno);
  }
  if (pid != 0) {
    /* this is the parent */
    return pid;
  }
  argv[0] = "webpack";
  argv[1] = "--config-name";
  argv[2] = config_name;
  char* str = malloc(0);
  int argvlen = 1;
  for (int i = 0; argv[i] != NULL; ++i) {
    argvlen += strlen(argv[i]);
  }
  str = realloc(str, argvlen);
  for (int i = 0; argv[i] != NULL; ++i) {
    strcat(str, argv[i]);
    strcat(str, " ");
  }

  LOG(str);
  /* exec only returns on error */
  return execvp(argv[0], argv);
}

int
parse_to_contiguous_null_terminated_fields(char* begin,
                                           const char* restrict sep,
                                           char*** tokens)
{
  char *tok, *tokbk;
  tok = strtok_r(begin, sep, &tokbk);

  for (int i = 0; i < 99; (tok = strtok_r(NULL, sep, &tokbk)), ++i) {
    if (!(*tokens = realloc(*tokens, (i + 1) * sizeof(char*)))) {
      DIE(errno);
    }
    *(*tokens + i) = tok;
    if (!tok) {
      return i;
    }
  }
  DIE(EINVAL);
}

/*
 * char* search_argv(int argc, char **argv, const char *needle)
 * {
 *   const int n = strlen(needle);
 *
 *   for (int i=0; i < argc; ++i)
 *     if (strncmp(needle, argv[i], n) == 0)
 *       return (char*) (argv + i);
 *
 *   return NULL;
 * }
 */
