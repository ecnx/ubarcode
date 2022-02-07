/* ------------------------------------------------------------------
 * uBarcode - Bitmap to PNG Convertsion
 * ------------------------------------------------------------------ */

#include "ubarcode.h"
#include "png.h"

/**
 * Structure to store PNG image bytes
 */
struct mem_encode
{
    char *buffer;
    size_t size;
};

/**
 * PNG fill buffer callback
 */
void my_png_write_data ( png_structp png_ptr, png_bytep data, png_size_t length )
{
    /* with libpng15 next line causes pointer deference error; use libpng12 */
    struct mem_encode *p = ( struct mem_encode * ) png_get_io_ptr ( png_ptr );  /* was png_ptr->io_ptr */
    size_t nsize = p->size + length;
    void *new_buffer;

    /* allocate or grow buffer */
    if ( p->buffer )
    {
        if ( ( new_buffer = realloc ( p->buffer, nsize ) ) )
        {
            p->buffer = new_buffer;
        } else
        {
            png_error ( png_ptr, "Re-Allocation Error" );
        }
    } else
    {
        if ( !( p->buffer = malloc ( nsize ) ) )
        {
            png_error ( png_ptr, "Allocation Error" );
        }
    }

    if ( !p->buffer )
    {
        png_error ( png_ptr, "Write Error" );
    }

    /* copy new bytes to end of buffer */
    memcpy ( p->buffer + p->size, data, length );
    p->size += length;
}

/**
 * This is optional but included to show how png_set_write_fn() is called
 */
void my_png_flush ( png_structp png_ptr )
{
    UNUSED ( png_ptr );
}

/*
   write an rgba image to a memory buffer in PNG format, without any fanciness.

   Params: rgba - the rgba values
       width - image width
       height - image height
       outsize - return for size of output buffer
   Returns: pointer to allocated buffer holding png data
*/
void *make_png_buffer_rgba ( unsigned char *rgba, int width, int height, size_t *outsize )
{
    int x;
    int y;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep row;

    struct mem_encode state = { 0, 0 };

    *outsize = 0;

    /* Initialize write structure */
    png_ptr = png_create_write_struct ( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    if ( png_ptr == NULL )
    {
        fprintf ( stderr, "Could not allocate write struct\n" );
        return NULL;
    }

    /* Initialize info structure */
    info_ptr = png_create_info_struct ( png_ptr );
    if ( info_ptr == NULL )
    {
        fprintf ( stderr, "Could not allocate info struct\n" );
        png_destroy_write_struct ( &png_ptr, NULL );
        return NULL;
    }

    /* Setup Exception handling */
    if ( setjmp ( png_jmpbuf ( png_ptr ) ) )
    {
        fprintf ( stderr, "Error during png creation\n" );
        png_destroy_write_struct ( &png_ptr, NULL );
        png_free_data ( png_ptr, info_ptr, PNG_FREE_ALL, -1 );
        return NULL;
    }

    /* if my_png_flush() is not needed, change the arg to NULL */
    png_set_write_fn ( png_ptr, &state, my_png_write_data, my_png_flush );

    /* Write header (8 bit colour depth) */
    png_set_IHDR ( png_ptr, info_ptr, width, height,
        8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );

    png_write_info ( png_ptr, info_ptr );

    /* Allocate memory for one row (3 bytes per pixel - RGB) */
    if ( !( row = ( png_bytep ) malloc ( 4 * width * sizeof ( png_byte ) ) ) )
    {
        fprintf ( stderr, "Could not allocate png row buffer\n" );
        png_destroy_write_struct ( &png_ptr, NULL );
        png_free_data ( png_ptr, info_ptr, PNG_FREE_ALL, -1 );
        return NULL;
    }
    // Write image data
    for ( y = 0; y < height; y++ )
    {
        for ( x = 0; x < width; x++ )
        {
            row[x * 4] = rgba[( y * width + x ) * 4];
            row[x * 4 + 1] = rgba[( y * width + x ) * 4 + 1];
            row[x * 4 + 2] = rgba[( y * width + x ) * 4 + 2];
            row[x * 4 + 3] = rgba[( y * width + x ) * 4 + 3];
        }
        png_write_row ( png_ptr, row );
    }

    // End write
    png_write_end ( png_ptr, NULL );

    png_free_data ( png_ptr, info_ptr, PNG_FREE_ALL, -1 );
    png_destroy_write_struct ( &png_ptr, NULL );
    free ( row );

    *outsize = state.size;
    return state.buffer;
}
