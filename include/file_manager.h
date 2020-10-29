#ifndef __FILE_MANAGER_H__
#define __FILE_MANAGER_H__

#ifdef _WIN32
# include <windows.h> /* CreateFile, ReadFileEx, WriteFileEx, CloseHandle.. */
#else
# include <fcntl.h> /* openat, O_RDONLY */
# include <unistd.h> /* pread, close */
#endif

#ifdef _WIN32
# define OPEN_READ_D(file_d, path)                                                                                                            \
    (file_d) = CreateFileW((path), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_RANDOM_ACCESS, NULL); \
    if ((file_d) != (void*)-1) {                                                                                                              \
        char buf[25] = { 0 };                                                                                                                 \
                                                                                                                                              \
        OVERLAPPED ol = { 0 };                                                                                                                \
        ReadFileEx((file_d), buf, 25UL, &ol, NULL);
# define OPEN_WRITE_D(file_d, path, flag)                                                          \
    (file_d) = CreateFileW((path), GENERIC_WRITE, 0, NULL, (flag), FILE_FLAG_RANDOM_ACCESS, NULL); \
    if ((file_d) != (void*)-1) {                                                                   \
        OVERLAPPED w_ol = { 0 };
# define CLOSE_ND(file_d) CloseHandle((file_d));
#else
# define OPEN_READ_D(file_d, path)             \
    (file_d) = openat(0, (path), O_RDONLY, 0); \
    if ((file_d) != -1) {                      \
        char buf[25] = { 0 };                  \
                                               \
        pread((file_d), buf, 25UL, 0);
# define OPEN_WRITE_D(file_d, path, flag)    \
    (file_d) = openat(0, (path), (flag), 0); \
    if ((file_d) != (file_t)-1) {
# define CLOSE_ND(file_d) close((file_d));
#endif

#define CLOSE_D(file_d) CLOSE_ND((file_d)); }

#endif /* __FILE_MANAGER_H__ */
