#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

enum { ESC_KEY = 27 };

static const char *base_name(const char *path) {
    const char *slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

static size_t strip_eol(char *s, size_t n) {
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        --n;
    }
    s[n] = '\0';
    return n;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        return 1;
    }

    const char *filepath = argv[1];
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        perror(filepath);
        return 1;
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int H = 0, W = 0;
    getmaxyx(stdscr, H, W);

    WINDOW *frame = newwin(H, W, 0, 0);
    if (!frame) {
        endwin();
        fclose(fp);
        fprintf(stderr, "Failed to create window\n");
        return 2;
    }

    int view_h = (H > 2) ? (H - 2) : 0;
    int view_w = (W > 2) ? (W - 2) : 0;
    WINDOW *view = NULL;
    if (view_h > 0 && view_w > 1) {
        view = derwin(frame, view_h, view_w, 1, 1);
    }
    if (!view) {
        delwin(frame);
        endwin();
        fclose(fp);
        fprintf(stderr, "Window too small\n");
        return 2;
    }

    keypad(view, TRUE);
    scrollok(view, TRUE);

    box(frame, 0, 0);
    const char *title = base_name(filepath);
    int title_room = W - 2;
    if (title_room > 0) {
        char buf[1024];
        snprintf(buf, sizeof(buf), "%s", title);
        if ((int)strlen(buf) > title_room) {
            buf[title_room] = '\0';
        }
        mvwprintw(frame, 0, 1, "%s", buf);
    }
    wrefresh(frame);

    int rows, cols;
    getmaxyx(view, rows, cols);
    int max_print_cols = (cols > 0) ? (cols - 1) : 0;

    char *line = NULL;
    size_t cap = 0;
    ssize_t nread = 0;

    int row = 0;
    while (row < rows && (nread = getline(&line, &cap, fp)) != -1) {
        size_t len = (size_t)nread;
        len = strip_eol(line, len);
        int to_print = (int)((len < (size_t)max_print_cols) ? len : (size_t)max_print_cols);
        if (to_print > 0) {
            mvwaddnstr(view, row, 0, line, to_print);
        }
        wmove(view, row, to_print);
        wclrtoeol(view);
        row++;
    }
    wrefresh(view);

    int at_eof = (nread == -1);
    for (;;) {
        int ch = wgetch(view);
        if (ch == ERR) {
            continue;
        }
        if (ch == ESC_KEY) {
            break;
        }
        if (ch == ' ') {
            if (at_eof) {
                continue;
            }
            nread = getline(&line, &cap, fp);
            if (nread == -1) {
                at_eof = 1;
                continue;
            }
            size_t len = (size_t)nread;
            len = strip_eol(line, len);
            int to_print = (int)((len < (size_t)max_print_cols) ? len : (size_t)max_print_cols);

            wscrl(view, 1);
            int last = rows - 1;
            if (to_print > 0) {
                mvwaddnstr(view, last, 0, line, to_print);
            }
            wmove(view, last, to_print);
            wclrtoeol(view);
            wrefresh(view);
        }
    }

    free(line);
    delwin(view);
    delwin(frame);
    endwin();
    fclose(fp);
    return 0;
}
