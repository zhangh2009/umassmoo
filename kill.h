/* See license.txt for license information. */

#define KILLSWITCH_ADDR 0x7FFFE // penultimate byte of 4-Mbit flash
                                // XXX choose nonrandomly?

unsigned char kill (void);
unsigned char is_killed (void);
