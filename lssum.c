/*
 * $Id: lssum.c,v 1.4 2007/12/20 13:11:35 urs Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <openssl/md5.h>

static void lssum(char *fname);

static int opt_mtime   = 0;
static int opt_ctime   = 0;
static int opt_user    = 0;
static int opt_group   = 0;
static int opt_verbose = 0;

static void usage(char *name)
{
    fprintf(stderr, "Usage: %s [-m] [-c] [-u] [-g] [-v] files...\n", name);
    exit(1);
}

int main(int argc, char **argv)
{
    int errflg = 0;
    int opt, i;

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
	lssum(argv[i]);

    return 0;
}

static void lssum(char *fname)
{
    unsigned char *h;
    int fd;
    struct stat st;
    void *addr;
    int i;
    char ts[sizeof("YYYY-MM-DD HH:MM:SS +0000 YYYY-MM-DD HH:MM:SS +0000")];
    struct tm *tm;

    if ((fd = open(fname, O_RDONLY)) < 0) {
	perror(fname);
	return;
    }
    if (fstat(fd, &st) < 0) {
	perror("stat");
	return;
    }
    tm = localtime(&st.st_mtime);
    strftime(ts, sizeof(ts), "%F %T %z", tm);
    if (opt_ctime) {
	tm = localtime(&st.st_ctime);
	strftime(ts + 25, sizeof(ts) - 25, " %F %T %z", tm);
    }
    if (S_ISDIR(st.st_mode)) {
	printf("%-44s  %s  %s\n", "dir", ts, fname);
	return;
    }
    if ((addr = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)) < 0) {
	perror("mmap");
	return;
    }

    h = MD5(addr, st.st_size, NULL);
    for (i = 0; i < 16; i++)
	printf("%02x", h[i]);
    printf("  %10lld  %s  %s\n", (long long)st.st_size, ts, fname);
    close(fd);
    munmap(addr, st.st_size);
}
