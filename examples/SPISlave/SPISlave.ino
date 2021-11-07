/*
Sample project for SPI Slave Mode for the Teensy 3.5 / 3.6.
This is a work in progress; the following code probably doesn't work on any
other microcontrollers beside the Teensy 3.5 and 3.6 and it was only tested
on the Teensy 3.5. Hardware-specific portions will probably be made part of
the library at a later stage, to make this more portable.

Copyright (C) 2021 Jac Goudsmit
Licensed under the GNU GPL 2.0 or the GNU Lesser GPL 2.1.

Code was based on the TSPISlave library by tonton81 (https://github.com/tonton81/TSPISlave)
and a forum thread at https://forum.pjrc.com/threads/56566-Teensy-3-5-SPI-Interrupt-Routine-on-SPI-Bus-1-or-2

The program is supposed to run while the Teensy is connected as slave to
another Arduino that's putting messages on the SPI bus. For example you can
use the master sample program by Nick Gammon from 
http://www.gammon.com.au/forum/?id=10892 on the master.

Note: On the Teensy pinout cards from the PJRC.com website, the Serial Data
Out pins are marked MOSI and the Serial Data In pins are marked MISO. In
slave mode, the direction of the pins doesn't change, so function of the
pins does change. In other words: the pins that are marked as "MISO" on the
drawing become "MOSI" in slave mode, and vice versa. The library uses the
usual pin names, so when you set the MISO pin, the code changes the SDI pin
in master mode, or changes the SDO pin in slave mode. You should always
call begin() first before you change the configuration.
*/

#include "SPI.h"

#ifndef SPI_HAS_SLAVEMODE
#error "Sorry I need SPI Slave Mode. https://github.com/jacgoudsmit/SPI"
#endif

volatile KINETISK_SPI_t * volatile p = &KINETISK_SPI0;
volatile bool ready;

void setup(void)
{
    Serial.begin(115200);
    Serial.println("\nHello World!");

    // Start in slave mode
    SPI.begin(true);
    Serial.println("Ready, set...");

    // By default: SPI0
    // SDI (MOSI) = 12 (Alternates 8, 39)
    // SDO (MISO) = 11 (Alternates 7, 28)
    // SCK = 13 (also LED) (Alternates 14, 27)
    // SS = 10 (Alternates 2, 9, 26 (master/slave), 20, 23, 21, 22, 15, 6, 45 (master only))

    // Setup interrupt
    p->RSER = SPI_RSER_RFDF_RE; // generate IRQ (which calls spi0_isr()) on receive
    NVIC_SET_PRIORITY(IRQ_SPI0, 1); // set IRQ priority
    NVIC_ENABLE_IRQ(IRQ_SPI0); // enable IRQ

    Serial.println("\nsetup done\n");

    ready = true;
}  // end of setup

void loop(void)
{
    Serial.println(millis());
    delay(1000);
}  // end of loop

void spi0_isr() { // Reserved function name; do not change
    if (ready)
    {
        Serial.println("IRQ");
        while ( 0 != (p->SR & 0xF0) ) {
            p->PUSHR = 0;
            Serial.print("VALUE: 0x");
            Serial.println(p->POPR, HEX);
        }
        Serial.println("EOI");
    }
    else
    {
        p->PUSHR = 0;
    }

    // Clear the Receive FIFO Drain Flag so we can receive the next
    // interrupt
    // NOTE: There may be a race condition here: If a frame arrived between
    // the last time that the "while" above detected that the FIFO was empty
    // and this instruction, and there are no further frames incoming,
    // will we get a receive interrupt?
    p->SR |= SPI_SR_RFDF;
}