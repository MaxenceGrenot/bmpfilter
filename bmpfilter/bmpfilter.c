#ifndef STDLIB_H
#include <stdlib.h>
#define STDLIB_H
#endif
#ifndef STDIO_H
#include <stdio.h>
#define STDIO_H
#endif
#ifndef BITMAP_H
#include "bitmap.h"
#define BITMAP_H
#endif

int main(int argc, char *argv[])
{
    BITMAP *bmp = NULL;
    int i;

    if (2 > argc)
    {
        printf("Usage:\t ./bmpfilter <bmp_file_path> [red|green|blue|blur|mirror]\n");
        return EXIT_FAILURE;
    }

    printf("Info: Opening the picture %s\n", argv[1]);
    bmp = loadBitmap(argv[1]);
    if (bmp)
    {
        for (i = 2; i < argc; i++)
        {
            applyFilter(bmp, getCorrespondingFilter(argv[i]));
        }
        printf("Info: Saving the picture %s\n", bmp->file_path);
        saveBitmap(bmp);
        destroyBitmap(bmp);
    }
    else
    {
        printf("Info: Failed to load the picture %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}