#include "../include/file_manager.h"
#include "../include/config.h"
#include "../include/helpers.h"

#include <stdlib.h> /* exit, strtol */
#include <signal.h> /* signal, SIGINT, SIGTERM */

#ifndef _WIN32
# include <time.h> /* sleep */
# include <sys/wait.h> /* waitpid */
#endif

static file_fmt_t buf = NULL;
static title_t title = NULL;
static title_t _num = NULL;

static inline int check_time(file_fmt_t filename) {
    int result = 0;

    register file_t fd = (file_t)0;
    OPEN_READ_D(fd, filename)
        char* ptr = NULL;
        result = (int)strtol(buf, &ptr, 10);
    CLOSE_D(fd)
    return result;
}

static void handler(UNUSED const int sig) {
    /* Frees memory */
    free(title);
    free(_num);

#ifndef _WIN32
    unlink(buf);
#else
    _wunlink(buf);
#endif
    exit(0);
}

int main(void) {
    signal(SIGINT, handler);
    signal(SIGTERM, handler);
#ifndef _WIN32
    signal(SIGQUIT, handler);
#endif

    register time_manager *t_conf = time_manager_new();
    if (init_conf(t_conf, &buf) != -1) {
        register const int conf_time = get_sleep_time(t_conf);
        register int sleep_time = conf_time;

        title = get_title(t_conf);
        _num = get_timeout(t_conf);

        time_manager_free(t_conf);

        while (1) {
#ifdef _WIN32
            SleepEx(60000 * sleep_time, 0);

            STARTUPINFO si = { 0 };
            si.cb = sizeof(si);

            PROCESS_INFORMATION pi = { 0 };
            wchar_t args[200] = L"chill.exe -f ";
            wcsncat_s(args, 200UL, buf, wcsnlen_s(buf, 200UL));

            wcsncat_s(args, 200UL, L" -n ", 5UL);
            wcsncat_s(args, 200UL, title, wcsnlen_s(title, 200UL));

            wcsncat_s(args, 200UL, L" -t ", 5UL);
            wcsncat_s(args, 200UL, _num, wcsnlen_s(_num, 200UL));

            if (CreateProcess(NULL, args, NULL, NULL, 0, 0, NULL, NULL, &si, &pi)) {
                WaitForSingleObject(pi.hProcess, INFINITE);

                /* Frees memory */
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
#else
            sleep(60 * sleep_time);

            register int pid = fork();
            int status = 0;
            if (pid == 0) {
                char* const args[8] = { "chill", "-f", (char *)buf, "-n", (char *)title, "-t", _num, NULL };
                execvp(args[0], args);
            } else {
                do {
                    waitpid(pid, &status, 0);
                } while ((!WIFEXITED(status)) && (!WIFSIGNALED(status)));
            }
#endif

            sleep_time = conf_time + check_time(buf);
        }
    }
    return 0;
}
