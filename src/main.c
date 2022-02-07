/* ------------------------------------------------------------------
 * uBarcode - Main Source File
 * ------------------------------------------------------------------ */

#include "ubarcode.h"

/**
 * Show program usage message
 */
static void show_usage ( void )
{
    fprintf ( stderr, "\nusage: ubarcode [--rotate] barcode-type width height input pngfile\n\n"
        "program options:\n"
        "    --rotate        rotate picture 90degs\n"
        "    barcode-type    barcode type\n"
        "    width           picture width in px\n"
        "    height          picture height in px\n"
        "    input           barcode input text\n"
        "    pngfile         output picture file\n\n"
        "supported barcode: %s\n\n", CODES_SUPPORTED );
}

/**
 * Program startup function
 */
int main ( int argc, char *argv[] )
{
    int argoff = 0;
    struct barcode_plot_options_t options = { 0 };

    printf ( "ubarcode - ver. %s\n", UBARCODE_VERSION );

    if ( argc < argoff + 6 )
    {
        show_usage (  );
        return 1;
    }

    if ( !strcmp ( argv[1], "--rotate" ) )
    {
        options.rotate = 1;
        argoff = 1;
    }

    if ( argc < argoff + 6 )
    {
        show_usage (  );
        return 1;
    }

    options.type = argv[argoff + 1];

    if ( sscanf ( argv[argoff + 2], "%i", &options.width ) <= 0 ||
        sscanf ( argv[argoff + 3], "%i", &options.height ) <= 0 )
    {
        show_usage (  );
        return 1;
    }

    if ( options.width <= 0 || options.height <= 0 )
    {
        show_usage (  );
        return 1;
    }

    options.input = argv[argoff + 4];
    options.bmpfile = argv[argoff + 5];

    if ( plot_barcode ( &options ) < 0 )
    {
        fprintf ( stderr, "plot barcode failed.\n" );
        return 1;
    }

    return 0;
}
