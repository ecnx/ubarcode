/* ------------------------------------------------------------------
 * uBarcode - Plot barcode type generic
 * ------------------------------------------------------------------ */

#include "ubarcode.h"

/**
 * Plot barcode type generic 
 */
int plot_barcode ( const struct barcode_plot_options_t *options )
{
    if ( !strcmp ( options->type, "code128" ) )
    {
        return plot_barcode_code128 ( options );

    } else if ( !strcmp ( options->type, "code39" ) )
    {
        return plot_barcode_code39 ( options );

    } else if ( !strcmp ( options->type, "codabar" ) )
    {
        return plot_barcode_codabar ( options );

    } else if ( !strcmp ( options->type, "code93" ) )
    {
        return plot_barcode_code93 ( options );

    }

    fprintf ( stderr, "Error: Unknown barcode type (%s)\n", options->type );
    return -1;
}
