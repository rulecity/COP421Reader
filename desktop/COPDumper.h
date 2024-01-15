#ifndef HITACHI_LDP_TESTER_COPDUMPER_H
#define HITACHI_LDP_TESTER_COPDUMPER_H


#include <mpolib/ISerial.h>

class COPDumper
{
public:
	explicit COPDumper(ISerial *pSerial)
	{
		this->m_pSerial = pSerial;
	}

	void DoIt();

	ISerial* m_pSerial;
	uint16_t m_crc;
};


#endif //HITACHI_LDP_TESTER_COPDUMPER_H
