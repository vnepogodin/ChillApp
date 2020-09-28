#include <stdlib.h> /* malloc, free */
#include <string.h> /* strtol */
#include <math.h> /* logf */
#ifdef _WIN32
# include <windows.h> /* CreateFile, ReadFileEx, WriteFileEx, CloseHandle.. */
#else
# include <sys/stat.h> /* mkdir */
# include <fcntl.h> /* openat, O_RDONLY */
# include <unistd.h> /* pread, close */
#endif

#include <ui.h> /* uiProgressBar, uiMain, uiQuit.. */


static uiProgressBar *pbar;
static int progress_value = 0;
#ifndef _WIN32
static char*    folder = NULL;
#else
static wchar_t* folder = NULL;
#endif

#define SWAP(_first, _second) do{       \
    register char _temp = *(_second);   \
    *(_second) = *(_first);             \
    *(_first) = _temp;                  \
}while(0)

/* Transform functions */
#define REVERSE(_str, _length) do{          \
    register int start = 0;                 \
    register int end = (_length) - 1;       \
                                            \
    while (start < end) {                   \
        SWAP(&(_str[start]), &(_str[end])); \
        ++start;                            \
        --end;                              \
    }                                       \
}while(0)

void itoa_d(const int _num_param, char* _str_param) {
    register int num = _num_param;

    /* Process individual digits */
    register int i = 0;
    while (num != 0) {
        register int rem = num % 10;

        if (rem > 9)
            _str_param[i] = (char)((rem - 10) + 'a');
        else
            _str_param[i] = (char)(rem + '0');

        num /= 10;

        ++i;
    }

    _str_param[i] = '\0'; /* Append string terminator */

    /* Reverse the string */
    REVERSE(_str_param, i);
}

static unsigned long count_numbers(const int num) {
    return (unsigned long)logf((float)num);
}

static void get_dir(void) {
#ifdef _WIN32
    wchar_t* result = (wchar_t*)malloc(7UL);
    result = L"config";
#else
    const char* env = getenv("HOME");
    const char path[24] = "/.config/chillapp/";

    register const unsigned long len = strlen(env) + strlen(path);
    char* result = (char *)malloc(len + 1UL);
#endif

#ifndef _WIN32
    strncat(result, env, sizeof(env) + 1UL);
    strncat(result, path, 24UL);
    strncat(result, "config", 7UL);
#endif

    folder = result;
}

static int addTime(void* data) {
    if (progress_value != 100) {
        ++progress_value;

        uiProgressBarSetValue(pbar, progress_value);
    } else {
        uiQuit();
    }

    return 1;
}

static int onClosing(uiWindow *w, void* data) {
    uiQuit();
    return 1;
}

static int onShouldQuit(void* data) {
    uiWindow *mainwin = uiWindow(data);

    uiControlDestroy(uiControl(mainwin));

    free(folder);
    return 1;
}

static void onSkip(uiButton *b, void* data) {
    uiProgressBarSetValue(pbar, 100);
    uiQuit();
}

static void onAdd(uiButton *b, void* data) {
#ifdef _WIN32
    register void* fd = CreateFile(folder, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
#else
    register int fd = openat(0, folder, O_RDONLY, 0);
#endif
    if (fd != -1) {
        char buf[1000];

#ifdef _WIN32
        OVERLAPPED ol = { 0 };
        ReadFileEx(fd, buf, 999, &ol, 3);
#else
        pread(fd, buf, 1000UL, 0);
#endif

        char *ptr = NULL;
        const int value = (int)strtol(buf, &ptr, 10) + 5;
        register const unsigned long len = count_numbers(value);


#ifdef _WIN32
        CloseHandle(fd);
        register void* buf_file = CreateFile(folder, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#else
        close(fd);
        register int buf_file = openat(0, folder, O_WRONLY, 0);
#endif

        if (buf_file != -1) {
            char* _num = (char *)malloc(len + 1UL);
            itoa_d(value, _num);

#ifdef _WIN32
            WriteFileEx(buf_file, _num, len, &ol, 3);

            /* Frees memory */
            CloseHandle(buf_file);
#else
            pwrite(buf_file, _num, len, 0);

            /* Frees memory */
            close(buf_file);
#endif
            free(_num);
        }
    }

    uiQuit();
}

int main(void) {
    uiInitOptions o = { 0 };

    if (uiInit(&o) != NULL)
        abort();
#ifdef _WIN32
    register const unsigned char isMenuBar = 1U;
#else
    register const unsigned char isMenuBar = 0U;
#endif

    uiWindow *win = uiNewWindow("Timer", 550, 80, isMenuBar);
    uiWindowOnClosing(win, onClosing, NULL);
    uiOnShouldQuit(onShouldQuit, win);

    uiWindowSetMargined(win, 2);
    uiWindowSetBorderless(win, 0);

    uiBox *vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    uiWindowSetChild(win, uiControl(vbox));


    /* TOP */
    uiBox *top_box = uiNewHorizontalBox();
    uiBoxAppend(vbox, uiControl(top_box), 0);

    /* Skip button */
    uiButton *skip = uiNewButton("Skip");
    uiButtonOnClicked(skip, onSkip, NULL);
    uiBoxAppend(top_box, uiControl(skip), 1);

    /* Add button */
    uiButton *add = uiNewButton("Add 5 minutes");
    uiButtonOnClicked(add, onAdd, NULL);
    uiBoxAppend(top_box, uiControl(add), 1);

    uiBoxAppend(vbox,
                uiControl(uiNewHorizontalSeparator()),
                0);

    pbar = uiNewProgressBar();
    uiBoxAppend(vbox, uiControl(pbar), 0);

    uiTimer(244, addTime, NULL);

    uiControlShow(uiControl(win));
    get_dir();

    uiMain();

    return 0;
}
