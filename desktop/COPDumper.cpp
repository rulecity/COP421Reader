#include "COPDumper.h"
#include <mpolib/mpo_numstr.h>
#include <mpolib/mpo_file_stream.h>

void COPDumper::DoIt()
{
	int fileOutputIdx = 0;

	// This is a quick n' dirty app.  Endless loop is acceptable :)
	for (;;)
	{
		printf("Listening!\n");
		fflush(stdout);

		uint8_t u8;
		size_t stBytesReceived = 0;

		uint8_t buf[1026];    // 1024 for buffer, +2 for CRC
		while (stBytesReceived < sizeof(buf))
		{
			size_t received = m_pSerial->Rx(&u8, 1, 1000);

			if (received != 0)
			{
				buf[stBytesReceived++] = u8;
			}
		}

		this->m_crc = 0xFFFF;

		for (int i = 0; i < 1024; i++)
		{
			uint16_t crc_new = (unsigned char)(m_crc >> 8) | (m_crc << 8);
			crc_new ^= buf[i];
			crc_new ^= (unsigned char)(crc_new & 0xff) >> 4;
			crc_new ^= crc_new << 12;
			crc_new ^= (crc_new & 0xff) << 5;
			m_crc = crc_new;
		}

		uint16_t u16ReceivedCRC = buf[1024] | (buf[1025] << 8);

		if (u16ReceivedCRC == m_crc)
		{
			printf("CRC's match!\n");

			string strFileName = "dump" + numstr::ToStr(fileOutputIdx++, 10, 2) + ".bin";
			blocking_sharedptr stream = MpoFileStreamFactory::CreateInstance(strFileName.c_str(), MPO_OPEN_CREATE);
			stream->Write(buf, 1024);
			printf("Wrote %s\n", strFileName.c_str());
		}
		else
		{
			printf("CRC mismatch.  Expected %04x but got %04x.\n", u16ReceivedCRC, m_crc);
		}
	}
}
