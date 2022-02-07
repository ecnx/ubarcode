/* ------------------------------------------------------------------
 * uBarcode - Plot barcode type generic
 * ------------------------------------------------------------------ */

#include "ubarcode.h"

/**
 * Check if strings are equal until plus sign is found
 */
int streq_plus ( const char *a, const char *b )
{
    size_t i;
    size_t len;
    size_t len_b;

    len = strlen ( a );
    len_b = strlen ( b );

    if ( len_b < len )
    {
        len = len_b;
    }

    for ( i = 0; i <= len; i++ )
    {
        if ( a[i] == '+' )
        {
            return !b[i];
        }

        if ( b[i] == '+' )
        {
            return !a[i];
        }

        if ( a[i] != b[i] )
        {
            return 0;
        }
    }

    return 1;
}

/**
 * Obtain next input symbol by plus sign separator
 */
void input_symbol_next ( const char **ptr )
{
    const char *aux;

    if ( ( aux = strchr ( *ptr, '+' ) ) )
    {
        *ptr = aux + 1;

    } else
    {
        *ptr = NULL;
    }
}

/**
 * Count input symbols
 */
size_t count_input_symbols ( const char *input )
{
    size_t result = 0;
    const char *ptr;

    ptr = input;

    while ( ptr )
    {
        result++;
        input_symbol_next ( &ptr );
    }

    return result;
}

/**
 * Set pixel black or white
 */
static void set_pixel_black_or_white ( struct pixel_t *pixel, int white )
{
    uint8_t value;

    value = white ? 255 : 0;

    pixel->red = value;
    pixel->green = value;
    pixel->blue = value;
    pixel->alpha = 255;
}

/**
 * Plot vertical bar
 */
void plot_draw_bar ( struct pixel_t *bitmap, int x0, int d, int width, int height, int white )
{
    int x;
    int x1;
    int y;

    x1 = x0 + d;

    if ( x1 >= width )
    {
        x1 = width - 1;
    }

    for ( x = x0; x < x1; x++ )
    {
        for ( y = 0; y < height; y++ )
        {
            set_pixel_black_or_white ( bitmap + y * width + x, white );
        }
    }
}
