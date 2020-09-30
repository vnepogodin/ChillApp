#include "file_manager.h"

#include <stdlib.h> /* malloc, free, exit */
#include <string.h> /* strtol */
#include <signal.h> /* signal, SIGINT, SIGTERM */
#ifndef _WIN32
# include <time.h> /* sleep */
# include <sys/wait.h> /* waitpid */
# include <sys/stat.h> /* mkdir */
# include <unistd.h> /* fork, execv */
#endif

#ifndef _WIN32
static char* folder = NULL;
#else
static wchar_t* folder = NULL;
#endif

static void get_dir(void) {
#ifdef _WIN32
    folder = L"config";
#else
    const char* env = getenv("HOME");
    const char path[24] = "/.config/chillapp/";

    register const unsigned long len = strlen(env) + strlen(path);
    char* result = (char*)malloc(len + 1UL);

    strncat(result, env, sizeof(env) + 1UL);
    strncat(result, path, 24UL);

    mkdir(result, 6610);
    strncat(result, "config", 7UL);

    folder = result;
#endif
}

static void handler(const int sig) {
    register void* fd = NULL;

#ifdef _WIN32
    OPEN_WRITE_D(fd, OPEN_EXISTING)
#else
    OPEN_WRITE_D(fd, O_WRONLY)
#endif

    CLOSE_D(fd)

    /* Frees memory */
#ifndef _WIN32
    free(folder);
#endif
    exit(0);
}

static void create_config(void) {
    get_dir();
    register void* fd = NULL;

#ifdef _WIN32
    OPEN_WRITE_D(fd, CREATE_ALWAYS)
#else
    OPEN_WRITE_D(fd, O_WRONLY | O_CREAT)
#endif

    CLOSE_D(fd)
}

int main(void) {
    create_config();

    register int timeActivity = 10;

    signal(SIGINT, handler);
    signal(SIGTERM, handler);
#ifndef _WIN32
    signal(SIGQUIT, handler);
#endif

    while (1) {
#ifdef _WIN32
        SleepEx(60000 * timeActivity, 0);
#else
        sleep(60 * timeActivity);
#endif

#ifdef _WIN32
        STARTUPINFO si = { 0 };
        si.cb = sizeof(si);

        PROCESS_INFORMATION pi = { 0 };
        if (CreateProcess(L"chill.exe", NULL, NULL, NULL, 0, 0, NULL, NULL, &si, &pi)) {
            WaitForSingleObject(pi.hProcess, INFINITE);

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
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
#endif

        register void* fd = NULL;
        OPEN_READ_D(fd)
            char* ptr = NULL;
            timeActivity = (int)strtol(buf, &ptr, 10);
        CLOSE_D(fd)
    }

    return 0;
}
