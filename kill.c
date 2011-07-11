/* See license.txt for license information. */

#include "kill.h"
#include "flash.h"

unsigned char kill (void) {
    return extflash_write_uchar (KILLSWITCH_ADDR, 1);
}
