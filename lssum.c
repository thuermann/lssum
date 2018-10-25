/*
 * $Id: lssum.c,v 1.22 2018/10/25 05:33:18 urs Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <openssl/md5.h>

#define BUFSIZE 65536

static int lssum(const char *fname);
static unsigned char *md5(const char *fname);
static char *hex(char *t, unsigned char *s, size_t len);

static int opt_ctime   = 0;
static int opt_user    = 0;
static int opt_group   = 0;

static void usage(const char *name)
{
    fprintf(stderr, "Usage: %s [-cug] files...\n", name);
}

int main(int argc, char **argv)
{
    int errflg = 0;
    int opt, i;

    setlocale(LC_ALL, "");

    while ((opt = getopt(argc, argv, "cug")) != -1) {
	switch (opt) {
	case 'c':
	    opt_ctime = 1;
	    break;
	case 'u':
	    opt_user = 1;
	    break;
	case 'g':
	    opt_group = 1;
	    break;
	default:
	    errflg = 1;
	    break;
	}
    }
    if (errflg) {
	usage(argv[0]);
	exit(1);
    }

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
    char id[2 * LOGIN_NAME_MAX + 1 + 1] = "";
    int id_cnt = 0;
    struct tm *tm;

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
    if (opt_user) {
	struct passwd *pw = getpwuid(st.st_uid);
	if (pw)
	    id_cnt = sprintf(id, "%-8s ", pw->pw_name);
	else
	    id_cnt = sprintf(id, "%8d ", st.st_uid);
    }
    if (opt_group) {
	struct group *gr = getgrgid(st.st_gid);
	if (gr)
	    sprintf(id + id_cnt, "%-8s ", gr->gr_name);
	else
	    sprintf(id + id_cnt, "%8d ", st.st_gid);
    }
    if (S_ISDIR(st.st_mode)) {
	printf("%-44s  %s%s  %s\n", "dir", id, ts, fname);
    } else if (S_ISLNK(st.st_mode)) {
	char sym[PATH_MAX];
	int  len = readlink(fname, sym, PATH_MAX - 1);
	if (len < 0) {
	    perror(fname);
	    return 1;
	}
	sym[len] = 0;
	printf("%-44s  %s%s  %s -> %s\n", "sym", id, ts, fname, sym);
    } else {
	char hashstr[2 * MD5_DIGEST_LENGTH + 1];
	unsigned char *hash;
	long long size = st.st_size;

	if (!(hash = md5(fname)))
	    return 1;

	hex(hashstr, hash, MD5_DIGEST_LENGTH);
	printf("%s  %10lld  %s%s  %s\n", hashstr, size, id, ts, fname);
    }
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

static char *hex(char *t, unsigned char *s, size_t len)
{
    size_t i;

    for (i = 0; i < len; i++)
	sprintf(t + 2 * i, "%02x", s[i]);

    return t;
}
