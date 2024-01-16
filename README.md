# COP421Reader
This is a hardware reader for the COP421 Microcontroller.

I designed it very quickly just to read the COP421 microcontroller used by the RDI Halcyon (since I have/had one temporarily on loan).  It turns out it's identical to the Thayer's Quest arcade COP421 program, but it's nice to know for sure.

I relied heavily on the information at this page to contruct my reader: http://john.ccac.rwth-aachen.de:8000/patrick/COPSreader.htm

# KiCad files
I started with an EAGLE project that I made for Star Rider for this PCB, so you will see references to Star Rider in the Kicad project.  I imported some EAGLE symbols and used some native Kicad symbols so you'll see a hybrid of both.  (like I said, this was a quick n' dirty project since I intended it to only be used once).  I used KiCad v6 for this project.

# AVR files
Source code for the ATMega644p microcontroller, as well as a Makefile and a script to install via AVR Dragon / AVRDude.
The AVR reads from the COP421 and outputs the resulting data to USB serial port.

# Desktop
Source code to build a simple desktop (Windows/Linux/Mac) application to read the data from the AVR (via USB serial port) and save to a file.

# Bill of Materials
CSV file showing which parts I used on the PCB.
