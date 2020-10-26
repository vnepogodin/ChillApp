#include "../include/file_manager.h"
#include "../include/helpers.h"

#include <stdlib.h> /* strtol */
#include <string.h> /* strlen */

#include <ui.h> /* uiProgressBar, uiMain, uiQuit.. */

static file_fmt_t title = NULL;
static file_fmt_t filename = NULL;
static int timeout = 0;

static uiProgressBar *pbar = NULL;
static int progress_value = 0;

static inline int addTime(UNUSED void* data) {
    if (progress_value != 100) {
        ++progress_value;

        uiProgressBarSetValue(pbar, progress_value);
    } else {
        uiQuit();
    }

    return 1;
}

static inline int onClosing(UNUSED uiWindow *w, UNUSED void* data) {
    uiQuit();
    return 1;
}

static inline int onShouldQuit(void* data) {
    uiWindow *mainwin = uiWindow(data);

    uiControlDestroy(uiControl(mainwin));

    return 1;
}

static inline void onSkip(UNUSED uiButton *b, UNUSED void* data) {
    uiProgressBarSetValue(pbar, 100);
    uiQuit();
}

static void onAdd(UNUSED uiButton *b, void* data) {
    register file_t fd = (file_t)0;

    OPEN_READ_D(fd, filename)
        char *ptr = NULL;
        register const int value = (int)strtol(buf, &ptr, 10) + (intptr_t)data;
        register const unsigned long len = count_numbers(value) + 1UL;

        CLOSE_ND(fd)

        register file_t buf_file = (file_t)0;
#ifdef _WIN32
        OPEN_WRITE_D(buf_file, filename, OPEN_EXISTING)

        char* _num = (char *)malloc(len);
#else
        OPEN_WRITE_D(buf_file, filename, O_WRONLY)

        char _num[len];
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

static unsigned char initArgs(const unsigned __argc_param, const char** __argv_param) {
    register unsigned char result = 1U;

    register unsigned i = 1U;
    while (i < __argc_param) {
        if (__argv_param[i + 1U] == NULL) {
            result = 1U;
            break;
        } else if (__argv_param[i + 1U][0] == '-') {
            result = 1U;
            break;
        }

        if (strlen(__argv_param[i]) == 2UL) {
        switch (__argv_param[i][1]) {
        case 'n':
            title = __argv_param[i + 1U];

            result = 0U;
            ++i;
            break;
        case 't': {
                char *ptr = NULL;
                register const int value = (int)strtol(__argv_param[i + 1U], &ptr, 10UL);
                timeout = value;

                result = 0U;
                ++i;
            }
            break;
        case 'f':
            filename = __argv_param[i + 1U];

            result = 0U;
            ++i;
            break;
        default: break;
        }
        }
        ++i;
    }

    return result;
}

int main(const int argc, const char** argv) {
#ifdef _WIN32
    FreeConsole();
#endif

    register unsigned char res_init = 0U;
    if (argc > 1)
        res_init = initArgs((const unsigned)argc, argv);

    uiInitOptions o = { 0 };
    if ((!res_init) && (uiInit(&o) == NULL)) {
#ifdef _WIN32
        register const unsigned char isMenuBar = 1U;
#else
        register const unsigned char isMenuBar = 0U;
#endif

        register uiWindow *win = uiNewWindow((title == NULL) ? "Timer" : title, 550, 80, isMenuBar);
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

        uiTimer((timeout == 0) ? 243 : (16.3 * timeout), addTime, NULL);

        uiControlShow(uiControl(win));
        uiMain();
    }

    return 0;
}
