
#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <stdio.h>      // printf
#include <stdlib.h>
#include <string.h>     // strncat, strdup, strncpy, strcmp, strstr
#include <errno.h>      // errno

#include <unistd.h>     // readlink
#include <limits.h>     // PATH_MAX
#include <libgen.h>     // dirname
#include <sys/types.h>  // open
#include <sys/stat.h>   // open
#include <fcntl.h>      // open

#include <limits.h>     // realpath
#include <stdlib.h>     // realpath



int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s FILE\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];

    char buffer[PATH_MAX + 1] = { 0 };
    char current[PATH_MAX + 1] = { 0 };
    strncat(current, filename, PATH_MAX);
    size_t buflen = PATH_MAX;

    char detector[PATH_MAX + 1] = { 0 };
    size_t i = 0;
    size_t next = 1;

    int dir_fd = AT_FDCWD;
    char* dir = NULL;
    char* dir_buf = NULL;

    while (1)
    {
        if (dir && current[0] != '/')
        {
            printf("%s/", dir);
        }

        printf("%s\n", current);

        int new_dir = dir_fd;
        char* slash = strstr(current, "/");

        if (slash != NULL)
        {
            free(dir_buf);
            dir_buf = strdup(current);
            if (!dir_buf)
                perror("alloc"), exit(2);
            dir = dirname(dir_buf);
            new_dir = open(dir, O_PATH | O_NOFOLLOW);
        }

        ssize_t r;
        if ((r = readlinkat(dir_fd, current, buffer, buflen)) == -1)
        {
            free(dir_buf);
            if (errno != EINVAL)
            {
                perror("error");
                return 1;
            }
            return 0;
        }

        dir_fd = new_dir;
        buffer[r] = 0;

        strncpy(current, buffer, buflen);

        if (strcmp(current, detector) == 0)
        {
            free(dir);
            fprintf(stderr, "error: cyclic symlinks\n");
            return 1;
        }

        if (++i == next)
        {
            next *= 2;
            strncpy(detector, buffer, buflen);
        }
    }

    free(dir);
    return 1;
}
