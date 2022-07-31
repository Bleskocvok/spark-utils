
// lrand48, srand48
#define _XOPEN_SOURCE
// fstatat
#define _POSIX_C_SOURCE 200809L

#include <unistd.h>         // exec, sleep
#include <sys/wait.h>       //       wait
#include <sys/types.h>      // fork, wait, opendir, dirfd, *stat*
#include <sys/stat.h>       //                             *stat*
#include <unistd.h>         // fork                        *stat*
#include <dirent.h>         //             opendir, dirfd
#include <stdlib.h>         // lrand48, srand48
#include <signal.h>         // sig_atomic_t, SIG*, sigaction, sigemptyset,
                            // sigaddset, sigprocmask, sigsuspend
#include <sys/time.h>       // setitimer
#include <syslog.h>         // syslog, openlog

#include <time.h>           // time
#include <stdio.h>          // *printf
#include <stdlib.h>         // strtol, EXIT_*, NULL, malloc
#include <errno.h>          // errno
#include <stdarg.h>         // va_args
#include <string.h>         // strlen, strcmp, memcpy, strcat


#define SIZE(lst) \
    (sizeof(lst) / sizeof(*(lst)))


char* prepend_uri(const char* uri)
{
    static const char prefix[] = "file://";

    size_t len = strlen(uri);

    char* data = malloc(len + SIZE(prefix));
    if (!data)
        return NULL;

    data[0] = '\0';

    strcat(data, prefix);
    strcat(data, uri);
    return data;
}


int change_wallpaper(char* path)
{
    pid_t pid = fork();

    if (pid == -1)
        return 1;

    if (pid == 0)
    {
        // need to add 'file://' prefix before the absolute path
        char* uri = prepend_uri(path);

        // ubuntu 22 - dark theme
        execlp("gsettings", "gsettings", "set", "org.gnome.desktop.background",
               "picture-uri-dark", uri, NULL);

        // // ubuntu 20.4
        // execlp("gsettings", "gsettings", "set", "org.gnome.desktop.background",
        //        "picture-uri", path, NULL);
        return 1;
    }

    int status;
    if (waitpid(pid, &status, 0))
        return 1;

    return 0;
}


void error(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}


long uniform_random(long min, long max)
{
    const long LRAND48_MAX = (1l << 31l) - 1l;  // is 2^31 - 1

    max = min > max ? min : max;
    long k = max - min + 1;

    long mod = (LRAND48_MAX + 1) % k;

    long r;
    do
    {
        r = lrand48();

    } while (r > LRAND48_MAX - mod);

    return (r % k) + min;
}


char* concat_alloc(char* path_a, char* path_b)
{
    if (!path_a || !path_b)
        return NULL;

    size_t len_a = strlen(path_a);
    size_t len_b = strlen(path_b);

    char* res = malloc(len_a + len_b + 2);

    memcpy(res,         path_a, len_a);
    memcpy(res + len_a, "/", 1);
    memcpy(res + len_a + 1, path_b, len_b + 1);

    return res;
}


typedef struct
{
    char* file;
    char* folder;
    int i;
    int dir_fd;
} file_info;


int for_each_file(char* folder,
                  // func is expected to return 1/0 to indicate
                  // continue/stop iteration
                  int (* func) (file_info*, void*),
                  void* arg)
{
    if (func == NULL)
        return 1;

    DIR* dir = opendir(folder);

    if (dir == NULL)
        return 1;

    file_info info =
    {
        .file = NULL,
        .folder = folder,
        .i = 0,
        .dir_fd = dirfd(dir),
    };

    while (1)
    {
        errno = 0;
        struct dirent* ent = readdir(dir);

        if (ent == NULL && errno != 0)
            return closedir(dir), 1;

        if (ent == NULL)
            break;

        info.file = ent->d_name;

        if (!func(&info, arg))
            break;

        info.i++;
    }

    closedir(dir);
    return 0;
}


int is_regular(int dfd, char* filename)
{
    if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
        return 0;

    struct stat st;

    if (fstatat(dfd, filename, &st, 0) == -1)
       return 0;

    return S_ISREG(st.st_mode);
}


int counter(file_info* info, void* ptr)
{
    if (is_regular(info->dir_fd, info->file))
        *(int*) ptr += 1;

    return 1;
}


int count_files(char* folder)
{
    int count = 0;
    for_each_file(folder, counter, &count);
    return count;
}


typedef struct
{
    char* result;
    int at;
    int curr;
} tra_t;


int traverse(file_info* info, void* ptr)
{
    tra_t* tra = (tra_t*) ptr;

    if (!is_regular(info->dir_fd, info->file))
        return 1;

    if (tra->curr == tra->at)
    {
        tra->result = concat_alloc(info->folder, info->file);
        return 0;
    }

    tra->curr++;

    return 1;
}


char* file_at_idx(char* folder, int idx)
{
    tra_t tra =
    {
        .result = NULL,
        .at = idx,
        .curr = 0,
    };
    for_each_file(folder, traverse, &tra);
    return tra.result;
}



static const int term_sigs[] =
{
    SIGTERM, SIGINT, SIGABRT, SIGQUIT, SIGALRM,
};


volatile sig_atomic_t awoken = 0;
volatile sig_atomic_t termed = 0;


void handler(int sig, siginfo_t* info, void* ucontext)
{
    (void) info;
    (void) ucontext;

    if (sig == SIGALRM)
    {
        awoken = 1;
        return;
    }
    termed = 1;
}


void setup_term_signals()
{
    for (size_t i = 0; i < SIZE(term_sigs); i++)
    {
        struct sigaction act =
        {
            .sa_sigaction = handler,
            .sa_flags = SA_SIGINFO,
        };
        sigaction(term_sigs[i], &act, NULL);
    }
}


int setup_timer(long seconds)
{
    struct itimerval interval =
    {
        .it_value = { .tv_sec = seconds, .tv_usec = 0 }
    };
    return setitimer(ITIMER_REAL, &interval, NULL);
}


int main(int argc, char** argv)
{
    if (argc < 3)
        return error("usage: %s MINUTES WALLPAPER_DIR\n",
                     argc > 0 ? argv[0] : "./wallpie"),
               EXIT_FAILURE;

    char* end;
    errno = 0;
    long minutes = strtol(argv[1], &end, 10);
    int err = errno;
    if (err == ERANGE)
        return error("number too large\n"), EXIT_FAILURE;
    if (end == argv[1] || err != 0)
        return error("invalid number\n"), EXIT_FAILURE;

    setup_term_signals();

    sigset_t set,
             old;
    sigemptyset(&set);

    for (size_t i = 0; i < SIZE(term_sigs); i++)
        sigaddset(&set, term_sigs[i]);

    if (sigprocmask(SIG_BLOCK, &set, &old) == -1)
        return perror("sigprocmask"), EXIT_FAILURE;

    openlog(argv[0], 0, 0);
    syslog(LOG_INFO, "(%s) started", argv[0]);

    char* folder = argv[2];

    srand48(time(NULL));

    char* prev = NULL;
    while (1)
    {
        setup_timer(minutes * 60);
        sigsuspend(&old);

        if (termed)
            break;

        int count = count_files(folder);
        char* path = NULL;

        if (count < 1)
            continue;

        while (1)
        {
            long idx = uniform_random(0, count - 1);
            path = file_at_idx(folder, idx);

            if (!path)
                break;

            if (count > 1 && prev != NULL && strcmp(path, prev) == 0)
            {
                free(path);
                continue;
            }

            change_wallpaper(path);
            syslog(LOG_INFO, "change background: '%s'", path);
            break;
        }
        free(prev);
        prev = path;
    }

    free(prev);

    return 0;
}
