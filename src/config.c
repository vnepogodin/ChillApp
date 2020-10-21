#include "../include/config.h"
#include "../include/helpers.h"
#include "../include/file_manager.h"

#include <stdlib.h> /* getenv, mkstemp, malloc, free */
#include <string.h> /* strncat */

#ifdef _WIN32
# include <io.h> /* _mktemp_s */
#endif

#include <libconfig.h> /* config_t, config_setting_t */

struct _Time_manager {
    title_t title;
    title_t out;

    int sleep;
};

inline time_manager* time_manager_new(void) {
    return (time_manager *)malloc(sizeof(time_manager));
}

inline void time_manager_free(time_manager *t_conf) {
    free(t_conf);
}

inline int get_sleep_time(time_manager *t_conf) {
    return t_conf->sleep;
}
inline title_t get_timeout(time_manager *t_conf) {
    return t_conf->out;
}
inline title_t get_title(time_manager *t_conf) {
    return t_conf->title;
}

static inline void get_envs(config_t *cfg, time_manager *t_conf) {
    int timeout_buf = 0;
    config_lookup_int(cfg, "sleep", &t_conf->sleep);
    config_lookup_int(cfg, "out", &timeout_buf);

    if (t_conf->sleep == 0)
        t_conf->sleep = 10;
    if (timeout_buf != 0) {
        register const unsigned long buf_len = count_numbers(timeout_buf) + 1UL;
        register char* _num_buf = (char *)malloc(buf_len);
        itoa_d(timeout_buf, _num_buf);

#ifdef _WIN32
        t_conf->out = (wchar_t *)malloc(buf_len);
        mbstowcs_s(NULL, t_conf->out, buf_len, _num_buf, buf_len);

        /* Frees buffer */
        free(_num_buf);
#else
        t_conf->out = _num_buf;
#endif

    } else {
        t_conf->out = (title_t)malloc(3UL * sizeof(title_t));
#ifdef _WIN32
        wcscpy_s(t_conf->out, 3UL, L"15");
#else
        strncpy(t_conf->out, "15", 3UL);
#endif
    }
}

static inline const char* config_get_dir(void) {
#ifndef _WIN32
#ifdef DEBUG
    return "etc/config";
#else
    const char* env = getenv("HOME");
    const char path[26] = "/.config/chill_app/config";

    strncat((char *)env, path, 26UL);

    return env;
#endif
#else
    return "etc\\config";
#endif
}

static inline void config_get_buf(file_fmt_t* buf) {
#ifndef _WIN32
    static char res[21] = "/tmp/.config-XXXXXXX";
    register int fdtmp = mkstemp(res);
    if (fdtmp != -1)
        CLOSE_ND(fdtmp)
#else
    static wchar_t res[100] = { 0 };
    GetTempPath(85, res);

    wcsncat_s(res, 100UL, L".config-XXXXXX", 15UL);
    _wmktemp_s(res, 100UL);

    register void* fdtmp = NULL;
    OPEN_WRITE_D(fdtmp, res, CREATE_ALWAYS)
    CLOSE_D(fdtmp)
#endif

    *buf = (file_fmt_t)res;
}


static unsigned char conf_read(time_manager *t_conf) {
    config_t cfg;
    config_init(&cfg);

    /* Read the file. If there is an error, report it and exit. */
    register const char* path = config_get_dir();

    if(!config_read_file(&cfg, path)) {
        printf("\033[31merror:%d\033[0m: %s\n", config_error_line(&cfg),
                                                config_error_text(&cfg));
        config_destroy(&cfg);
        return 0U;
    }

    const char* buf = NULL;
    config_lookup_string(&cfg, "title", &buf);
    if (buf == NULL)
        buf = "Timer";

    register const unsigned long title_len = strlen(buf) + 1UL;
    t_conf->title = (title_t)malloc(title_len);

#ifdef _WIN32
    mbstowcs_s(NULL, t_conf->title, title_len, buf, title_len);
#else
    strncpy(t_conf->title, buf, title_len);
#endif

    get_envs(&cfg, t_conf);

    config_destroy(&cfg);
    return 1U;
}

inline unsigned char init_conf(time_manager *t_conf, file_fmt_t *file_buf) {
    config_get_buf(file_buf);

    return conf_read(t_conf);
}
