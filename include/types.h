#ifndef __TYPES_H__
#define __TYPES_H__

#ifdef _WIN32
# include <wchar.h>
typedef void* file_t;
typedef wchar_t* title_t;
typedef const wchar_t* file_fmt_t;
#else
typedef int file_t;
typedef char* title_t;
typedef const char* file_fmt_t;
#endif

#endif /* __TYPES_H__ */
