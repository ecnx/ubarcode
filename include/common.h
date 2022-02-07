/* ------------------------------------------------------------------
 * uBarcode - Common Definitions
 * ------------------------------------------------------------------ */

#ifndef UBARCODE_COMMON_H
#define UBARCODE_COMMON_H

/**
 * Barcode plot options structure
 */
struct barcode_plot_options_t
{
    int rotate;
    const char *type;
    int width;
    int height;
    const char *input;
    const char *bmpfile;
};

#endif
