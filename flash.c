/* See license.txt for license information. */

#include "moo.h"
#include "flash.h"

unsigned char extflash_read_uchar (unsigned long addr) {
    return 0; // XXX
}

void extflash_enable_chip (void) {
    // lower the CHIP ENABLE pin to enable extflash chip
    EXTFLASH_PORT_OUT &= ~EXTFLASH_CHIP_ENABLE_PIN;
}

void extflash_disable_chip (void) {
    // raise the CHIP ENABLE pin to disable extflash chip
    EXTFLASH_PORT_OUT |= EXTFLASH_CHIP_ENABLE_PIN;
}

void extflash_send_byte (unsigned char c) {
    UCB1TXBUF = c; // place c into transmit buffer
    while (!(UC1IFG & UCB1TXIFG)); // busy-wait until flag indicates TX is done
}

unsigned char extflash_write_uchar (unsigned long addr, unsigned char c) {
    extflash_enable_chip();
    extflash_send_byte(EXTFLASH_WRITE_ENABLE_COMMAND);

    extflash_send_byte(EXTFLASH_BYTE_PROGRAM_COMMAND);

    // send three bytes of address, from upper to lower
    extflash_send_byte((addr & 0xFF0000) >> 16);
    extflash_send_byte((addr & 0xFF00) >> 8);
    extflash_send_byte(addr & 0xFF);

    // send the character to write
    extflash_send_byte(c);

    extflash_disable_chip();
    return 1; // XXX
}
