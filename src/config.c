#include "../include/config.h"
#include "../include/file_manager.h"

#include <stdlib.h> /* getenv, mkstemp, malloc, free */
#include <string.h> /* strncat */

#ifdef _WIN32
# include <io.h> /* _mktemp_s */
#endif

#include <libconfig.h> /* config_t, config_setting_t */

struct _Time_manager {
    int sleep;
    int out;
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

static inline void get_envs(config_setting_t *setting, const char** args, time_manager *t_conf) {
    if (setting != NULL) {
        register const unsigned len = config_setting_length(setting);
        register unsigned i = 0U;
        while (i < len) {
            config_setting_t *pos = config_setting_get_elem(setting, i);

            config_setting_lookup_int(pos, args[0], &t_conf->sleep);
            config_setting_lookup_int(pos, args[1], &t_conf->out);

            ++i;
        }
    }
}

static inline file_fmt_t config_get_dir(void) {
#ifndef _WIN32
#ifdef DEBUG
    return "etc/config";
#else
    const char* env = getenv("HOME");
    const char path[26] = "/.config/chill_app/config";

    strncat((char *)env, path, 25UL);

    return env;
#endif
#else
    return L".\\etc\\config";
#endif
}

static inline void config_get_buf(file_fmt_t* buf) {
#ifndef _WIN32
    static char res[21] = "/tmp/.config-XXXXXXX";
    int fdtmp = mkstemp(res);
    if (fdtmp != -1)
        CLOSE_ND(fdtmp)
#else
    static wchar_t res[150] = L".config-XXXXXX";
    GetTempPath(134, res);

    _wmktemp_s(res, 150UL);

    register file_t fd = NULL;
    OPEN_WRITE_D(fd, res, CREATE_ALWAYS)
    CLOSE_D(fd)
#endif

    *buf = (file_fmt_t)res;
}


static int conf_read(time_manager *t_conf) {
    config_t cfg;
    config_init(&cfg);

    /* Read the file. If there is an error, report it and exit. */
    register file_fmt_t path = config_get_dir();

    if(!config_read_file(&cfg, path)) {
        fprintf(stderr, "ERROR %d: %s\n", config_error_line(&cfg),
                                          config_error_text(&cfg));
        config_destroy(&cfg);
        return -1;
    }

    config_setting_t *setting = config_lookup(&cfg, "time");
    const char* vars[2] = { "sleep", "out" };
    get_envs(setting, vars, t_conf);

    config_destroy(&cfg);
    return 0;
}

int init_conf(time_manager *t_conf, file_fmt_t *file_buf) {
    register int result = -1;

    config_get_buf(file_buf);

    if (conf_read(t_conf) != -1)
        result = 0;

    return result;
}
