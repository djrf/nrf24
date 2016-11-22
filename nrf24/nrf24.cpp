// include the SPI library:
#include <Arduino.h>
#include <nrf24.h>

NRF24 nrf24;

void NRF24::initialize(int CE, int CSN)
{
	m_CE = CE;
	m_CSN = CSN;
}

void NRF24::writeRegister(uint8_t address, uint8_t value)
{
	digitalWrite(m_CSN, LOW);
	delayMicroseconds(10);

	SPI.transfer(0x20 | ( 0x1f & address));
	SPI.transfer(value);

	digitalWrite(m_CSN, HIGH);
	delayMicroseconds(10);
}

uint8_t NRF24::readRegister(uint8_t address)
{
	digitalWrite(m_CSN, LOW);
	delayMicroseconds(10);

	SPI.transfer(0x1f & address);
	uint8_t ret = SPI.transfer(0x00);

	digitalWrite(m_CSN, HIGH);
	delayMicroseconds(10);

	return ret;
}

uint8_t NRF24::readStatus()
{
	digitalWrite(m_CSN, LOW);
	delayMicroseconds(10);

	uint8_t ret = SPI.transfer(0xff);

	digitalWrite(m_CSN, HIGH);
	delayMicroseconds(10);

	return ret;
}

void NRF24::flushTxFifo()
{
	digitalWrite(m_CSN, LOW);
	delayMicroseconds(10);

	SPI.transfer(0xe1);

	digitalWrite(m_CSN, HIGH);
	delayMicroseconds(10);
}

void NRF24::sendPacket(uint8_t* packet, size_t len)
{
	digitalWrite(m_CSN, LOW);
	delayMicroseconds(10);

	SPI.transfer(0xa0);
	SPI.transfer(packet, len);

	digitalWrite(m_CSN, HIGH);
	delayMicroseconds(10);

	while((readRegister(0x17) & 0x10) == 0) {  // while FIFO not empty
		if((readStatus() & 0x10) != 0) {         // max rt?
			flushTxFifo();

			// ACK interrupt
			writeRegister(0x07, 0x10);
		}
	}
}

void NRF24::receivePacket(uint8_t* packet, size_t len)
{
	digitalWrite(m_CSN, LOW);
	delayMicroseconds(10);

	SPI.transfer(0x61);
	for(int i=0; i<len; i++)
		packet[i] = SPI.transfer(0x00);

	digitalWrite(m_CSN, HIGH);
	delayMicroseconds(10);
}

void NRF24::setTxAddress(uint64_t address)
{
	uint8_t addr[5];

	for(int i=0; i<5; i++)
		addr[i]=(address>>(8*i)) & 0xff;

	digitalWrite(m_CSN, LOW);
	delayMicroseconds(10);

	SPI.transfer(0x20 | 0x10);
	for(int i=0; 0<5; i++)
		SPI.transfer(addr[i]);

	digitalWrite(m_CSN, HIGH);
	delayMicroseconds(10);
}

void NRF24::setRxAddress(uint8_t pipe, uint64_t address)
{
	uint8_t addr[5];

	for(int i=0; i<5; i++)
		addr[i]=(address>>(8*i)) & 0xff;

	digitalWrite(m_CSN, LOW);
	delayMicroseconds(10);

	SPI.transfer(0x20 | (0x0a+pipe));
	for(int i=0; 0<5; i++)
		SPI.transfer(addr[i]);

	digitalWrite(m_CSN, HIGH);
	delayMicroseconds(10);
}
