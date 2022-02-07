/* ------------------------------------------------------------------
 * uBarcode - Project Config Header
 * ------------------------------------------------------------------ */

#ifndef UBARCODE_CONFIG_H
#define UBARCODE_CONFIG_H

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define UBARCODE_VERSION "1.0.24"
#define CODES_SUPPORTED "code128 code39 codabar code93"

#define DEFAULT_X_PRINT_RES 0
#define DEFAULT_Y_PRINT_RES 0

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#endif
