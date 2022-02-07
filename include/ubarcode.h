/* ------------------------------------------------------------------
 * uBarcode - Shared Project Header
 * ------------------------------------------------------------------ */

#ifndef UBARCODE_H
#define UBARCODE_H

#include "config.h"
#include "common.h"
#include "code128.h"
#include "code39.h"
#include "codabar.h"
#include "code93.h"

/**
 * RGBA Pixel strucure
 */
struct pixel_t
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
} __attribute__( ( packed ) );

/**
 * Byte array structure
 */
struct byte_array_t
{
    size_t len;
    size_t size;
    uint8_t *data;
};

/**
 * Create new byte array
 */
extern int byte_array_new ( struct byte_array_t *array, size_t initial_size );

/**
 * Push byte on top of byte array
 */
extern int byte_array_push_byte ( struct byte_array_t *array, uint8_t byte );

/**
 * Push bytes on top of byte array
 */
extern int byte_array_push_bytes ( struct byte_array_t *array, const void *buf, size_t len );

/**
 * Free byte array
 */
extern void byte_array_free ( struct byte_array_t *array );

/**
 * Write complete data to file
 */
extern int write_complete ( int fd, const void *buf, size_t tot );

/**
 * Plot barcode type generic 
 */
extern int plot_barcode ( const struct barcode_plot_options_t *options );

/**
 * Check if strings are equal until plus sign is found
 */
extern int streq_plus ( const char *a, const char *b );

/**
 * Obtain next input symbol by plus sign separator
 */
extern void input_symbol_next ( const char **ptr );

/**
 * Count input symbols
 */
extern size_t count_input_symbols ( const char *input );

/**
 * Plot vertical bar
 */
extern void plot_draw_bar ( struct pixel_t *bitmap, int x0, int d, int width, int height,
    int white );

/**
 * Write an rgba image to a memory buffer in PNG format, without any fanciness
 */
extern void *make_png_buffer_rgba ( unsigned char *rgba, int width, int height, size_t *outsize );

/**
 * Export picture to file
 */
extern int export_picture ( const void *buf, size_t len, const char *path );

#endif
