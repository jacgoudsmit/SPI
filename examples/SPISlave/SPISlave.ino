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

#ifndef SPI_HAS_SLAVE_MODE
#error "Sorry I need SPI Slave Mode. https://github.com/jacgoudsmit/SPI"
#endif

void setup(void)
{
    Serial.begin(115200);
    Serial.println("\nHello World!");

    // Start in slave mode
    SPI.begin(true);
    Serial.println("Ready, set...");

    // SPI0: Default pin | Alternate pin (Teensy 3.2/3.5/3.6) | Additional alternate pins (Teensy 3.5/3.6)
    // SDI (MOSI)       = 12 |  8 | 39
    // SDO (MISO)       = 11 |  7 | 28
    // SCK (also LED)   = 13 | 14 | 27
    // SS  (slave)      = 10 |  2 | 26
    // SS  (master)     = 10 |  2, 9, 6, 20, 23, 21, 22, 15 | 26, 45

    // Setup interrupt
    SPI.onReceive(receive, NULL);

    Serial.println("\nsetup done\n");
}  // end of setup

void loop(void)
{
    Serial.println(millis());
    delay(1000);
}  // end of loop

void receive(void *userdata, uint32_t rxdata)
{
    Serial.print("VALUE: 0x");
    Serial.println(rxdata, HEX);
}
