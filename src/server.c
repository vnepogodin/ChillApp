#include "file_manager.h"

#include <stdlib.h> /* exit */
#include <string.h> /* strtol */
#include <signal.h> /* signal, SIGINT, SIGTERM */
#ifndef _WIN32
# include <time.h> /* sleep */
# include <sys/wait.h> /* waitpid */
#else
# include <io.h> /* _mktemp_s */
#endif

#ifndef _WIN32
static char buf[21] = "/tmp/.config-XXXXXXX";
#else
static wchar_t buf[150] = { 0 };
#endif

static inline int check_time(file_fmt_t filename) {
    int result = 0;

    register file_t fd = 0;
    OPEN_READ_D(fd, filename)
        char* ptr = NULL;
        result = (int)strtol(buf, &ptr, 10);
    CLOSE_D(fd)
    return result;
}

static void handler(const int sig) {
#ifndef _WIN32
    unlink(buf);
#else
    _wunlink(buf);
#endif
    exit(0);
}

int main(void) {
    register const int config_time = check_time(CONFIG_PATH);
    register int sleep_time = config_time;

#ifndef _WIN32
    int fdtmp = mkstemp(buf);
    if (fdtmp != -1)
        CLOSE_ND(fdtmp)
#else
    GetTempPath(134, buf);

    wcsncat_s(buf, 150UL, L".config-XXXXXX", 16UL);
    _wmktemp_s(buf, 150UL);

    register file_t fd = NULL;
    OPEN_WRITE_D(fd, buf, CREATE_ALWAYS)
    CLOSE_D(fd)
#endif

    signal(SIGINT, handler);
    signal(SIGTERM, handler);
#ifndef _WIN32
    signal(SIGQUIT, handler);
#endif

    while (1) {
#ifdef _WIN32
        SleepEx(60000 * sleep_time, 0);
#else
        sleep(60 * sleep_time);
#endif

#ifdef _WIN32
        STARTUPINFO si = { 0 };
        si.cb = sizeof(si);

        PROCESS_INFORMATION pi = { 0 };
        wchar_t args[161] = L"chill.exe ";
        wcsncat_s(args, 161UL, buf, wcsnlen_s(buf, 161UL));

        if (CreateProcess(NULL, args, NULL, NULL, 0, 0, NULL, NULL, &si, &pi)) {
            WaitForSingleObject(pi.hProcess, INFINITE);

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
#else
        register int pid = fork();
        int status = 0;
        if (pid == 0) {
            char* const args[3] = { "chill", buf, NULL };

            execvp(args[0], args);
        } else {
            do {
                waitpid(pid, &status, 0);
            } while ((!WIFEXITED(status)) && (!WIFSIGNALED(status)));
        }
#endif

        sleep_time = config_time + check_time(buf);
    }

    return 0;
}
