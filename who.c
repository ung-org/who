#define _XOPEN_SOURCE 700
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <utmpx.h>

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

char * utmpx_time(const struct utmpx *u)
{
	static char dt[32];
	time_t t = u->ut_tv.tv_sec;
	struct tm *tm = localtime(&t);
	strftime(dt, sizeof(dt), "%b %e %H:%M", tm);
	return dt;
}

char utmpx_state(const struct utmpx *u)
{
	char term[FILENAME_MAX];
	snprintf(term, sizeof(term), "/dev/%s", u->ut_line);
	struct stat st;

	if (stat(term, &st) == -1) {
		return '?';
	}

	if ((st.st_mode & S_IFCHR) != S_IFCHR) {
		return ' ';
	}

	if (S_IWGRP & st.st_mode) {
		return '+';
	}

	return '-';
}

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
			flags = QUICK;
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

	if (flags == 0 && argc == optind + 2) {
		if (strcmp("am", argv[optind]))
			return 1;

		if (strcmp("I", argv[optind+1]) && strcmp("i", argv[optind+1])) {
			return 1;
		}

		flags = SELF;
	}

	setutxent();
	struct utmpx *u = NULL;
	while ((u = getutxent()) != NULL) {
		if (flags == QUICK) {
			printf("%s\n", u->ut_user);
			continue;
		}

		if (u->ut_type == BOOT_TIME) {
			if (!(flags & BOOT)) {
				continue;
			}
			u->ut_user[0] = '\0';
			strcpy(u->ut_line, "system boot");
		}

		if (!strcmp(u->ut_user, "runlevel")) {
			if (!(flags & RUNLEVEL)) {
				continue;
			}
			u->ut_user[0] = '\0';
			strcpy(u->ut_line, "run-level");
		}

		printf("%-12s\t", u->ut_user);

		if (flags & TERMINAL) {
			if (u->ut_user[0] == '\0') {
				printf("  ");
			} else {
				printf("%c ", utmpx_state(u));
			}
		}

		printf("%-8s\t", u->ut_line);

		printf("%s", utmpx_time(u));

		if ((flags & TERMINAL) && u->ut_user[0] != '\0') {
			printf("\t");
			/* activity */
			printf("%d\t", u->ut_pid);
			/* comment */
			/* exit */
		}

		printf("\n");
	}

	return 0;
}
