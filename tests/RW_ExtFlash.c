#include  "msp430x26x.h"
#include "moo.h"
#include "flash.h"

void main(void)
{
  unsigned char rcv_data;
  unsigned long fadd;

  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer

  P2SEL = 0;
  P2IFG = 0;
  
  init_spi();       // Initialize the flash memory
  Chip_Erase();     // Erase the chip
  
  rcv_data = Read_Status_Register();
  while(rcv_data & 0x9C)
  {
    WRSR(0x02);
  }
  
  fadd = 0x100;    // Start address: 256, end address: 65535
  rcv_data = 0;

  while(fadd < 0x010000 ) // end address: 65535
  { 
    Byte_Program(fadd, rcv_data); // Write
    rcv_data = Read(fadd); // Read
    rcv_data = rcv_data + 1;
    fadd = fadd + 1;
  }
}
