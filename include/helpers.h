#ifndef __HELPERS_H__
#define __HELPERS_H__

#ifdef _WIN32
# define UNUSED
#else
# define UNUSED __attribute__((unused))
#endif

void itoa_d(const int, char*);

extern unsigned long count_numbers(const int);

#endif /* __HELPERS_H__ */
