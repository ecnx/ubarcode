/* ------------------------------------------------------------------
 * uBarcode - Utility Stuff
 * ------------------------------------------------------------------ */

#include "ubarcode.h"

/**
 * Create new byte array
 */
int byte_array_new ( struct byte_array_t *array, size_t initial_size )
{
    memset ( array, '\0', sizeof ( struct byte_array_t ) );

    if ( !( array->data = ( uint8_t * ) malloc ( initial_size ) ) )
    {
        return -1;
    }

    array->len = 0;
    array->size = initial_size;

    return 0;
}

/**
 * Push byte on top of byte array
 */
int byte_array_push_byte ( struct byte_array_t *array, uint8_t byte )
{
    uint8_t *new_data;

    if ( array->len + 1 >= array->size )
    {
        if ( !( new_data = ( uint8_t * ) realloc ( array->data, array->size << 1 ) ) )
        {
            return -1;
        }

        array->data = new_data;
        array->size <<= 1;
    }

    array->data[array->len++] = byte;

    return 0;
}

/**
 * Push bytes on top of byte array
 */
int byte_array_push_bytes ( struct byte_array_t *array, const void *buf, size_t len )
{
    const uint8_t *ptr;
    const uint8_t *lim;

    for ( ptr = ( const uint8_t * ) buf, lim = ptr + len; ptr < lim; ptr++ )
    {
        if ( byte_array_push_byte ( array, *ptr ) < 0 )
        {
            return -1;
        }
    }

    return 0;
}

/**
 * Free byte array
 */
void byte_array_free ( struct byte_array_t *array )
{
    if ( array->data )
    {
        free ( array->data );
        array->data = NULL;
    }
}

/**
 * Write complete data to file
 */
int write_complete ( int fd, const void *buf, size_t tot )
{
    size_t len;
    size_t sum;

    for ( sum = 0; sum < tot; sum += len )
    {
        if ( ( ssize_t ) ( len =
                write ( fd, ( const unsigned char * ) buf + sum, tot - sum ) ) <= 0 )
        {
            return -1;
        }
    }

    return 0;
}
