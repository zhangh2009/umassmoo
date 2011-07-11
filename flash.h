#ifndef FLASH_H
#define FLASH_H
/*Software Driver

SST25WF040 4 Mbit Serial Flash Memory

June 11, 2011, Rev. 1.0

ABOUT THE SOFTWARE
This application note provides software driver examples for SST25WF040,
Serial Flash. Extensive comments are included in each routine to describe 
the function of each routine.  The interface coding uses polling methods 
of the SPI protocol to interface with these serial devices.  The functions 
are differentiated below in terms of the communication protocols(uses 
Mode 0) and specific device operation instructions. This code has been 
designed to compile using the IAR C/C++ Compiler for MSP430.


ABOUT THE SST25WF040

Companion product datasheets for the SST25WF040 should be reviewed in 
conjunction with this application note for a complete understanding 
of the device.


Device Communication Protocol(pinout related) functions:

Functions                    		Function
------------------------------------------------------------------
init_spi				Initializes clock to set up mode 0.
Send_Byte				Sends one byte by putting it in TX buffer
Get_Byte				Receives one byte from RX buffer 
CE_High					Sets Chip Enable pin of the serial flash to high
CE_Low					Clears Chip Enable of the serial flash to low

Note:  The pin names of the SST25WF040 are used in this application note. 
The associated test code will not compile unless these pinouts (SCK, SIMO, SOMI,CE) 
are pre-defined on your software which should reflect your hardware interfaced. 	 

Device Operation Instruction functions:

Functions                    		Function
------------------------------------------------------------------
Read_Status_Register			Reads the status register of the serial flash
EWSR					Enables the Write Status Register
WRSR					Performs a write to the Status Register 
WREN					Write enables the serial flash
WRDI					Write disables the serial flash
Read_ID					Reads the manufacturer ID and device ID
Chip_Erase				Erases entire serial flash
Sector_Erase				Erases one sector (4 KB) of the serial flash
Block_Erase_32K				Erases 32 KByte block memory of the serial flash
Block_Erase_64K				Erases 64 KByte block memory of the serial flash
Read					Reads one byte from the serial flash and returns byte(max of 20 MHz CLK frequency)
Byte_Program				Program one byte to the serial flash
*/

void init_spi();
void Send_Byte(unsigned char out);
unsigned char Get_Byte();
void CE_High();
void CE_Low();
unsigned char Read_Status_Register();
void EWSR();
void WRSR(unsigned char byte);
void WREN();
void WRDI();
unsigned char Read_ID(unsigned char ID_addr);
void Chip_Erase();
void Sector_Erase(unsigned long Dst);
void Block_Erase_32K(unsigned long Dst);
void Block_Erase_64K(unsigned long Dst);
unsigned char Read(unsigned long Dst);
void Byte_Program(unsigned long Dst, unsigned char byte);

#endif // FLASH_H
