// fuck with terminal windows idk lol
// certified luna banger

#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <errno.h>

bool canUseIoctl();
void resizeWithIoctl(int, int);
void resizeWithAnsi(int, int);
void resizeTerminal(int, int);
int msleep(long);
void enforceSize(short rows, short columns);

struct TerminalSize {
    unsigned short rows;
    unsigned short columns;
};

int main (void) {
    while(true) {
        enforceSize(24, 80);
    }
    return 0;
}

bool canUseIoctl(void) {
    struct winsize w;
    return ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0;
}

void resizeWithIoctl(int rows, int cols) {
    struct winsize w;
    w.ws_row = rows;
    w.ws_col = cols;
    w.ws_xpixel = 0;
    w.ws_ypixel = 0;
    ioctl(STDOUT_FILENO, TIOCSWINSZ, &w);
}

void resizeWithAnsi(int rows, int cols) {
    printf("\033[8;%d;%dt", rows, cols);
    fflush(stdout); // print that hoe NEEEEOW
}

void resizeTerminal(int rows, int cols) {
    bool useIoctl = canUseIoctl();
    useIoctl ? resizeWithIoctl(rows, cols) : resizeWithAnsi(rows, cols);
    std::cout << (canUseIoctl ? "Resized with IOCtl." : "Resized with ANSI.") << std::endl;
}

// custom sleep function to allow for millisecond-expressed process inturruption times
int msleep(long msec) {
    struct timespec ts; // takes time_t tv_sec and long tv_nsec
    int res; // resolution status output code

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    } // invalid input handling

    ts.tv_sec = msec / 1000; // nanoseconds since the POSIX epoch, 00:00, January 1st, 1970 UTC
    ts.tv_nsec = (msec % 1000) * 1000000; // nanosecond equivalent of provided milliseconds

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

void enforceSize(short rows, short columns) {
    // determine current size of terminal
    struct winsize w;
    TerminalSize termWindow;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        std::cerr << "Error | your OS may not respect IOCtl, bummer :/" << std::endl;
        exit(1);
    }

    termWindow.rows = w.ws_row;
    termWindow.columns = w.ws_col;

    if (termWindow.columns != columns) {
        msleep(1000);
        if (termWindow.columns < columns) {
            for (short i = termWindow.columns; i < columns; i++) {
                resizeWithAnsi(24, i);
                msleep(25);
            }
        } else {
            for (short i = termWindow.columns; i > columns; i--) {
                resizeWithAnsi(24, i);
                msleep(25);
            }
        }
    }
    // todo concurrently check for row number and use parallel process scheduling to do it

    return;
}
