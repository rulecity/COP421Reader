# COP421Reader
This is a hardware reader for the COP421 Microcontroller.

I designed it very quickly just to read the COP421 microcontroller used by the RDI Halcyon (since I have/had one temporarily on loan).  It turns out it's identical to the Thayer's Quest arcade COP421 program, but it's nice to know for sure.

I relied heavily on the information at this page to contruct my reader: http://john.ccac.rwth-aachen.de:8000/patrick/COPSreader.htm

## Building
### KiCad files
I started with an EAGLE project that I made for Star Rider for this PCB, so you will see references to Star Rider in the Kicad project.  I imported some EAGLE symbols and used some native Kicad symbols so you'll see a hybrid of both.  (like I said, this was a quick n' dirty project since I intended it to only be used once).  I used KiCad v6 for this project.

### AVR files
Source code for the ATMega644p microcontroller, as well as a Makefile and a script to install via AVR Dragon / AVRDude.
The AVR reads from the COP421 and outputs the resulting data to USB serial port.

### Desktop
Source code to build a simple desktop (Windows/Linux/Mac) application to read the data from the AVR (via USB serial port) and save to a file.

### Bill of Materials
CSV file showing which parts I used on the PCB.

## Running
- Put COP421 into ZIF socket in custom PCB.
- Power on custom PCB with 5V wall wart.
- Plug in USB cable from desktop device (Windows/Linux/Mac) into custom PCB.  The USB serial device should be registered automatically.
- Run the desktop command line application.  You should see a "Listening" prompt.
- Hit the RESET button on the custom PCB.
- After a few seconds, the desktop app should report CRC success or fail.  On success, it will report the file path that it has written the dumped binary to.  You may repeat this (by repeatedly pressing RESET) to confirm that you've got a consistent read.
