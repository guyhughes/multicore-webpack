#include "multicore.h"

/* prototypes */
int parse_to_contiguous_null_terminated_fields(char *begin, const char *restrict sep, char ***tokens);
int fork_exec_webpack(const char *config_name, int arg, char **argv);
char *search_argv(int argc, char **argv, const char *needle);

int main(const int argc, const char *__argv[const restrict 4])
{
    int ret;
    pid_t *pids = NULL;

    if (argc < 3 || memcmp(__argv[2], ":::", 4) != 0) {
        DIE(EINVAL);
    }

    const size_t argvwidth = (argc + 1) * sizeof(char *);
    char **const argv = malloc(argvwidth);
    memcpy(argv, __argv, argvwidth);
    char *const spec = argv[1];

    /*
     *   [> skip past parsed argv and progname <]
     *   argc -= 3;
     *   argv += 3;
     *
     *   [SEARCH_ARGV can be called in this section]
     *
     *   [> go back 3 spaces to use this for exec with the first 3 pointers
     * overriden <] argc += 3; argv -= 3;
     */

    char **groups = NULL;
    const int n_groups = parse_to_contiguous_null_terminated_fields(spec, ",", &groups);
    if (n_groups == 0) {
        goto fail;
    }
    pids = calloc(n_groups, sizeof(pid_t));

    for (int i = 0; i < n_groups; ++i) {
        char *config = groups[i];
        ret = fork_exec_webpack(config, argc, argv);
        if (ret == -1) {
            /* error in exec, child process */
            DIE(errno);
        }
        /* parent process only */
        pids[i] = (pid_t)ret;
    }
    free(groups);

    int status = EXIT_SUCCESS;
    for (;;) {
        int pid;
        if ((pid = waitpid(0, &status, WNOHANG)) == -1) {
            if (errno == ECHILD) {
                LOG("all done");
                break;
            }
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
            goto fail;
        }
        if (errno == ECHILD) {
            LOG("all done");
            break;
        }
        sleep(2);
    }

    if (argv)
        free(argv);
    if (pids)
        free(pids);
    return EXIT_SUCCESS;
fail:
    if (argv)
        free(argv);
    if (pids)
        free(pids);
    DIE(EFAULT);
    return EXIT_FAILURE;
}

int fork_exec_webpack(const char *config_name, int argc, char **argv)
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
    argv[0] = WEBPACK_BINARY;
    argv[1] = "--config-name";
    argv[2] = (char *)config_name;
    size_t argv_total_width = 1; /* null byte */
    for (int i = 0; i < argc && argv[i] != NULL; ++i)
        argv_total_width += strlen(argv[i]) + 1;
    char *const str = malloc(argv_total_width);
    for (int i = 0; i < argc && argv[i] != NULL; ++i) {
        strcat(str, argv[i]);
        strcat(str, " ");
    }
    LOG(str);
    free(str);
    /* exec only returns on error */
    return execvp(argv[0], argv);
}

int parse_to_contiguous_null_terminated_fields(char *begin, const char *const restrict sep, char ***tokens)
{
    char *tok = NULL, *tokbk = NULL;
    tok = strtok_r(begin, sep, &tokbk);

    for (int i = 0; i < 99; (tok = strtok_r(NULL, sep, &tokbk)), ++i) {
        if (!(*tokens = realloc(*tokens, (i + 1) * sizeof(char *)))) {
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
