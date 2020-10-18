#ifndef __CONFIG_H__
#define __CONFIG_H__

typedef struct _Time_manager time_manager;

#ifdef _WIN32
int init_conf(time_manager *, wchar_t**);
#else
int init_conf(time_manager *, const char**);
#endif

extern time_manager* time_manager_new(void);
extern void time_manager_free(time_manager *);

extern int get_sleep_time(time_manager *);
extern int get_timeout(time_manager *);
extern char* get_title(time_manager *);

#endif /* __CONFIG_H__ */
