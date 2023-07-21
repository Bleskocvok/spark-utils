
// getline
#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif


#include <stdio.h>      // printf, stderr, perror, getline
#include <stdint.h>     // uint8_t
#include <stddef.h>     // size_t
#include <stdlib.h>     // malloc, calloc, realloc, free, strtol
#include <string.h>     // memset


void get_histogram(const int* vec, size_t count, int** out, size_t* out_count)
{
    long long max_value = 0;

    for (size_t i = 0; i < count; ++i)
    {
        max_value = vec[i] > max_value ? vec[i] : max_value;
    }

    int* values = calloc(1 + max_value, sizeof(int));

    if (!values)
    {
        perror("alloc");
        return;
    }

    for (size_t i = 0; i < count; ++i)
    {
        ++values[vec[i]];
    }

    *out = values;
    *out_count = max_value + 1;
}


void print_histogram(const int* hist, size_t hist_size, size_t count)
{
    static const char* blocks[] =
    {
        "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█",
    };
    static const int blocks_count = 8;

    for (size_t i = 0; i < hist_size; ++i)
    {
        double percent = hist[i] * 100 / (double) count;

        double p = percent;
        int chunk = 10;
        for (int j = 0; j < chunk; ++j)
        {
            if (p >= chunk)
                fprintf(stderr, "%s", blocks[blocks_count - 1]);
            else if (p >= 0)
                fprintf(stderr, "%s",
                        blocks[(size_t)(p / chunk * (blocks_count - 1))]);
            else
                fprintf(stderr, " ");

            p -= chunk;
        }

        fprintf(stderr, "  %3zu: %8d (%8.3f %%)\n", i, hist[i], percent);
    }
}


void sub_info(const int* hist, size_t hist_size, size_t count, size_t sub_count)
{
    size_t sub_sum = 0;
    for (size_t i = 0; i < sub_count && i < hist_size; ++i)
    {
        sub_sum += hist[i];
    }

    double expected = sub_count * 100 / (double) hist_size;
    double actual   = sub_sum   * 100 / (double) count;

    // printf("expected = %zd * 100 / %zd\n", sub_count, hist_size);
    // printf("actual   = %zd * 100 / %zd\n", sub_sum, count);

    printf( "total sum = %10zd\n"
            "sub sum   = %10zd\n"
            "(%zd / %zd)\n"
            "expected  = %10.6f %%\n"
            "actual    = %10.6f %%\n"
            "diff      = %10.6f %%\n",
            count,
            sub_sum,
            sub_count, hist_size,
            expected,
            actual,
            actual - expected );
}


void getlines(FILE* file, int** out, size_t* out_count)
{
    size_t outlen = 1024;
    int* ptr = malloc(outlen * sizeof(int));
    size_t count = 0;

    char* buf = 0;
    size_t buflen = 0;
    while (getline(&buf, &buflen, file) != -1)
    {
        if (count >= outlen)
        {
            outlen *= 2;
            ptr = realloc(ptr, outlen * sizeof(int));
        }

        ptr[count] = strtol(buf, 0, 0);
        ++count;
    }
    free(buf);

    *out = ptr;
    *out_count = count;
}


void get_histogram_better(FILE* file, int** out_hist, size_t* out_size,
                          size_t* out_count)
{
    size_t outlen = 1;
    int* hist = calloc(outlen, sizeof(int));
    size_t hist_size = 0;

    size_t count = 0;

    char* buf = 0;
    size_t buflen = 0;
    while (getline(&buf, &buflen, file) != -1)
    {
        long num = strtol(buf, 0, 0);

        if (num >= (long long) outlen)
        {
            size_t prev = outlen;
            outlen = 2 * (num + 1);
            hist = realloc(hist, outlen * sizeof(int));
            memset(hist + prev, 0, outlen - prev);
        }

        if (num >= (long long) hist_size)
            hist_size = num + 1;

        ++hist[num];
        ++count;
    }
    free(buf);

    *out_hist = hist;
    *out_size = hist_size;
    *out_count = count;
}


double get_median(const int* hist, size_t hist_size, size_t count)
{
    if (count == 0)
        return 0;

    size_t rem = count;
    size_t used = 0;
    for (size_t i = 0; i < hist_size; ++i)
    {
        rem  -= hist[i];
        used += hist[i];

        if (rem < used)
            return i;

        if (rem == used)
        {
            for (size_t j = i + 1; j < hist_size; ++j)
                if (hist[j] > 0)
                    return (i + j) * 0.5;
            return i;  // cannot happen if provided <count> is consistent
        }
    }
    return hist_size - 1;
}


double get_avg(const int* hist, size_t hist_size, size_t count)
{
    if (count == 0)
        return 0;

    double avg = 0;

    for (size_t i = 0; i < hist_size; ++i)
        avg += i * (double) hist[i];

    avg /= (double) count;

    return avg;
}


int main(int argc, char** argv)
{
    int* hist = 0;
    size_t hist_size = 0;
    size_t count = 0;

    int use_min_max = 1;
    int print_hist  = 0;

    if ((argc + 1) % 2 == 0)
    {
        get_histogram_better(stdin, &hist, &hist_size, &count);
    }
    else
    {
        const char* filename = argv[1];
        FILE* file = fopen(filename, "r");
        if (!file)
        {
            perror(filename);
            return 1;
        }

        get_histogram_better(file, &hist, &hist_size, &count);

        fclose(file);
    }

    if (print_hist)
    {
        print_histogram(hist, hist_size, count);
    }

    printf("total: %zd\n", count);

    long min = 0,
         max = hist_size == 0 ? 0 : hist_size - 1;

    double avg    = get_avg(   hist, hist_size, count);
    double median = get_median(hist, hist_size, count);

    printf("min=%ld\n"
           "max=%ld\n"
           "avg=%f\n"
           "med=%f\n", min, max, avg, median);

    if (use_min_max)
    {
        const long LRAND48_MAX = (1l << 31l) - 1l;  // is 2^31 - 1
        long k = max - min + 1;
        long mod = (LRAND48_MAX + 1) % k;

        printf("  k=%ld\n"
               "mod=%ld\n", k, mod);

        sub_info(hist, hist_size, count, mod);
    }

    free(hist);

    return 0;
}
