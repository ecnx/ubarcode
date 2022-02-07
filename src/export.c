/* ------------------------------------------------------------------
 * uBarcode - Picture Exporting
 * ------------------------------------------------------------------ */

#include "ubarcode.h"

/**
 * Export picture to file
 */
int export_picture ( const void *buf, size_t len, const char *path )
{
    int fd;

    if ( ( fd = open ( path, O_CREAT | O_TRUNC | O_WRONLY, 0644 ) ) < 0 )
    {
        return -1;
    }

    if ( write_complete ( fd, buf, len ) < 0 )
    {
        close ( fd );
        return -1;
    }

    close ( fd );
    return 0;
}
