#ifndef STDLIB_H
#include <stdlib.h>
#define STDLIB_H
#endif

#ifndef STDIO_H
#include <stdio.h>
#define STDIO_H
#endif

#ifndef STRING_H
#include <string.h>
#define STRING_H
#endif

#ifndef BITMAP_H
#include "bitmap.h"
#define BITMAP_H
#endif

BITMAP *loadBitmap(const char *file_path)
{
    FILE *fp = NULL;
    BITMAP *bmp = NULL;
    int ret, variableLength;

    /* Opening the input file */
    if (!file_path || !(fp = fopen(file_path, "rb")))
    {
        return NULL;
    }
    /* ------- */

    /* Creating and feeding the storage structure */
    if (!(bmp = malloc(sizeof(BITMAP))))
    {
        fprintf(stderr, "Error: Failed to allocate memory\n");
        if (fp)
        {
            fclose(fp);
        }
        return NULL;
    }
    memset(bmp, 0x00, sizeof(BITMAP));

    ret = fread(&(bmp->magic_number), 1, 2, fp);
    if (2 != ret)
    {
        fprintf(stderr, "Error: Failed to read\n");
        destroyBitmap(bmp);
        if (fp)
        {
            fclose(fp);
        }
        return NULL;
    }
    ret = fread(&(bmp->size), 4, 1, fp);
    if (1 != ret)
    {
        fprintf(stderr, "Error: Failed to read\n");
        destroyBitmap(bmp);
        if (fp)
        {
            fclose(fp);
        }
        return NULL;
    }
    ret = fread(&(bmp->application), 1, 4, fp);
    if (4 != ret)
    {
        fprintf(stderr, "Error: Failed to read\n");
        destroyBitmap(bmp);
        if (fp)
        {
            fclose(fp);
        }
        return NULL;
    }
    ret = fread(&(bmp->start_offset), 4, 1, fp);
    if (1 != ret)
    {
        fprintf(stderr, "Error: Failed to read\n");
        destroyBitmap(bmp);
        if (fp)
        {
            fclose(fp);
        }
        return NULL;
    }

    ret = fread(&(bmp->bitmap_header_size), 4, 1, fp);
    if (1 != ret)
    {
        fprintf(stderr, "Error: Failed to read\n");
        destroyBitmap(bmp);
        if (fp)
        {
            fclose(fp);
        }
        return NULL;
    }
    if (bmp->bitmap_header_size <= 12)
    {
        variableLength = 2;
    }
    else
    {
        variableLength = 4;
    }
    ret = fread(&(bmp->width), variableLength, 1, fp);
    if (1 != ret)
    {
        fprintf(stderr, "Error: Failed to read\n");
        destroyBitmap(bmp);
        if (fp)
        {
            fclose(fp);
        }
        return NULL;
    }
    ret = fread(&(bmp->height), variableLength, 1, fp);
    if (1 != ret)
    {
        fprintf(stderr, "Error: Failed to read\n");
        destroyBitmap(bmp);
        if (fp)
        {
            fclose(fp);
        }
        return NULL;
    }
    fseek(fp, 2, SEEK_CUR);
    ret = fread(&(bmp->depth), 2, 1, fp);
    if (1 != ret)
    {
        fprintf(stderr, "Error: Failed to read\n");
        destroyBitmap(bmp);
        if (fp)
        {
            fclose(fp);
        }
        return NULL;
    }
    /* ------- */

    /* Storing the header */
    rewind(fp);
    if (!(bmp->header = malloc(bmp->start_offset)))
    {
        fprintf(stderr, "Error: Failed to allocate memory\n");
        destroyBitmap(bmp);
        if (fp)
        {
            fclose(fp);
        }
        return NULL;
    }
    ret = fread(bmp->header, 1, bmp->start_offset, fp);
    if (ret != bmp->start_offset)
    {
        fprintf(stderr, "Error: Failed to read\n");
        destroyBitmap(bmp);
        if (fp)
        {
            fclose(fp);
        }
        return NULL;
    }
    /* ------- */

    /* Storing the pixels into the raster */
    if (!(bmp->raster = malloc(bmp->size - bmp->start_offset)))
    {
        fprintf(stderr, "Error: Failed to allocate memory\n");
        destroyBitmap(bmp);
        if (fp)
        {
            fclose(fp);
        }
        return NULL;
    }
    ret = fread(bmp->raster, 1, bmp->size - bmp->start_offset, fp);
    if (ret != bmp->size - bmp->start_offset)
    {
        fprintf(stderr, "Error: Failed to read\n");
        destroyBitmap(bmp);
        if (fp)
        {
            fclose(fp);
        }
        return NULL;
    }
    /* ------- */

    /* Saving the name of the file (and adding the "_mod" suffix) */
    strncpy(bmp->file_path, file_path, strlen(file_path));
    strncat(bmp->file_path, "_mod", 4);
    /* ------- */

    /* Allowed bitmap test (24bpp color depth only) */
    if (!allowedBitmapFormat(bmp))
    {
        fprintf(stderr, "Error: Bitmap format not allowed (only 24 bpp)\n");
        destroyBitmap(bmp);
        if (fp)
        {
            fclose(fp);
        }
        return NULL;
    }
    /* ------- */

    if (fp)
    {
        fclose(fp);
    }
    return bmp;
}

bool allowedBitmapFormat(BITMAP *bmp)
{
    if (!bmp || strncmp(bmp->magic_number, "BM", 2))
    {
        return false;
    }

    return (24 == bmp->depth) ? true : false;
}

void destroyBitmap(BITMAP *bmp)
{
    if (bmp)
    {
        if (bmp->header)
        {
            free(bmp->header);
        }
        if (bmp->raster)
        {
            free(bmp->raster);
        }
        free(bmp);
    }
}

/*
 * Getting a pixel from the raster using its coordinates following the
 * common logic (The upper-left pixel coordinates are (0, 0))
 */
PIXEL *getPixel(PIXEL *raster, int width, int height, int x, int y)
{
    return raster;
}

/*
 * Setting a pixel from the raster using its coordinates following the
 * common logic (The upper-left pixel coordinates are (0, 0))
 */
void setPixel(PIXEL *raster, int width, int height, int x, int y, PIXEL *value)
{
    (*raster).red = 0;
}

/*
 * Cutting one color channel for every pixels in the content of the raster
 */
void applyFilterColor(BITMAP *bmp, FILTER filter)
{
    int i = 0;
    if (filter == RED)
    {
        printf("Applying red filter... \n");
        for (i = 0; i < bmp->width * bmp->height; i++)
        {
            bmp->raster[i].red = 0;
        }
    }
    if (filter == GREEN)
    {
        printf("Applying green filter... \n");
        for (i = 0; i < bmp->width * bmp->height; i++)
        {
            bmp->raster[i].green = 0;
        }
    }
	if (filter == BLUE)
    {
        printf("Applying blue filter... \n");
        for (i = 0; i < bmp->width * bmp->height; i++)
        {
            bmp->raster[i].blue = 0;
        }
    }
	
    
}

/*
 * Applying a simple blur effect to the picture
 */
void applyFilterSimpleBlur(BITMAP *bmp)
{
    printf("Applying blur filter... \n");
    int pixel = 0;
    for (pixel = bmp->width + 2; pixel < (bmp->width * (bmp->height - 1)) - 2; pixel++)
    {
        /*calcul de la moyenne des pixels rouges*/
        bmp->raster[pixel].red =
                (bmp->raster[pixel].red +
                 bmp->raster[pixel + 1].red +
                 bmp->raster[pixel - 1].red +
                 bmp->raster[pixel + bmp->width].red +
                 bmp->raster[pixel - bmp->width].red +
                 bmp->raster[pixel + bmp->width - 1].red +
                 bmp->raster[pixel - bmp->width - 1].red +
                 bmp->raster[pixel + bmp->width + 1].red +
                 bmp->raster[pixel - bmp->width + 1].red) /
                9;
        /*calcul de la moyenne des pixels verts*/
        bmp->raster[pixel].green =
                (bmp->raster[pixel].green +
                 bmp->raster[pixel + 1].green +
                 bmp->raster[pixel - 1].green +
                 bmp->raster[pixel + bmp->width].green +
                 bmp->raster[pixel - bmp->width].green +
                 bmp->raster[pixel + bmp->width - 1].green +
                 bmp->raster[pixel - bmp->width - 1].green +
                 bmp->raster[pixel + bmp->width + 1].green +
                 bmp->raster[pixel - bmp->width + 1].green) /
                9;
        /* calcul de la moyenne des pixels bleus*/
        bmp->raster[pixel].blue =
                (bmp->raster[pixel].blue +
                 bmp->raster[pixel + 1].blue +
                 bmp->raster[pixel - 1].blue +
                 bmp->raster[pixel + bmp->width].blue +
                 bmp->raster[pixel - bmp->width].blue +
                 bmp->raster[pixel + bmp->width - 1].blue +
                 bmp->raster[pixel - bmp->width - 1].blue +
                 bmp->raster[pixel + bmp->width + 1].blue +
                 bmp->raster[pixel - bmp->width + 1].blue) /
                9;
    }

    printf("BLUR --> OK")
}

/*
 * Applying a mirror effect to the content of the raster
 */
void applyFilterMirror(BITMAP *bmp)
{

    printf("Applying Mirror filter... \n");
    int i;
    int w = bmp->width - 1;
    int moins = 1;
    int x = 0;
    for (i = 0; i < bmp->width * bmp->height; i++)
    {
        if (w > bmp->width / 2)
        {
            x = bmp->raster[i].red;
            bmp->raster[i].red = bmp->raster[i + w - moins].red;
            bmp->raster[i + w - moins].red = x;

            x = bmp->raster[i].green;
            bmp->raster[i].green = bmp->raster[i + w - moins].green;
            bmp->raster[i + w - moins].green = x;

            x = bmp->raster[i].blue;
            bmp->raster[i].blue = bmp->raster[i + w - moins].blue;
            bmp->raster[i + w - moins].blue = x;
        }
        w--;
        moins++;
        if (w == 0)
        {
            w = bmp->width;
            moins = 1;
        }
    }
}

void applyFilter(BITMAP *bmp, FILTER filter)
{
    switch (filter)
    {
    case RED:
    case GREEN:
    case BLUE:
        applyFilterColor(bmp, filter);
        break;

    case BLUR:
        applyFilterSimpleBlur(bmp);
        break;

    case MIRROR:
        applyFilterMirror(bmp);
        break;

    case UNKNOWN:
        break;
    }
}

FILTER getCorrespondingFilter(const char *filter_name)
{
    if (!filter_name)
    {
        return UNKNOWN;
    }

    if (!strcmp("red", filter_name))
    {
        return RED;
    }
    if (!strcmp("green", filter_name))
    {
        return GREEN;
    }
    if (!strcmp("blue", filter_name))
    {
        return BLUE;
    }
    if (!strcmp("blur", filter_name))
    {
        return BLUR;
    }
    if (!strcmp("mirror", filter_name))
    {
        return MIRROR;
    }

    return UNKNOWN;
}

void saveBitmap(BITMAP *bmp)
{
    FILE *fp = NULL;
    int rec;

    /* Opening the ouput file */
    if (!bmp || !bmp->file_path || !(fp = fopen(bmp->file_path, "wb")))
    {
        return;
    }
    /* ------- */

    rec = fwrite(bmp->header, 1, bmp->start_offset, fp);
    if (rec != bmp->start_offset)
    {
        fprintf(stderr, "Error: Failed to write\n");
        goto end;
    }
    rec = fwrite((unsigned char *)(bmp->raster), 1, bmp->size - bmp->start_offset, fp);
    if (rec != bmp->size - bmp->start_offset)
    {
        fprintf(stderr, "Error: Failed to write\n");
        goto end;
    }

end:
    if (fp)
    {
        fclose(fp);
    }
}