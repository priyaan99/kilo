/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/

struct termios original_termios;

/*** terminal ***/

void die(const char *s) {
    // clear screen
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
} 

void disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1) {
        die("tcsetattr");
    }
}

void enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1) die("tcgetattr");
    atexit(disable_raw_mode);

    struct termios raw = original_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /// VMIN value sets the minimum number of bytes of input needed before read() can return.
    /// 0 so that read() returns as soon as there is any input to be read.
    raw.c_cc[VMIN] = 0;
    /// VTIME value sets the maximum amount of time to wait before read() returns
    /// It is in tenths of a second, so we set it to 1/10 of a second, or 100 milliseconds.
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

char editor_read_key() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) {
            die("read");
        }
    }
    return c;
}

/*** output ***/

void editor_refresh_screen() {
    // clear screen
    write(STDOUT_FILENO, "\x1b[2J", 4);
    // reposition the cursor
    write(STDOUT_FILENO, "\x1b[H", 3);
}

/*** input ***/

void editor_process_keypress() {
    char c = editor_read_key();

    switch (c) {
        case CTRL_KEY('q'): {
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
        }
        break;
    }
}

/*** init ***/

int main() {
    enable_raw_mode();

    while (1) {
        editor_refresh_screen();
        editor_process_keypress();
    }

    return 0;
}
