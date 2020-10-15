#include "../include/file_manager.h"

#include <stdlib.h> /* strtol */
#include <math.h> /* logf */

#include <ui.h> /* uiProgressBar, uiMain, uiQuit.. */

static uiProgressBar *pbar;
static int progress_value = 0;
static file_fmt_t filename = 0;

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

static void itoa_d(const int _num_param, char* _str_param) {
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

static inline unsigned long count_numbers(const int num) {
    return (unsigned long)logf((float)num);
}

static inline int addTime(void* data) {
    if (progress_value != 100) {
        ++progress_value;

        uiProgressBarSetValue(pbar, progress_value);
    } else {
        uiQuit();
    }

    return 1;
}

static inline int onClosing(uiWindow *w, void* data) {
    uiQuit();
    return 1;
}

static inline int onShouldQuit(void* data) {
    uiWindow *mainwin = uiWindow(data);

    uiControlDestroy(uiControl(mainwin));

    return 1;
}

static inline void onSkip(uiButton *b, void* data) {
    uiProgressBarSetValue(pbar, 100);
    uiQuit();
}

static void onAdd(uiButton *b, void* data) {
    register file_t fd = 0;

    OPEN_READ_D(fd, filename)
        char *ptr = NULL;
        const int value = (int)strtol(buf, &ptr, 10) + (int)data;
        register const unsigned long len = count_numbers(value);

        CLOSE_ND(fd)

        register file_t buf_file = 0;
#ifdef _WIN32
        OPEN_WRITE_D(buf_file, filename, OPEN_EXISTING)

        char* _num = (char *)malloc(len + 1UL);
#else
        OPEN_WRITE_D(buf_file, filename, O_WRONLY)

        char _num[len + 1UL];
#endif

        itoa_d(value, _num);

#ifdef _WIN32
        WriteFileEx(buf_file, _num, len, &w_ol, 3);
        free(_num);
#else
        pwrite(buf_file, _num, len, 0);
#endif
        /* Frees memory */
        CLOSE_D(buf_file)
    }

    uiQuit();
}

int main(const int argc, file_fmt_t* argv) {
    if (argc > 1)
        filename = argv[1];

    uiInitOptions o = { 0 };
    if (uiInit(&o) == NULL) {
#ifdef _WIN32
        register const unsigned char isMenuBar = 1U;
#else
        register const unsigned char isMenuBar = 0U;
#endif

        register uiWindow *win = uiNewWindow("Timer", 550, 80, isMenuBar);
        uiWindowOnClosing(win, onClosing, NULL);
        uiOnShouldQuit(onShouldQuit, win);

        uiWindowSetMargined(win, 2);
        uiWindowSetBorderless(win, 0);

        uiBox *vbox = uiNewVerticalBox();
        uiBoxSetPadded(vbox, 1);
        uiWindowSetChild(win, uiControl(vbox));

        pbar = uiNewProgressBar();
        uiBoxAppend(vbox, uiControl(pbar), 0);

        /* TOP */
        uiBox *top_box = uiNewHorizontalBox();
        uiBoxAppend(vbox, uiControl(top_box), 0);

        /* Add buttons */
        uiButton *first_b = uiNewButton("Add 5 Minutes");
        uiButtonOnClicked(first_b, onAdd, (void*)5);
        uiBoxAppend(top_box, uiControl(first_b), 1);

        uiButton *second_b = uiNewButton("Add 10 Minutes");
        uiButtonOnClicked(second_b, onAdd, (void*)10);
        uiBoxAppend(top_box, uiControl(second_b), 1);


        /* Skip button */
        uiButton *skip = uiNewButton("Skip");
        uiButtonOnClicked(skip, onSkip, NULL);
        uiBoxAppend(top_box, uiControl(skip), 1);


        uiTimer(244, addTime, NULL);

        uiControlShow(uiControl(win));

        uiMain();
    }

    return 0;
}
