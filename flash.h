/* See license.txt for license information. */

#ifndef FLASH_H
#define FLASH_H

#define EXTFLASH_PORT_OUT P5OUT
#define EXTFLASH_CHIP_ENABLE_PIN 0x01 // pin 1 wired to extflash's CE# pin

// flash commands
#define EXTFLASH_BYTE_PROGRAM_COMMAND 0x02
#define EXTFLASH_BYTE_READ_COMMAND 0x03
#define EXTFLASH_WRITE_ENABLE_COMMAND 0x06

unsigned char extflash_read_uchar (unsigned long);
unsigned char extflash_write_uchar (unsigned long, unsigned char);
void extflash_enable_chip (void);
void extflash_disable_chip (void);
void extflash_send_byte (unsigned char);

#endif // FLASH_H
