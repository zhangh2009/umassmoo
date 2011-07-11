/* See license.txt for license information. */
//******************************************************************************
//   UMass Moo Serial flash Driver - USCI_B1, SPI 3-Wire Master
//
//   Description: MCU talks to SST25WF040 flash using 3-wire mode. USCI RX ISR 
//   is used to handle communication with the MCU. 
//   Initialization waits for DCO to stabilize against ACLK.
//   ACLK = 32.768kHz, MCLK = SMCLK = DCO ~ 1048kHz.  BRCLK = SMCLK/2
//
//
//                    MSP430F261x
//                 -----------------
//             /|\|              XIN|-
//              | |                 |  32kHz xtal
//              --|RST          XOUT|-
//                |                 |
//                |             P5.1|-> Data Out (UCB1SIMO)
//                |                 |
//  Chip Enable <-|P5.0         P5.2|<- Data In (UCB1SOMI)
//                |                 |
//  Slave reset <-|RST         P5.3|-> Serial Clock Out (UCB1CLK)
//
//                    SST25WF040
//                 -----------------
//             /|\|                 |
//              | |                 | 
//     MCU P5.0 --|CE#           Vdd|- Vreg
//                |                 |
//     MCU P5.2 <-|SO     RST#/HOLD#|<- Master reset
//                |                 |
//          GND --|WP#           SCK|<- MCU P5.3 (UCB1CLK)
//                |                 |
//          GND --|Vss            SI|<- MCU P5.1 (UCB1SIMO)
//
//
//   Hong Zhang
//   PRISMS Lab, Computer Science, UMass, Amherst.
//   January 2011
//   Built with IAR Embedded Workbench Version: 4.21.2
//   (adapted from MSP430x261x_uscia0_spi_09.c from TI's MSP430F261x code
//    examples, and example "C" language Driver of SST25WF040 Serial Flash by
//    Hardik Patel, Microchip Technology Inc.)
//******************************************************************************
                                                                     
//"C" LANGUAGE DRIVERS 
#include  "msp430x26x.h"
#include "moo.h"
#include "flash.h"
// Define pin number to the port 5 of MCU MSP430F2618
#define CE    0x01
#define SIMO  0x02
#define SOMI  0x04
#define SCK   0x08

unsigned char iodata; // global variable to store read data

void init_spi()
{
  P5OUT |= 0x01;                            // P5 setup for slave reset
  P5DIR |= 0x01;                            // P5.0 set output for CE#
  P5SEL |= 0x0E;                            // P5.3,2,1 option select
  UCB1CTL0 |= UCMST+UCSYNC+UCCKPL+UCMSB;    //3-pin, 8-bit SPI master
  UCB1CTL1 |= UCSSEL_2;                     // SMCLK
  UCB1BR0 = 0x02;                           // /2
  UCB1BR1 = 0;                              //
  UCB1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  UC1IE |= UCB1RXIE;                        // Enable USCI_B1 RX interrupt
  _BIS_SR(GIE);  //LPM4_bits + GIE, CPU off, enable interrupts
}

/************************************************************************/
/* PROCEDURE: Send_Byte							*/
/*            This procedure outputs a byte to flash			*/
/* Input: out   							*/
/* Output: byte output in TX buffer then at SIMO pin	  	        */
/************************************************************************/
void Send_Byte(unsigned char out)
{
  UCB1TXBUF = out;
  while (!(UC1IFG & UCB1TXIFG));  // USCI_B1 TX buffer ready
}

/************************************************************************/
/* PROCEDURE: Get_Byte							*/
/*	This procedure trigers a byte clock cycle by send a null byte,	*/
/* 	and get a byte from RX Buffer when the RX ISR is called.	*/
/* Input:	Nothing							*/
/* Output:	in							*/
/************************************************************************/
unsigned char Get_Byte()
{
  volatile unsigned int i;

  Send_Byte(0x00);		// provide SCK for the read data
  for (i=5; i>0; i--);          // Add time delay between transmissions
  return iodata;
}

/************************************************************************/
/* PROCEDURE: CE_High							*/
/*            This procedure set CE = High.				*/
/* Input: None		        					*/
/* Output: CE	                					*/
/************************************************************************/
void CE_High() 
{
  P5OUT |= CE;			// set CE high
}

/************************************************************************/
/* PROCEDURE: CE_Low							*/
/*            This procedure drives the CE of the device to low. 	*/
/* Input: None		        					*/
/* Output: CE			        				*/
/************************************************************************/
void CE_Low() 
{	
  P5OUT &= ~CE;			// clear CE low
}

/************************************************************************/
/* PROCEDURE: Read_Status_Register					*/
/*            This procedure reads the status register and returns it.	*/
/* Input:	None							*/
/* Returns:	byte							*/
/************************************************************************/
unsigned char Read_Status_Register()
{
  unsigned char byte = 0;
  CE_Low();                     // CE low, enable device
  Send_Byte(0x05);		// send read status register command (05h)
  byte = Get_Byte();
  CE_High();
  return byte;
}

/************************************************************************/
/* PROCEDURE: EWSR							*/
/* This procedure Enables Write Status Register.  			*/
/* Input:	None							*/
/* Returns:	Nothing							*/
/************************************************************************/
void EWSR()
{
  CE_Low();			// enable device
  Send_Byte(0x50);		// enable writing to the status register
  CE_High();			// disable device
}

/************************************************************************/
/* PROCEDURE: WRSR							*/
/*	      This procedure writes to the Status Register.		*/
/* Input:	byte							*/
/* Returns:	Nothing							*/
/************************************************************************/
void WRSR(unsigned char byte)
{
  EWSR();
  CE_Low();
  Send_Byte(0x01);		// send write status register command (01h) 
  Send_Byte(byte);              // send new status register value
  CE_High();
}

/************************************************************************/
/* PROCEDURE: WREN							*/
/*            This procedure enables the Write Enable Latch.  It can 	*/
/*            also  be used to Enables Write Status Register.           */
/* Note:                                                                */
/*   The WREN instruction must be executed prior to any Write (Program/ */
/*   /Erase) operation. The WREN instruction may also be used to allow  */
/*   execution of the Write-Status-Register(WRSR) instruction. However, */
/*   the Write-Enable-Latch bit in the Status Register will be cleared  */
/*   upon the right edge CE# of the WRSR instruction.                   */ 
/* Input:	None							*/
/* Returns:	Nothing							*/
/************************************************************************/
void WREN()
{
  CE_Low();			// enable device 
  Send_Byte(0x06);		// send WREN command 
  CE_High();			// disable device
}

/************************************************************************/
/* PROCEDURE: WRDI							*/
/*            This procedure disables the Write Enable Latch.  It can 	*/
/*            also be used to disables Write Status Register.           */
/* Input:	None							*/
/* Returns:	Nothing							*/
/************************************************************************/
void WRDI()
{
  CE_Low();			// enable device 
  Send_Byte(0x04);		// send WREN command 
  CE_High();			// disable device
}

/************************************************************************/
/* PROCEDURE: Read_ID							*/
/* This procedure Reads the manufacturer's ID and device ID.  It will 	*/
/* use 90h or ABh as the command to read the ID (90h in this sample).   */
/* It is up to the user to give the last byte ID_addr to determine      */
/* whether the device outputs manufacturer's ID first, or device ID 	*/
/* first.  Please see the product datasheet for details.  Returns ID in */
/* variable byte.							*/
/*									*/
/* Input:	ID_addr							*/
/* Returns:	byte:	ID1(Manufacture's ID = BFh or Device ID = 04h)	*/
/************************************************************************/
unsigned char Read_ID(unsigned char ID_addr)
{
  CE_Low();                     // CE low, enable device 
  Send_Byte(0x90);		// send read ID command (90h or ABh)
  Send_Byte(0x00);		// send address 
  Send_Byte(0x00);		// send address
  Send_Byte(ID_addr);		// send address - either 00H or 01H 
  Get_Byte();
  CE_High();
  return iodata;
}

/************************************************************************/
/* PROCEDURE:	Chip_Erase						*/
/*            This procedure clears all bits in the device to FFH.	*/
/* Input:	Nothing                                          	*/
/* Returns:	Nothing							*/
/************************************************************************/
void Chip_Erase()
{
  WREN();
  CE_Low();
  Send_Byte(0x60); 			/* Erase the Chip */
  CE_High();
  // Delay 125ms
  for (long i = 0; i < 12500; i++) {;}
}

/************************************************************************/
/* PROCEDURE:	Block_Erase_32K						*/
/*  This procedure clears all bits in the selected 32 KByte block to FFH*/
/* Input:	Nothing                                          	*/
/* Returns:	Nothing							*/
/************************************************************************/
void Block_Erase_32K(unsigned long Dst)
{
  WREN();
  CE_Low();				/* enable device */
  Send_Byte(0x52);			/* send Sector Erase command */
  Send_Byte(((Dst & 0xFFFFFF) >> 16)); 	/* send 3 address bytes */
  Send_Byte(((Dst & 0xFFFF) >> 8));
  Send_Byte(Dst & 0xFF);
  CE_High();				/* disable device */
  // Delay 62ms
  for (long i = 0; i < 6200; i++) {;}
}

/************************************************************************/
/* PROCEDURE:	Block_Erase_64K						*/
/*  This procedure clears all bits in the selected 64 KByte block to FFH*/
/* Input:	Nothing                                          	*/
/* Returns:	Nothing							*/
/************************************************************************/
void Block_Erase_64K(unsigned long Dst)
{
  WREN();
  CE_Low();				/* enable device */
  Send_Byte(0xD8);			/* send Sector Erase command */
  Send_Byte(((Dst & 0xFFFFFF) >> 16)); 	/* send 3 address bytes */
  Send_Byte(((Dst & 0xFFFF) >> 8));
  Send_Byte(Dst & 0xFF);
  CE_High();				/* disable device */
  // Delay 62ms
  for (long i = 0; i < 6200; i++) {;}
}

/************************************************************************/
/* PROCEDURE:	Sector_Erase						*/
/*  This procedure clears all bits in the selected 4 KByte sector to FFH*/
/* Input:	Nothing                                          	*/
/* Returns:	Nothing							*/
/************************************************************************/
void Sector_Erase(unsigned long Dst)
{
  WREN();
  CE_Low();				/* enable device */
  Send_Byte(0x20);			/* send Sector Erase command */
  Send_Byte(((Dst & 0xFFFFFF) >> 16)); 	/* send 3 address bytes */
  Send_Byte(((Dst & 0xFFFF) >> 8));
  Send_Byte(Dst & 0xFF);
  CE_High();				/* disable device */
  // Delay 75ms
  for (long i = 0; i < 7500; i++) {;}
}

/************************************************************************/
/* PROCEDURE:	Read							*/
/*            This procedure reads one address of the device.  It will 	*/
/*            return the byte read in variable byte.			*/
/* Input:	Dst:	Destination Address 000000H - 7ffffH		*/
/* Returns:	byte							*/
/************************************************************************/
unsigned char Read(unsigned long Dst) 
{
  unsigned char byte = 0;
  CE_Low();                             // CE low, enable device
  Send_Byte(0x03);                      // read command 
  Send_Byte(((Dst & 0xFFFFFF) >> 16));	// send 3 address bytes 
  Send_Byte(((Dst & 0xFFFF) >> 8));
  Send_Byte(Dst & 0xFF);
  byte = Get_Byte();
  CE_High();
  return byte;		

}

/************************************************************************/
/* PROCEDURE:	Byte_Program						*/
/*            This procedure programs one address of the device.	*/
/* Assumption:  Address being programmed is already erased and is NOT	*/
/*		block protected.					*/
/* Input:	Dst:		Destination Address 000000H - 7ffffH	*/
/*		byte:		byte to be programmed           	*/
/* Returns:	Nothing							*/
/************************************************************************/
void Byte_Program(unsigned long Dst, unsigned char byte)
{
  WREN();
  CE_Low();
  Send_Byte(0x02); 			/* send Byte Program command */
  Send_Byte(((Dst & 0xFFFFFF) >> 16));	/* send 3 address bytes */
  Send_Byte(((Dst & 0xFFFF) >> 8));
  Send_Byte(Dst & 0xFF);
  Send_Byte(byte);			/* send byte to be programmed */
  CE_High();
  // Delay 60us
  for (long i = 0; i < 6; i++) {;}
}


/************************************************************************/
/* ISR: USCIB1RX_ISR							*/
/*  This ISR saves the received data from SPI bus to a global variable.	*/
/* Input:	Nothing							*/
/* Returns:	Nothing							*/
/************************************************************************/
#pragma vector=USCIAB1RX_VECTOR
__interrupt void USCIB1RX_ISR (void)
{

  if (UC1IFG & UCB1RXIFG)
  {
    iodata = UCB1RXBUF;
  }

}                                           

