 // includes 

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

//data

struct editorConfig {
	struct termios orig_termios;
};

struct editorConfig E;


// defines

#define CTRL_KEY(k) ((k) & 0x1f)

// data

// terminal

void die(const char *s) {
	write(STDOUT_FILENO, "\x1b[2j", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
 	perror(s);
 	exit(1);
}

void disableRawMode() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1);
}

void enableRawMode() {
	if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1);
	atexit(disableRawMode);

	struct termios raw = E.orig_termios;
	//disable terminal flags
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);//disable output processing
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

char editorReadKey() {
	int nread;
	char c;
	while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
		if (nread == -1 && errno != EAGAIN) die("read");
	}
	return c;
}

// output

void editorDrawRows() {
	int y;
	for (y = 0; y < 24; y++) {
		write(STDOUT_FILENO, "~\r\n", 3);
	}
}

void editorRefreshScreen() {
	write(STDOUT_FILENO, "\x1b[2j", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);

	editorDrawRows();

	write(STDOUT_FILENO, "\x1b[H", 3);
}

// input

void editorProcessKeypress() {
	char c = editorReadKey();

	switch (c) {
		case CTRL_KEY('q'):
			write(STDOUT_FILENO, "\x1b[2j", 4);
			write(STDOUT_FILENO, "\x1b[H", 3);
			exit(0);
			break;
	}
}

// init
int main() {
	enableRawMode();

	while (1) {
		editorRefreshScreen();
		editorProcessKeypress();
	}
	return 0;

}
