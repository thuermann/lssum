/*
 * $Id: lssum.c,v 1.18 2016/01/08 13:09:26 urs Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <openssl/md5.h>

#define BUFSIZE 65536

static int lssum(const char *fname);
static unsigned char *md5(const char *fname);

static int opt_mtime   = 0;
static int opt_ctime   = 0;
static int opt_user    = 0;
static int opt_group   = 0;
static int opt_verbose = 0;

static void usage(const char *name)
{
    fprintf(stderr, "Usage: %s [-mcugv] files...\n", name);
    exit(1);
}

int main(int argc, char **argv)
{
    int errflg = 0;
    int opt, i;

    setlocale(LC_ALL, "");

    while ((opt = getopt(argc, argv, "mcugv")) != -1) {
	switch (opt) {
	case 'm':
	    opt_mtime = 1;
	    break;
	case 'c':
	    opt_ctime = 1;
	    break;
	case 'u':
	    opt_user = 1;
	    break;
	case 'g':
	    opt_group = 1;
	    break;
	case 'v':
	    opt_verbose = 1;
	    break;
	default:
	    errflg = 1;
	    break;
	}
    }
    if (errflg)
	usage(argv[0]);

    for (i = optind; i < argc; i++)
	if (lssum(argv[i]))
	    errflg = 1;

    return errflg;
}

static int lssum(const char *fname)
{
#define TIMEFMT  "%F %T %z"
#define TIMESIZE (sizeof("YYYY-MM-DD HH:MM:SS +0000") - 1)

    struct stat st;
    char ts[2 * TIMESIZE + 1 + 1];
    struct tm *tm;
    unsigned char *hash;
    int i;

    if (lstat(fname, &st) < 0) {
	perror(fname);
	return 1;
    }
    tm = localtime(&st.st_mtime);
    strftime(ts, sizeof(ts), TIMEFMT, tm);
    if (opt_ctime) {
	tm = localtime(&st.st_ctime);
	strftime(ts + TIMESIZE, sizeof(ts) - TIMESIZE, " " TIMEFMT, tm);
    }
    if (S_ISDIR(st.st_mode)) {
	printf("%-44s  %s  %s\n", "dir", ts, fname);
	return 0;
    } else if (S_ISLNK(st.st_mode)) {
	char sym[PATH_MAX];
	int  len = readlink(fname, sym, PATH_MAX - 1);
	if (len < 0) {
	    perror(fname);
	    return 1;
	}
	sym[len] = 0;
	printf("%-44s  %s  %s -> %s\n", "sym", ts, fname, sym);
	return 0;
    }

    if (!(hash = md5(fname)))
	return 1;

    for (i = 0; i < MD5_DIGEST_LENGTH; i++)
	printf("%02x", hash[i]);
    printf("  %10lld  %s  %s\n", (long long)st.st_size, ts, fname);

    return 0;
}

static unsigned char *md5(const char *fname)
{
    static unsigned char hash[MD5_DIGEST_LENGTH];
    unsigned char buffer[BUFSIZE];
    MD5_CTX ctx;
    int fd, nbytes;

    if ((fd = open(fname, O_RDONLY)) < 0) {
	perror(fname);
	return NULL;
    }

    MD5_Init(&ctx);
    while ((nbytes = read(fd, buffer, sizeof(buffer))) > 0)
	MD5_Update(&ctx, buffer, nbytes);
    if (nbytes < 0) {
	perror(fname);
	return NULL;
    }
    MD5_Final(hash, &ctx);

    close(fd);

    return hash;
}
