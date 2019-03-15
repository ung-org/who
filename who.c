#define _XOPEN_SOURCE
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

enum {
	BOOT	= 1<<0,
	DEAD	= 1<<1,
	HEADER	= 1<<2,
	LOGIN	= 1<<3,
	SELF	= 1<<4,
	INIT	= 1<<5,
	QUICK	= 1<<6,
	RUNLEVEL= 1<<7,
	TIME	= 1<<8,
	TERMINAL= 1<<9,
	IDLE	= 1<<10,
};

int main(int argc, char *argv[])
{
	int flags = 0;

	setlocale(LC_ALL, "");

	int c;
	while ((c = getopt(argc, argv, "abdHlmpqrstTu")) != -1) {
		switch (c) {
		case 'a':
			/* -bdlprtTu */
			flags |= BOOT;
			flags |= DEAD;
			flags |= LOGIN;
			flags |= INIT;
			flags |= RUNLEVEL;
			flags |= TIME;
			flags |= TERMINAL;
			flags |= IDLE;
			break;

		case 'b':
			flags |= BOOT;
			break;

		case 'd':
			flags |= DEAD;
			break;

		case 'H':
			flags |= HEADER;
			break;

		case 'l':
			flags |= LOGIN;
			break;

		case 'm':
			flags |= SELF;
			break;

		case 'p':
			flags |= INIT;
			break;

		case 'q':
			flags |= QUICK;
			break;

		case 'r':
			flags |= RUNLEVEL;
			break;

		case 's':
			/* default */
			break;

		case 't':
			flags |= TIME;
			break;

		case 'T':
			flags |= TERMINAL;
			break;

		case 'u':
			flags |= IDLE;
			break;

		default:
			return 1;
		}
	}

	if (flags & QUICK) {
		flags = QUICK;
	}

	if (flags == 0 && argc == optind + 2) {
		if (strcmp("am", argv[optind]))
			return 1;

		if (strcmp("I", argv[optind+1]) && strcmp("i", argv[optind+1])) {
			return 1;
		}

		printf("ok\n");
		flags = SELF;
	}

	return 0;
}
