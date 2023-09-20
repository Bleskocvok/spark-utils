
#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <stdio.h>      // printf
#include <stdlib.h>
#include <string.h>     // strncat, strdup, strncpy, strcmp, strstr
#include <errno.h>      // errno

#include <unistd.h>     // readlink, close
#include <limits.h>     // PATH_MAX
#include <libgen.h>     // dirname
#include <sys/types.h>  // open
#include <sys/stat.h>   // open
#include <fcntl.h>      // open

#include <limits.h>     // realpath
#include <stdlib.h>     // realpath


void close_dir(int fd)
{
    if (fd == -1 || fd == AT_FDCWD)
        return;

    if (close(fd) == -1) perror("close");
}


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
    int new_dir = -1;
    char* dir = NULL;
    char* dir_buf = NULL;

    int rv = 1;

    while (1)
    {
        if (dir && current[0] != '/')
        {
            printf("%s/", dir);
        }

        printf("%s\n", current);

        new_dir = dir_fd;
        char* slash = strstr(current, "/");

        if (slash != NULL)
        {
            free(dir_buf);
            dir_buf = strdup(current);
            if (!dir_buf)
                perror("alloc"), exit(2);

            dir = dirname(dir_buf);
            new_dir = openat(dir_fd, dir, O_RDONLY | O_DIRECTORY | O_NOFOLLOW, 0);

            if (new_dir == -1)
            {
                perror("openat");
                fprintf(stderr, "file: %s\n", dir);
                goto error;
            }
        }

        ssize_t r;
        if ((r = readlinkat(dir_fd, current, buffer, buflen)) == -1)
        {
            if (errno != EINVAL)
            {
                perror("error");
                goto error;
            }
            // successfully exitting
            break;
        }

        if (new_dir != dir_fd)
            close_dir(dir_fd);
        dir_fd = new_dir;
        buffer[r] = 0;

        strncpy(current, buffer, buflen);

        if (strcmp(current, detector) == 0)
        {
            fprintf(stderr, "error: cyclic symlinks\n");
            goto error;
        }

        if (++i == next)
        {
            next *= 2;
            strncpy(detector, buffer, buflen);
        }
    }

    rv = 0;
error:
    close_dir(dir_fd);
    if (new_dir != dir_fd) close_dir(new_dir);
    free(dir_buf);
    return rv;
}
