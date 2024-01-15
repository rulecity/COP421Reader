#include <iostream>

#include <mpolib/SerialFactory.h>
#include "COPDumper.h"

int main(int argc, char **argv)
{
	if (argc == 2)
	{
		try
		{
			ISerialSPtr serial = SerialFactory::CreateInstance(argv[1]);

			auto *pDumper = new COPDumper(serial.get());
			serial->Init(115200, false);
			try
			{
				pDumper->DoIt();
			}
			catch (std::exception &ex)
			{
				fprintf(stderr, "Exception: %s", ex.what());
			}
			delete pDumper;
		}
		catch (std::exception &ex)
		{
			printf("Exception: %s\n", ex.what());
		}
	}
	else
	{
		printf("Command line: %s [path to serial device]\n", argv[0]);
        printf("Windows example of serial device: COM1\n");
        printf("Mac example of serial device: /dev/tty.usbserial-14620\n");
        printf("    (try 'ls -l /dev/*tty*serial*' to find a USB serial device)\n");
	}
	return 0;
}
