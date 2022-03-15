/* ------------------------------------------------------------------
 * uBarcode - Plot barcode type CODE-128
 * ------------------------------------------------------------------ */

#include "ubarcode.h"
#include <png.h>

/**
 * Code128 subtypes
 */
enum code128_subtype_t
{
    CODE128_UNKNOWN,
    CODE128_A,
    CODE128_B,
    CODE128_C
};

/**
 * Barcode dictionary
 */
static const char *dictionary[][4] = {
    {" ", " ", "00", "212222"},
    {"!", "!", "01", "222122"},
    {"\"", "\"", "02", "222221"},
    {"#", "#", "03", "121223"},
    {"$", "$", "04", "121322"},
    {"%", "%", "05", "131222"},
    {"&", "&", "06", "122213"},
    {"\"", "\"", "07", "122312"},
    {"(", "(", "08", "132212"},
    {")", ")", "09", "221213"},
    {"*", "*", "10", "221312"},
    {"Plus", "Plus", "11", "231212"},
    {",", ",", "12", "112232"},
    {"-", "-", "13", "122132"},
    {".", ".", "14", "122231"},
    {"/", "/", "15", "113222"},
    {"0", "0", "16", "123122"},
    {"1", "1", "17", "123221"},
    {"2", "2", "18", "223211"},
    {"3", "3", "19", "221132"},
    {"4", "4", "20", "221231"},
    {"5", "5", "21", "213212"},
    {"6", "6", "22", "223112"},
    {"7", "7", "23", "312131"},
    {"8", "8", "24", "311222"},
    {"9", "9", "25", "321122"},
    {":", ":", "26", "321221"},
    {";", ";", "27", "312212"},
    {"<", "<", "28", "322112"},
    {"=", "=", "29", "322211"},
    {">", ">", "30", "212123"},
    {"?", "?", "31", "212321"},
    {"@", "@", "32", "232121"},
    {"A", "A", "33", "111323"},
    {"B", "B", "34", "131123"},
    {"C", "C", "35", "131321"},
    {"D", "D", "36", "112313"},
    {"E", "E", "37", "132113"},
    {"F", "F", "38", "132311"},
    {"G", "G", "39", "211313"},
    {"H", "H", "40", "231113"},
    {"I", "I", "41", "231311"},
    {"J", "J", "42", "112133"},
    {"K", "K", "43", "112331"},
    {"L", "L", "44", "132131"},
    {"M", "M", "45", "113123"},
    {"N", "N", "46", "113321"},
    {"O", "O", "47", "133121"},
    {"P", "P", "48", "313121"},
    {"Q", "Q", "49", "211331"},
    {"R", "R", "50", "231131"},
    {"S", "S", "51", "213113"},
    {"T", "T", "52", "213311"},
    {"U", "U", "53", "213131"},
    {"V", "V", "54", "311123"},
    {"W", "W", "55", "311321"},
    {"X", "X", "56", "331121"},
    {"Y", "Y", "57", "312113"},
    {"Z", "Z", "58", "312311"},
    {"{", "{", "59", "332111"},
    {"\\", "\\", "60", "314111"},
    {" }", " }", "61", "221411"},
    {"^", "^", "62", "431111"},
    {"_", "_", "63", "111224"},
    {"NUL", "`", "64", "111422"},
    {"SOH", "a", "65", "121124"},
    {"STX", "b", "66", "121421"},
    {"ETX", "c", "67", "141122"},
    {"EOT", "d", "68", "141221"},
    {"ENQ", "e", "69", "112214"},
    {"ACK", "f", "70", "112412"},
    {"BEL", "g", "71", "122114"},
    {"BS", "h", "72", "122411"},
    {"HT", "i", "73", "142112"},
    {"LF", "j", "74", "142211"},
    {"VT", "k", "75", "241211"},
    {"FF", "l", "76", "221114"},
    {"CR", "m", "77", "413111"},
    {"SO", "n", "78", "241112"},
    {"SI", "o", "79", "134111"},
    {"DLE", "p", "80", "111242"},
    {"DC1", "q", "81", "121142"},
    {"DC2", "r", "82", "121241"},
    {"DC3", "s", "83", "114212"},
    {"DC4", "t", "84", "124112"},
    {"NAK", "u", "85", "124211"},
    {"SYN", "v", "86", "411212"},
    {"ETB", "w", "87", "421112"},
    {"CAN", "x", "88", "421211"},
    {"EM", "y", "89", "212141"},
    {"SUB", "z", "90", "214121"},
    {"ESC", "{", "91", "412121"},
    {"FS", "|", "92", "111143"},
    {"GS", " }", "93", "111341"},
    {"RS", "~", "94", "131141"},
    {"US", "DEL", "95", "114113"},
    {"FNC 3", "FNC 3", "96", "114311"},
    {"FNC 2", "FNC 2", "97", "411113"},
    {"Shift B", "Shift A", "98", "411311"},
    {"Code C", "Code C", "99", "113141"},
    {"Code B", "FNC 4", "Code B", "114131"},
    {"FNC 4", "Code A", "Code A", "311141"},
    {"FNC 1", "FNC 1", "FNC 1", "411131"},
    {"Start A", "Start A", "Start A", "211412"},
    {"Start B", "Start B", "Start B", "211214"},
    {"Start C", "Start C", "Start C", "211232"},
    {"Stop", "Stop", "Stop", "2331112"},
};

/**
 * Encode signle symbol into bar and space widths
 */
static const char *encode_symbol ( enum code128_subtype_t subtype, const char *symbol_name,
    size_t *index )
{
    size_t i;
    size_t offset;
    const size_t n = sizeof ( dictionary ) / sizeof ( const char * ) / 4;

    switch ( subtype )
    {
    case CODE128_A:
        offset = 0;
        break;
    case CODE128_B:
        offset = 1;
        break;
    case CODE128_C:
        offset = 2;
        break;
    default:
        return NULL;
    }

    for ( i = 0; i < n; i++ )
    {
        if ( streq_plus ( dictionary[i][offset], symbol_name ) )
        {
            if ( index )
            {
                *index = i;
            }
            return dictionary[i][3];
        }
    }

    for ( i = 0; i < n; i++ )
    {
        if ( streq_plus ( dictionary[i][0], symbol_name ) &&
            streq_plus ( dictionary[i][1], symbol_name ) &&
            streq_plus ( dictionary[i][2], symbol_name ) )
        {
            if ( index )
            {
                *index = i;
            }
            return dictionary[i][3];
        }
    }

    return NULL;
}

/**
 * Stringify code128 subtype
 */
static const char *code128_str_subtype ( enum code128_subtype_t subtype )
{
    switch ( subtype )
    {
    case CODE128_A:
        return "code128-a";
    case CODE128_B:
        return "code128-b";
    case CODE128_C:
        return "code128-c";
    default:
        return "code128-?";
    }
}

/**
 * Switch encoding subtype
 */
static void switch_encoding ( enum code128_subtype_t *subtype, enum code128_subtype_t new_subtype )
{
    *subtype = new_subtype;
    printf ( "switched encoding -> %s\n", code128_str_subtype ( *subtype ) );
}

/**
 * Restore encoding subtype
 */
static void restore_encoding ( enum code128_subtype_t *subtype, enum code128_subtype_t new_subtype )
{
    *subtype = new_subtype;
    printf ( "Restored encoding -> %s\n", code128_str_subtype ( *subtype ) );
}

/**
 * Encode barcode input string
 */
static int encode_input ( const char *input, struct byte_array_t *array )
{
    enum code128_subtype_t subtype = CODE128_UNKNOWN;
    enum code128_subtype_t subtype_backup = CODE128_UNKNOWN;
    const size_t n = sizeof ( dictionary ) / sizeof ( const char * ) / 4;
    int num;
    int num_high;
    int num_low;
    unsigned long long checksum = 0;
    size_t pos;
    size_t index;
    const char *ptr;
    const char *aux;
    const char *encoded;
    char num_buf[256];

    /* Left quiet zone */
    if ( byte_array_push_bytes ( array, "a", sizeof ( char ) ) )
    {
        return -1;
    }

    for ( ptr = input, pos = 0; ptr; pos++ )
    {
        aux = ptr;

        if ( subtype_backup != CODE128_UNKNOWN )
        {
            restore_encoding ( &subtype, subtype_backup );
            subtype_backup = CODE128_UNKNOWN;
        }

        if ( streq_plus ( ptr, "Start A" ) )
        {
            switch_encoding ( &subtype, CODE128_A );

        } else if ( streq_plus ( ptr, "Start B" ) )
        {
            switch_encoding ( &subtype, CODE128_B );

        } else if ( streq_plus ( ptr, "Start C" ) )
        {
            switch_encoding ( &subtype, CODE128_C );

        } else if ( subtype == CODE128_C && !streq_plus ( ptr, "Code A" ) &&
            !streq_plus ( ptr, "Code B" ) && !streq_plus ( ptr, "Code C" ) &&
            !streq_plus ( ptr, "Shift A" ) && !streq_plus ( ptr, "Shift B" ) )
        {
            if ( !streq_plus ( ptr, "FNC 1" ) && !streq_plus ( ptr, "Stop" ) )
            {
                if ( sscanf ( ptr, "%i", &num_high ) <= 0 )
                {
                    fprintf ( stderr, "Error: %s expected first number at ...%s\n",
                        code128_str_subtype ( subtype ), ptr );
                    return -1;
                }

                aux = ptr;

                input_symbol_next ( &aux );

                if ( !aux )
                {
                    fprintf ( stderr, "Error: %s expected second number after ...%s\n",
                        code128_str_subtype ( subtype ), ptr );
                    return -1;
                }

                ptr = aux;

                if ( sscanf ( ptr, "%i", &num_low ) <= 0 )
                {
                    fprintf ( stderr, "Error: %s expected second number at ...%s\n",
                        code128_str_subtype ( subtype ), ptr );
                    return -1;
                }

                num = num_high * 10 + num_low;

                if ( snprintf ( num_buf, sizeof ( num_buf ), "%.2i", num ) <= 0 )
                {
                    fprintf ( stderr, "Error: %s format number failed at ...%s\n",
                        code128_str_subtype ( subtype ), ptr );
                    return -1;
                }

                aux = num_buf;
            }
        }

        if ( !( encoded = encode_symbol ( subtype, aux, &index ) ) )
        {
            fprintf ( stderr, "Error: %s encoding exception at ...%s\n",
                code128_str_subtype ( subtype ), ptr );
            return -1;
        }

        if ( byte_array_push_bytes ( array, encoded, strlen ( encoded ) ) )
        {
            return -1;
        }

        if ( pos )
        {
            checksum += pos * index;

        } else
        {
            checksum = index;
        }

        if ( streq_plus ( ptr, "Code A" ) )
        {
            switch_encoding ( &subtype, CODE128_A );

        } else if ( streq_plus ( ptr, "Code B" ) )
        {
            switch_encoding ( &subtype, CODE128_B );

        } else if ( streq_plus ( ptr, "Code C" ) )
        {
            switch_encoding ( &subtype, CODE128_C );

        } else if ( streq_plus ( ptr, "Shift B" ) )
        {
            subtype_backup = CODE128_A;
            switch_encoding ( &subtype, CODE128_B );

        } else if ( streq_plus ( ptr, "Shift A" ) )
        {
            subtype_backup = CODE128_B;
            switch_encoding ( &subtype, CODE128_A );
        }

        input_symbol_next ( &ptr );
    }

    checksum %= 103;

    if ( checksum >= n )
    {
        fprintf ( stderr, "Error: Checksum mapping out of range (%llu >= %lu).\n", checksum,
            ( unsigned long ) n );
        return -1;
    }

    printf ( "checksum (modulo 103) is %i.\n", ( int ) checksum );

    encoded = dictionary[checksum][3];

    /* Checksum */
    if ( byte_array_push_bytes ( array, encoded, strlen ( encoded ) ) )
    {
        return -1;
    }

    if ( !( encoded = encode_symbol ( subtype, "Stop", NULL ) ) )
    {
        fprintf ( stderr, "Error: %s encoding exception at (Stop)\n",
            code128_str_subtype ( subtype ) );
        return -1;
    }

    /* Stop pattern */
    if ( byte_array_push_bytes ( array, "2331112", 7 * sizeof ( char ) ) )
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

void swap_pixel ( struct pixel_t *a, struct pixel_t *b )
{
    struct pixel_t tmp;
    memcpy ( &tmp, a, sizeof ( struct pixel_t ) );
    memcpy ( a, b, sizeof ( struct pixel_t ) );
    memcpy ( b, &tmp, sizeof ( struct pixel_t ) );
}

/**
 * Plot barcode type CODE-128
 */
int plot_barcode_code128 ( const struct barcode_plot_options_t *options )
{
    int i;
    int j;
    int width;
    int height;
    size_t len;
    void *buf;
    struct byte_array_t array;
    struct pixel_t *bitmap;
    struct pixel_t *xbitmap;

    printf ( "making a code128 barcode...\n" );
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

    if ( options->rotate )
    {
        if ( !( xbitmap =
                ( struct pixel_t * ) malloc ( options->width * options->height *
                    sizeof ( struct pixel_t ) ) ) )
        {
            fprintf ( stderr, "Error: Unable to allocate rotated bitmap: %i\n", errno );
            free ( bitmap );
            return -1;
        }

        for ( i = options->width - 1; i >= 0; --i )
        {
            for ( j = options->height - 1; j >= 0; --j )
            {
                /* use formulas: x' = y; y' = -x' to rotate left */
                /* rotated[i][j] = plain[j][width-i-1]; */
                memcpy ( xbitmap + options->height * i + j, bitmap + options->width * j + i,
                    sizeof ( struct pixel_t ) );
            }
        }

        free ( bitmap );
        bitmap = xbitmap;

        width = options->height;
        height = options->width;

    } else
    {
        width = options->width;
        height = options->height;
    }

    if ( !( buf = make_png_buffer_rgba ( ( uint8_t * ) bitmap, width, height, &len ) ) )
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
