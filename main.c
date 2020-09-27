#include <stdlib.h> /* malloc, free */
#include <string.h> /* strtol */
#include <math.h> /* logf */
#ifdef _WIN32
# include <windows.h>
#else
# include <sys/stat.h> /* mkdir */
# include <fcntl.h> /* openat, O_RDONLY */
# include <unistd.h> /* pread, close */
#endif

#include <ui.h> /* uiProgressBar, uiMain, uiQuit.. */


static uiProgressBar *pbar;
static int progress_value = 0;
static char* folder = NULL;

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

void itoa(const int _num_param, char* _str_param, const int _base_param) {
    register int num = _num_param;

    /* Process individual digits */
    register int i = 0;
    while (num != 0) {
        register int rem = num % _base_param;

        if (rem > 9)
            _str_param[i] = (char)((rem - 10) + 'a');
        else
            _str_param[i] = (char)(rem + '0');

        num /= _base_param;

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
    const char* env = getenv("HOME");
#ifdef _WIN32
    const char path[24] = "\\.config\\chillapp\\";
#else
    const char path[24] = "/.config/chillapp/";
#endif

    register const unsigned long len = strlen(env) + strlen(path);
    char* result = (char *)malloc(len + 1UL);

    strncat(result, env, sizeof(env) + 1UL);
    strncat(result, path, 24UL);

    mkdir(result, 6610);
    strncat(result, "config", 7UL);

    folder = result;
}

static int addTime(void* data) {
    if (progress_value == 100) {
        free(folder);
        uiQuit();
    } else {
        ++progress_value;

        uiProgressBarSetValue(pbar, progress_value);
    }

    return 1;
}

static int onClosing(uiWindow *w, void *data) {
    free(folder);
    uiQuit();
    return 1;
}

static int onShouldQuit(void *data) {
    uiWindow *mainwin = uiWindow(data);

    uiControlDestroy(uiControl(mainwin));

    free(folder);
    return 1;
}

static void onSkip(uiButton *b, void* data) {
    uiProgressBarSetValue(pbar, 100);
    free(folder);
    uiQuit();
}

static void onAdd(uiButton *b, void* data) {
    register int fd = openat(0, folder, O_RDONLY, 0);

    if (fd != -1) {
        char buf[1000];
        pread(fd, buf, 1000UL, 0);

        char* ptr = NULL;
        const int value = (int)strtol(buf, &ptr, 10) + 5;
        register const unsigned long len = count_numbers(value);

        close(fd);
        register int buf_file = openat(0, folder, O_WRONLY, 0);
        if (buf_file != -1) {
            char* _num = (char *)malloc(len + 1UL);
            itoa(value, _num, 10);

            pwrite(buf_file, _num, len, 0);

            /* Frees memory */
            free(_num);
            close(buf_file);
        }
    }

    uiQuit();
}

int main(void) {
    uiInitOptions o = { 0 };

    if (uiInit(&o) != NULL)
        abort();

    uiWindow *win = uiNewWindow("Timer", 640, 480, 0);
    uiWindowOnClosing(win, onClosing, NULL);
    uiOnShouldQuit(onShouldQuit, win);

    uiWindowSetMargined(win, 1);
    uiWindowSetBorderless(win, 1);

    uiBox *vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    uiWindowSetChild(win, uiControl(vbox));


    /* TOP */
    uiBox *top_box = uiNewHorizontalBox();
    uiBoxAppend(vbox, uiControl(top_box), 0);

    /* Skip button */
    uiButton *skip = uiNewButton("Skip");
    uiButtonOnClicked(skip, onSkip, NULL);
    uiBoxAppend(top_box, uiControl(skip), 0);

    /* Add button */
    uiButton *add = uiNewButton("Add 5 minutes");
    uiButtonOnClicked(add, onAdd, NULL);
    uiBoxAppend(top_box, uiControl(add), 0);

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
