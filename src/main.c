#include "../include/types.h" /* file_t, title_t */
#include "../include/helpers.h" /* itoa_d, count_numbers */
#include "../include/file_manager.h" /* OPEN_*_D, CLOSE_D */

#include <stdlib.h> /* strtol */
#include <string.h> /* strlen */

#ifdef __linux__
#include <gtk/gtk.h>
#endif
#include <ui.h> /* uiProgressBar, uiMain, uiQuit.. */

static const char* title = NULL;

#ifdef _WIN32
static wchar_t* filename = NULL;
#else
static const char* filename = NULL;
#endif

static int timeout = 0;

static uiProgressBar *pbar = NULL;
static int progress_value = 0;

static inline void load_style(void) {
#ifdef __linux__
    const char* env = getenv("HOME");
    const char path[29] = "/.config/chill_app/style.css";
    strncat((char *)env, path, 29UL);

    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GError *error = NULL;
    gtk_css_provider_load_from_path(provider, (gchar*)env, &error);
    g_object_unref(provider);
#endif
}

static inline int addTime(UNUSED void* data) {
    if (progress_value != 100) {
        ++progress_value;

        uiProgressBarSetValue(pbar, progress_value);
    } else {
        uiQuit();
    }

    return 1;
}

static inline int onClosing(UNUSED uiWindow *w,
                            UNUSED void* data) {
    uiQuit();
    return 1;
}

static inline int onShouldQuit(void* data) {
    uiWindow *mainwin = uiWindow(data);

    uiControlDestroy(uiControl(mainwin));

    return 1;
}

static inline void onSkip(UNUSED uiButton *b,
                          UNUSED void* data) {
    uiQuit();
}

static void onAdd(UNUSED uiButton *b, void* data) {
    TYPE file_t fd = (file_t)0;

    OPEN_READ_D(fd, filename)
        char *ptr = NULL;
        CTYPE int value = (int)strtol(buf, &ptr, 10) + (intptr_t)data;
        CTYPE size_t len = count_numbers(value) + 1UL;

        CLOSE_ND(fd)

        TYPE file_t buf_file = (file_t)0;
#ifdef _WIN32
        OPEN_WRITE_D(buf_file, filename, OPEN_EXISTING)

        char* _num = (char *)malloc(len);
#else
        OPEN_WRITE_D(buf_file, filename, O_WRONLY)

        char _num[len];
#endif

        itoa_d(value, _num);

#ifdef _WIN32
        WriteFileEx(buf_file, _num, len, &w_ol, NULL);
        free(_num);
#else
        pwrite(buf_file, _num, len, 0);
#endif
        CLOSE_D(buf_file)
    }

#ifdef WIN32
    /* Frees buffer */
    free(filename);
#endif
    uiQuit();
}

static unsigned char initArgs(const unsigned __argc_param,
                              const char** __argv_param) {
    TYPE uint8_t result = 1U;

    TYPE unsigned i = 1U;
    while (i < __argc_param) {
        const char* p_next = __argv_param[i + 1U];
        const char* buf = __argv_param[i];
        CTYPE size_t buf_l = strlen(buf);

        if (p_next == NULL) {
            result = 1U;
            break;
        } else if (p_next[0] == '-') {
            result = 1U;
            break;
        }

        if (buf_l == 2UL) {
        switch (buf[1]) {
        case 'n':
            title = p_next;

            result = 0U;
            ++i;
            break;
        case 't': {
                char *ptr = NULL;
                timeout = (int)strtol(p_next,
                                      &ptr, 10UL);

                result = 0U;
                ++i;
            }
            break;
        case 'f': {
#ifdef _WIN32
                CTYPE size_t buf_len = strlen(p_next) + 1UL;
                filename = (title_t)malloc(buf_len);
                mbstowcs_s(NULL, filename, buf_len,
                           p_next, buf_len);
#else
                filename = p_next;
#endif
                result = 0U;
                ++i;
            }
            break;
        default: break;
        }
        }
        ++i;
    }

    return result;
}

int main(const int argc, const char** argv) {
    TYPE uint8_t res_init = 0U;
    if (argc > 1)
        res_init = initArgs((const unsigned)argc, argv);

    uiInitOptions o = { 0 };
    if ((!res_init) && (uiInit(&o) == NULL)) {
#ifdef _WIN32
        CTYPE uint8_t isMenuBar = 1U;
#else
        CTYPE uint8_t isMenuBar = 0U;
#endif

        load_style();
        TYPE uiWindow *win = uiNewWindow((title == NULL) ? "Timer" : title,
                                         550, 80, isMenuBar);
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
        uiButton *first_b = uiNewButton("Postpone 5 Minutes");
        uiButtonOnClicked(first_b, onAdd, (void*)5);
        uiBoxAppend(top_box, uiControl(first_b), 1);

        uiButton *second_b = uiNewButton("Postpone 10 Minutes");
        uiButtonOnClicked(second_b, onAdd, (void*)10);
        uiBoxAppend(top_box, uiControl(second_b), 1);


        /* Skip button */
        uiButton *skip = uiNewButton("Skip");
        uiButtonOnClicked(skip, onSkip, NULL);
        uiBoxAppend(top_box, uiControl(skip), 1);

        uiTimer((timeout == 0) ? 150 : (10 * timeout),
                addTime, NULL);

        uiControlShow(uiControl(win));
        uiMain();
    }

    return 0;
}
