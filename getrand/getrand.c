
// lrand48, srand48
#define _XOPEN_SOURCE
#define _DEFAULT_SOURCE

#include <stddef.h>         // NULL
#include <stdio.h>          // fopen, perror, fprintf, stderr
#include <errno.h>          // errno
#include <stdlib.h>         // strtol
#include <string.h>         // strcmp

// posix
#include <stdlib.h>         // lrand48, srand48



//
// ORIGINAL DISTRIBUTION FUNCTIONS
//


long uniform_random(long min, long max)
{
    const long LRAND48_MAX = (1l << 31l) - 1l;  // is 2^31 - 1

    long k = max - min + 1;

    long mod = (LRAND48_MAX + 1) % k;

    long r;
    do
    {
        r = lrand48();

    } while (r > LRAND48_MAX - mod);

    return (r % k) + min;
}


long naive_random(long min, long max)
{
    long k = max - min + 1;
    long r = lrand48();
    return (r % k) + min;
}


//
// “OPTIMIZED” DISTRIBUTION FUNCTIONS
//


typedef struct
{
    long treshold,
         k,
         min,
         max,
         mod;

} uniform_t;


typedef struct
{
    long min,
         max,
         k;

} naive_t;


void uniform_init(uniform_t* out, long min, long max)
{
    const long LRAND48_MAX = (1l << 31l) - 1l;

    *out = (uniform_t){ 0 };

    out->min = min;
    out->max = max;
    out->k = max - min + 1;
    out->mod = (LRAND48_MAX + 1) % out->k;
    out->treshold = LRAND48_MAX - out->mod;
}


long uniform_get(const uniform_t* out)
{
    long r;
    do
    {
        r = lrand48();

    } while (r > out->treshold);

    return (r % out->k) + out->min;
}


void naive_init(naive_t* out, long min, long max)
{
    *out = (naive_t){ 0 };
    out->min = min;
    out->max = max;
    out->k = max - min + 1;
}


long naive_get(const naive_t* out)
{
    long r = lrand48();
    return (r % out->k) + out->min;
}



int get_seed(long* num, const char* filename)
{
    FILE* file = fopen(filename, "r");

    if (!file)
        return -1;

    long result = 0;

    if (fread(&result, sizeof(result), 1, file) != 1)
    {
        int prerrno = errno;
        fclose(file);
        errno = prerrno;
        return -1;
    }

    if (num)
        *num = result;

    return fclose(file);
}


int main(int argc, char** argv)
{
    if (argc <= 2)
    {
        fprintf(stderr, "usage: %s [--bad] MIN MAX [COUNT]\n", argv[0]);
        return 1;
    }

    int end_arg = 0;

    int flag_use_bad = 0;

    if (argc >= 1 && strcmp(argv[1], "--bad") == 0)
    {
        flag_use_bad = 1;
        end_arg = 1;
    }

    long min = 0,
         max = 0,
         count = 1;

    min = strtol(argv[end_arg + 1], NULL, 0);
    max = strtol(argv[end_arg + 2], NULL, 0);

    if (argc - end_arg >= 4)
        count = strtol(argv[end_arg + 3], NULL, 0);

    if (max < min || min < 0)
    {
        fprintf(stderr, "usage: %s MIN MAX\n"
                        "where: 0 <= MIN <= MAX\n", argv[0]);
        return 1;
    }

    long seed = 0;

    if (get_seed(&seed, "/dev/random") != 0)
    {
        perror("read /dev/random");
        return 1;
    }

    srand48(seed);

    if (flag_use_bad)
    {
        fprintf(stderr, "using bad dist...\n");
        naive_t dist;
        naive_init(&dist, min, max);

        for (long i = 0; i < count; ++i)
        {
            // long n = naive_random(min, max);
            long n = naive_get(&dist);
            printf("%ld\n", n);
        }
    }
    else
    {
        uniform_t dist;
        uniform_init(&dist, min, max);

        for (long i = 0; i < count; ++i)
        {
            // long n = uniform_random(min, max);
            long n = uniform_get(&dist);
            printf("%ld\n", n);
        }
    }

    return 0;
}
