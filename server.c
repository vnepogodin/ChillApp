#include <stdlib.h> /* malloc, free, exit */
#include <string.h> /* strtol */
#include <signal.h> /* signal, SIGINT, SIGTERM, SIGQUIT */
#ifdef _WIN32
# include <windows.h> /* Sleep */
#else
# include <time.h> /* sleep */
# include <sys/wait.h> /* waitpid */
# include <sys/stat.h> /* mkdir */
# include <fcntl.h> /* openat, O_RDONLY */
# include <unistd.h> /* pread, close, fork, execv */
#endif

static char* folder = NULL;

static void get_dir(void) {
    const char* env = getenv("HOME");
#ifdef _WIN32
    const char path[24] = "\\.config\\chillapp\\";
#else
    const char path[24] = "/.config/chillapp/";
#endif

    register const unsigned long len = strlen(env) + strlen(path);
    char* result = (char *)malloc(len + 1UL);

    strncat(result, env, sizeof(env) + 1UL);
    strncat(result, path, 24UL);

    mkdir(result, 6610);
    strncat(result, "config", 7UL);

    folder = result;
}

static void handler(const int sig) {
#ifdef _WIN32
#else
    register int fd = openat(0, folder, O_WRONLY, 0);

    if (fd != -1) {
        pwrite(fd, "10", 2, 0);
        close(fd);
    }
#endif

    /* Frees memory */
    free(folder);
    exit(0);
}

static void create_config(void) {
    get_dir();
#ifdef _WIN32
#else
    register int fd = openat(0, folder, O_WRONLY | O_CREAT, 0);

    if (fd != -1) {
        pwrite(fd, "10", 2, 0);
        close(fd);
    }
#endif
}

int main(void) {
    create_config();

    register int timeActivity = 10;

    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGQUIT, handler);
    while (1) {
#ifdef _WIN32
        Sleep(60000);
#else
        sleep(60 * timeActivity);
#endif

#ifdef _WIN32
        printf("Fix me\n");
#else
        register int pid = fork();
        int status = 0;
        if (pid == 0) {
            char* args[2] = { "chill", NULL };

            execv(args[0], args);
        } else {
            do {
                waitpid(pid, &status, 0);
            } while ((!WIFEXITED(status)) && (!WIFSIGNALED(status)));
        }

        register int fd = openat(0, folder, O_RDONLY, 0);
        if (fd != -1) {
            char buf[1000];
            pread(fd, buf, 1000, 0);

            char* ptr = NULL;
            timeActivity = (int)strtol(buf, &ptr, 10);

            /* Frees memory */
            close(fd);
        }
#endif
    }

    return 0;
}
