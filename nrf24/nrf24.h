#ifndef NRF24_H
#define NRF24_H

#include <Arduino.h>
#include <SPI.h>

class NRF24 {

protected:
	int m_CE, m_CSN;

public:
	void initialize(int CE, int CSN);
	void writeRegister(uint8_t address, uint8_t value);
	uint8_t readRegister(uint8_t address);
	uint8_t readStatus();
	void flushTxFifo();
	void sendPacket(uint8_t* packet, size_t len);
	void receivePacket(uint8_t* packet, size_t len);
	void setTxAddress(uint64_t address);
	void setRxAddress(uint8_t pipe, uint64_t address);
};

extern NRF24 nrf24;

#endif
