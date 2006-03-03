/*
 * $Id: lssum.c,v 1.1 2006/03/03 21:09:32 urs Exp $
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <openssl/md5.h>

void lssum(char *fname);

int main(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; i++)
	lssum(argv[i]);

    return 0;
}

void lssum(char *fname)
{
    unsigned char *h;
    int fd;
    struct stat st;
    void *addr;
    int i;
    char ts[sizeof("YYYY-MM-DD HH:MM:SS +0000")];
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
    printf("  %10d  %s  %s\n", st.st_size, ts, fname);
    close(fd);
    munmap(addr, st.st_size);
}
