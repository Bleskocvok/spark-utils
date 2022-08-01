
#include "deps/stb_image.h" // stbi*

#include <stdio.h>          // fprintf
#include <stdbool.h>        // bool, true, false
#include <stdint.h>         // uint32_t
#include <assert.h>         // assert
#include <string.h>         // strcmp
#include <stdlib.h>         // NULL, atoi
#include <math.h>           // ceilf


typedef struct
{
    int w,      // width
        h,      // height
        chnls;  // number of channels

    unsigned char* data;

} img_t;


typedef struct
{
    uint32_t r : 8;
    uint32_t g : 8;
    uint32_t b : 8;
    uint32_t a : 8;

} rgba_t;


uint32_t get_uint(int x, int y, img_t img)
{
    unsigned char* pixel = &img.data[x *  img.chnls +
                                     y * (img.chnls * img.w)];

    switch (img.chnls)
    {
        case 1: return (*pixel) * 0x01010100 | 0x000000ff;
        case 2: return pixel[0] * 0x01010100 | pixel[1] * 0x00000001;
        case 3: return pixel[0] * 0x01000000
                     | pixel[1] * 0x00010000
                     | pixel[2] * 0x00000100
                     | 0x000000ff;
        case 4: return pixel[0] * 0x01000000
                     | pixel[1] * 0x00010000
                     | pixel[2] * 0x00000100
                     | pixel[3] * 0x00000001;
        default: assert(false); return 0;
    }
}


rgba_t get(int x, int y, img_t img)
{
    uint32_t c = get_uint(x, y, img);

    return (rgba_t) { .r = (c & 0xff000000) >> 24,
                      .g = (c & 0x00ff0000) >> 16,
                      .b = (c & 0x0000ff00) >>  8,
                      .a = c & 0x000000ff, };
}


bool is_black(int x, int y, img_t info)
{
    rgba_t c = get(x, y, info);
    // return (c & 0xffffff00) < 0x88888800
    //     || (c & 0x000000ff) < 0x00000088;
    return c.r < 127 || c.a < 127;
}


uint8_t grey(rgba_t c)
{
    return 255 - ((c.r + c.g + c.b + c.a) / 4);
}


void write(img_t img, FILE* out, int res_h, bool verbose)
{
    static const char* HEADER = "%%MatrixMarket matrix coordinate pattern general\n";

    fputs(HEADER, out);

    int block = res_h / img.h;

    long count = 0;
    for (int y = 0; y < img.h; y++)
    {
        for (int x = 0; x < img.w; x++)
        {
            uint8_t opacity = grey(get(x, y, img));
            long nonzeros = opacity * block * block / 255;
            count += nonzeros;
        }
    }

    fprintf(out, "%d %d %ld\n", img.w * block, res_h, count);

    for (int y = 0; y < img.h; y++)
    {
        for (int x = 0; x < img.w; x++)
        {
            uint8_t opacity = grey(get(x, y, img));
            long nonzeros = ceilf(opacity * block * block * 1 / 255.0f);
            for (int i = 0; i < nonzeros; i++)
            {
                int dx = rand() % block;
                int dy = rand() % block;
                fprintf(out, "%d %d\n", y * block + 1 + dx,
                                        x * block + 1 + dy);
            }

            if (verbose)
            {
                static const char symbols[] = " .+*";
                char c = symbols[opacity / (256 / sizeof(symbols))];
                fprintf(stderr, "%c", c);
            }
        }
        if (verbose)
            fprintf(stderr, "\n");
    }
}


int main(int argc, char** argv)
{
    if (argc <= 1)
    {
        fprintf(stderr, "Usage: %s [-o FILE] [-h HEIGHT] [-v] IMAGE\n", argv[0]);
        return 1;
    }

    const char* imgfile = NULL;
    FILE* output = NULL;
    int height = 0;
    bool verbose = false;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp("-o", argv[i]) == 0)
        {
            output = fopen(argv[++i], "w");
        }
        else if (strcmp("-h", argv[i]) == 0)
        {
            height = atoi(argv[++i]);
        }
        else if (strcmp("-v", argv[i]) == 0)
        {
            verbose = true;
        }
        else
        {
            imgfile = argv[i];
        }
    }

    img_t img = { 0 };
    img.data = stbi_load(imgfile, &img.w, &img.h, &img.chnls, 0);

    fprintf(stderr, "img w=%d h=%d chnls=%d\n", img.w, img.h, img.chnls);

    write(img, output ? output : stdout, height > 0 ? height : img.h, verbose);

    stbi_image_free(img.data);

    if (output)
        fclose(output);

    return 0;
}
