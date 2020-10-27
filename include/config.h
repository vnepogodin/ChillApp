#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "types.h"

typedef struct _Time_manager time_manager;

extern unsigned char init_conf(time_manager *, file_fmt_t*);

extern time_manager* time_manager_new(void);
extern void time_manager_free(time_manager *);

extern int get_sleep_time(time_manager *);
extern title_t get_timeout(time_manager *);
extern title_t get_title(time_manager *);

#endif /* __CONFIG_H__ */
