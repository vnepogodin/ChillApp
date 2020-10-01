#ifndef __FILE_MANAGER_H__
#define __FILE_MANAGER_H__

#ifdef _WIN32
# include <windows.h> /* CreateFile, ReadFileEx, WriteFileEx, CloseHandle.. */
#else
# include <fcntl.h> /* openat, O_RDONLY */
# include <unistd.h> /* pread, close */
#endif

#ifdef _WIN32
typedef void* file_t;
# define OPEN_READ_D(file_d) \
    (file_d) = CreateFile(folder, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL); \
    if ((file_d) != -1) {                                                                                                                  \
        char buf[1000] = { 0 };                                                                                                            \
                                                                                                                                           \
        OVERLAPPED ol = { 0 };                                                                                                             \
        ReadFileEx((file_d), buf, 1000UL, &ol, 3);
# define OPEN_WRITE_D(file_d, flag) \
    (file_d) = CreateFile(folder, GENERIC_WRITE, 0, NULL, (flag), FILE_ATTRIBUTE_NORMAL, NULL); \
    if ((file_d) != -1) {                                                                       \
        OVERLAPPED w_ol = { 0 };
# define CLOSE_ND(file_d) CloseHandle((file_d));
#else
typedef int file_t;
# define OPEN_READ_D(file_d) \
    (file_d) = openat(0, folder, O_RDONLY, 0); \
    if ((file_d) != -1) {                      \
        char buf[1000] = { 0 };                \
                                               \
        pread((file_d), buf, 1000UL, 0);
# define OPEN_WRITE_D(file_d, flag) \
    (file_d) = openat(0, folder, (flag), 0); \
    if ((file_d) != -1) {
# define CLOSE_ND(file_d) close((file_d));
#endif

#define CLOSE_D(file_d) CLOSE_ND((file_d)); }

#endif /* __FILE_MANAGER_H__ */
