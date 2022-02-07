/* ------------------------------------------------------------------
 * uBarcode - Plot barcode type CODE-39
 * ------------------------------------------------------------------ */

#include "ubarcode.h"
#include <png.h>

/**
 * Barcode dictionary
 */
static const char *dictionary[][2] = {
    {"0", "131112"},
    {"1", "111213"},
    {"2", "111312"},
    {"3", "111411"},
    {"4", "121113"},
    {"5", "121212"},
    {"6", "121311"},
    {"7", "111114"},
    {"8", "131211"},
    {"9", "141111"},
    {"A", "211113"},
    {"B", "211212"},
    {"C", "211311"},
    {"D", "221112"},
    {"E", "221211"},
    {"F", "231111"},
    {"G", "112113"},
    {"H", "112212"},
    {"I", "112311"},
    {"J", "122112"},
    {"K", "132111"},
    {"L", "111123"},
    {"M", "111222"},
    {"N", "111321"},
    {"O", "121122"},
    {"P", "131121"},
    {"Q", "212112"},
    {"R", "212211"},
    {"S", "211122"},
    {"T", "211221"},
    {"U", "221121"},
    {"V", "222111"},
    {"W", "112122"},
    {"X", "112221"},
    {"Y", "122121"},
    {"Z", "123111"},
    {"-", "121131"},
    {".", "311112"},
    {" ", "311211"},
    {"$", "321111"},
    {"/", "112131"},
    {"Plus", "113121"},
    {"%", "211131"},
    {"($)", "121221"},
    {"(%)", "312111"},
    {"(/)", "311121"},
    {"(Plus)", "122211"},
    {"*", "111141"}
};

/**
 * Encode signle symbol into bar and space widths
 */
static const char *encode_symbol ( const char *symbol_name, size_t *index )
{
    size_t i;
    const size_t n = sizeof ( dictionary ) / sizeof ( const char * ) / 2;

    for ( i = 0; i < n; i++ )
    {
        if ( streq_plus ( dictionary[i][0], symbol_name ) )
        {
            if ( index )
            {
                *index = i;
            }
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
    int c_weight;
    unsigned long long c_checksum = 0;
    int k_weight;
    unsigned long long k_checksum = 0;
    size_t index;
    size_t n_symbols;
    const char *ptr;
    const char *encoded;
    const size_t n = sizeof ( dictionary ) / sizeof ( const char * ) / 2;

    /* Setup checksum weights */
    n_symbols = count_input_symbols ( input );

    c_weight = n_symbols % 20;
    k_weight = ( n_symbols + 1 ) % 15;

    if ( !c_weight )
    {
        c_weight = 20;
    }

    if ( !k_weight )
    {
        k_weight = 15;
    }

    /* Left quiet zone */
    if ( byte_array_push_bytes ( array, "a", sizeof ( char ) ) )
    {
        return -1;
    }

    if ( !( encoded = encode_symbol ( "*", NULL ) ) )
    {
        fprintf ( stderr, "Error: Encoding exception at (Start)\n" );
        return -1;
    }

    /* Start symbol */
    if ( byte_array_push_bytes ( array, encoded, strlen ( encoded ) ) )
    {
        return -1;
    }

    for ( ptr = input; ptr; input_symbol_next ( &ptr ) )
    {
        if ( !( encoded = encode_symbol ( ptr, &index ) ) )
        {
            fprintf ( stderr, "Error: Encoding exception at ...%s\n", ptr );
            return -1;
        }

        if ( byte_array_push_bytes ( array, encoded, strlen ( encoded ) ) )
        {
            return -1;
        }

        c_checksum += index * c_weight;
        k_checksum += index * k_weight;

        c_weight--;

        if ( !c_weight )
        {
            c_weight = 20;
        }

        k_weight--;

        if ( !k_weight )
        {
            k_weight = 15;
        }
    }

    c_checksum %= 47;

    if ( c_checksum >= n )
    {
        fprintf ( stderr, "Error: C-checksum mapping out of range (%llu >= %lu).\n", c_checksum,
            ( unsigned long ) n );
        return -1;
    }

    printf ( "c-checksum (modulo 47) is %i.\n", ( int ) c_checksum );

    encoded = dictionary[c_checksum][1];

    /* Checksum */
    if ( byte_array_push_bytes ( array, encoded, strlen ( encoded ) ) )
    {
        return -1;
    }

    k_checksum += c_checksum * k_weight;

    k_checksum %= 47;

    if ( k_checksum >= n )
    {
        fprintf ( stderr, "Error: K-checksum mapping out of range (%llu >= %lu).\n", k_checksum,
            ( unsigned long ) n );
        return -1;
    }

    printf ( "k-checksum (modulo 47) is %i.\n", ( int ) k_checksum );

    encoded = dictionary[k_checksum][1];

    /* Checksum */
    if ( byte_array_push_bytes ( array, encoded, strlen ( encoded ) ) )
    {
        return -1;
    }

    if ( !( encoded = encode_symbol ( "*", NULL ) ) )
    {
        fprintf ( stderr, "Error: Encoding exception at (Stop)\n" );
        return -1;
    }

    /* Stop symbol */
    if ( byte_array_push_bytes ( array, encoded, strlen ( encoded ) ) )
    {
        return -1;
    }

    /* Terminal bar */
    if ( byte_array_push_bytes ( array, "1", sizeof ( char ) ) )
    {
        return -1;
    }

    /* Right quiet zone */
    if ( byte_array_push_bytes ( array, "a", sizeof ( char ) ) )
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
    int space;
    int distance;
    size_t i;
    size_t alloc_len;
    int x;
    int offset;
    int module;
    int modsum = 0;
    char buf[2];
    struct pixel_t *bitmap;

    for ( i = 0, buf[1] = '\0'; i < array->len; i++ )
    {
        buf[0] = array->data[i];

        if ( sscanf ( buf, "%x", &distance ) <= 0 )
        {
            return -1;
        }

        modsum += distance;
    }

    if ( !( module = options->width / modsum ) )
    {
        fprintf ( stderr, "Error: X-resolution too small (%u < %u)\n", options->width, modsum );
        return -1;
    }

    modsum *= module;

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

    for ( i = 0, x = offset, space = 1, buf[1] = '\0'; i < array->len; space = !space, i++ )
    {
        buf[0] = array->data[i];

        if ( sscanf ( buf, "%x", &distance ) <= 0 )
        {
            free ( bitmap );
            return -1;
        }

        distance *= module;
        plot_draw_bar ( bitmap, x, distance, width, height, space );

        x += distance;
    }

    plot_draw_bar ( bitmap, x, offset, width, height, 1 );

    return 0;
}

/**
 * Plot barcode type CODE-93
 */
int plot_barcode_code93 ( const struct barcode_plot_options_t *options )
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
