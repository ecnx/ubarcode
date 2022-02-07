/* ------------------------------------------------------------------
 * uBarcode - Plot barcode type CODABAR
 * ------------------------------------------------------------------ */

#include "ubarcode.h"
#include <png.h>

/**
 * Barcode dictionary
 */
static const char *dictionary[][2] = {
    {"0", "NnNnNwW"},
    {"1", "NnNnWwN"},
    {"2", "NnNwNnW"},
    {"3", "WwNnNnN"},
    {"4", "NnWnNwN"},
    {"5", "WnNnNwN"},
    {"6", "NwNnNnW"},
    {"7", "NwNnWnN"},
    {"8", "NwWnNnN"},
    {"9", "WnNwNnN"},
    {"A", "NnWwNwN"},
    {"T", "NnWwNwN"},
    {"B", "NwNwNnW"},
    {"N", "NwNwNnW"},
    {"C", "NnNwNwW"},
    {"*", "NnNwNwW"},
    {"D", "NnNwWwN"},
    {"E", "NnNwWwN"},
    {".", "WnWnWnN"},
    {"/", "WnWnNnW"},
    {":", "WnNnWnW"},
    {"+", "NnWnWnW"}
};

/**
 * Encode signle symbol into bar and space widths
 */
static const char *encode_symbol ( const char *symbol_name )
{
    size_t i;
    const size_t n = sizeof ( dictionary ) / sizeof ( const char * ) / 2;

    for ( i = 0; i < n; i++ )
    {
        if ( streq_plus ( dictionary[i][0], symbol_name ) )
        {
            return dictionary[i][1];
        }
    }

    return NULL;
}

/**
 * Encode barcode input string
 */
static int encode_input ( const char *input, struct byte_array_t *array )
{
    const char *ptr;
    const char *encoded;

    /* Left quiet zone */
    if ( byte_array_push_bytes ( array, "nnnnnnnnnn", 10 * sizeof ( char ) ) )
    {
        return -1;
    }

    if ( !( encoded = encode_symbol ( "B" ) ) )
    {
        fprintf ( stderr, "Error: Encoding exception at (Start)\n" );
        return -1;
    }

    /* Start symbol */
    if ( byte_array_push_bytes ( array, encoded, strlen ( encoded ) ) )
    {
        return -1;
    }

    /* Space */
    if ( byte_array_push_bytes ( array, "n", sizeof ( char ) ) )
    {
        return -1;
    }

    for ( ptr = input; ptr; input_symbol_next ( &ptr ) )
    {
        if ( !( encoded = encode_symbol ( ptr ) ) )
        {
            fprintf ( stderr, "Error: Encoding exception at ...%s\n", ptr );
            return -1;
        }

        if ( byte_array_push_bytes ( array, encoded, strlen ( encoded ) ) )
        {
            return -1;
        }

        /* Space */
        if ( byte_array_push_bytes ( array, "n", sizeof ( char ) ) )
        {
            return -1;
        }
    }

    if ( !( encoded = encode_symbol ( "E" ) ) )
    {
        fprintf ( stderr, "Error: Encoding exception at (Stop)\n" );
        return -1;
    }

    /* Stop symbol */
    if ( byte_array_push_bytes ( array, encoded, strlen ( encoded ) ) )
    {
        return -1;
    }

    /* Right quiet zone */
    if ( byte_array_push_bytes ( array, "nnnnnnnnnn", 10 * sizeof ( char ) ) )
    {
        return -1;
    }

    return 0;
}

/**
 * Generate barcode bitmap
 */
static int generate_bitmap ( const struct barcode_plot_options_t *options,
    const struct byte_array_t *array, int width, int height, struct pixel_t **pbitmap )
{
    int distance;
    size_t i;
    size_t alloc_len;
    int x;
    int offset;
    int module;
    int modsum = 0;
    struct pixel_t *bitmap;

    for ( i = 0; i < array->len; i++ )
    {
        modsum += ( array->data[i] == 'W' || array->data[i] == 'w' ) ? 5 : 2;
    }

    if ( !( module = 2 * options->width / modsum ) )
    {
        fprintf ( stderr, "Error: X-resolution too small (%u < %u)\n", options->width, modsum );
        return -1;
    }

    modsum = modsum * module / 2;

    alloc_len = width * height * sizeof ( struct pixel_t );

    if ( !( bitmap = ( struct pixel_t * ) malloc ( alloc_len ) ) )
    {
        fprintf ( stderr, "Error: Allocate bitmap failed (%lu bytes)\n",
            ( unsigned long ) alloc_len );
        return -1;
    }

    *pbitmap = bitmap;

    offset = ( options->width - modsum ) / 2;

    plot_draw_bar ( bitmap, 0, offset, width, height, 1 );

    for ( i = 0, x = offset; i < array->len; i++ )
    {
        if ( array->data[i] == 'W' || array->data[i] == 'w' )
        {
            distance = 2 * module + module / 2;

        } else
        {
            distance = module;
        }

        plot_draw_bar ( bitmap, x, distance, width, height, array->data[i] == 'n'
            || array->data[i] == 'w' );

        x += distance;
    }

    plot_draw_bar ( bitmap, x, offset, width, height, 1 );

    return 0;
}

/**
 * Plot barcode type CODABAR
 */
int plot_barcode_codabar ( const struct barcode_plot_options_t *options )
{
    size_t len;
    void *buf;
    struct byte_array_t array;
    struct pixel_t *bitmap;

    printf ( "making a code39 barcode...\n" );
    printf ( "input symbols count: %lu\n",
        ( unsigned long ) count_input_symbols ( options->input ) );
    printf ( "barcode picture width: %i\n", options->width );
    printf ( "barcode picture height: %i\n", options->height );

    /* Set initial size to 256 * 6 = 1536 */
    if ( byte_array_new ( &array, 1536 ) < 0 )
    {
        return -1;
    }

    if ( encode_input ( options->input, &array ) < 0 )
    {
        byte_array_free ( &array );
        return -1;
    }

    printf ( "bar-spaces total count: %lu\n", ( unsigned long ) array.len );

    if ( generate_bitmap ( options, &array, options->width, options->height, &bitmap ) < 0 )
    {
        byte_array_free ( &array );
        return -1;
    }

    byte_array_free ( &array );

    printf ( "generated barcode bitmap.\n" );

    if ( !( buf =
            make_png_buffer_rgba ( ( uint8_t * ) bitmap, options->width, options->height, &len ) ) )
    {
        free ( bitmap );
        return -1;
    }

    free ( bitmap );

    printf ( "barcode png buffer size: %lu bytes.\n", ( unsigned long ) len );

    if ( export_picture ( buf, len, options->bmpfile ) < 0 )
    {
        fprintf ( stderr, "Error: Unable to write bitmap file: %i\n", errno );
        free ( buf );
        return -1;
    }

    printf ( "saved barcode bitmap to file.\n" );

    free ( buf );

    return 0;
}
