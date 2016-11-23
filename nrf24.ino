#include <nrf24.h>

const int CE = 7;
const int CSN = 3;
const int channel = 40;
const int INT = 2;

const size_t payload_length = 4;
uint8_t packet[payload_length] = {0xca,0xfe,0xba,0xbe};


void setup() {
	// set the slaveSelectPin as an output:
	pinMode(CE, OUTPUT);
	pinMode(CSN, OUTPUT);

	digitalWrite(CE, LOW);
	digitalWrite(CSN, HIGH);

	// initialize SPI:
	SPI.beginTransaction(SPISettings(80000, MSBFIRST, SPI_MODE0));
	SPI.begin();

	Serial.begin(9600);

	nrf24.initialize(CE, CSN);

	Serial.println("Power on (receiver)");
	nrf24.writeRegister(0x00, 0x0b);

	Serial.println("Set channel");
	nrf24.writeRegister(0x05, channel);

	Serial.println("Set payload length");
	nrf24.writeRegister(0x11, payload_length); // RX Pipe
	nrf24.writeRegister(0x12, payload_length); // BC Pipe

	//Serial.println("Set RX address")
	//setRxAddress(0, 0xA1A2A3A4A5);
	//setRxAddress(1, 0xBCBCBCBCBC);

	digitalWrite(CE, HIGH);
}

void loop() {

	// Data received
	if((nrf24.readStatus() & 0x40) != 0)
	{
		Serial.println("Data received:");

		// Read packet from FIFO
		nrf24.receivePacket(packet, payload_length);

		// Print packet
		for(int i=0; i<payload_length; i++)
			Serial.print(packet[i], HEX);
		Serial.println();

		// ACK interrupt
		nrf24.writeRegister(0x07, 0x40);

		Serial.print("Status: ");
		Serial.print(nrf24.readStatus(), HEX);
		Serial.print(" ");
		Serial.println(nrf24.readRegister(0x17), HEX);
	}
}

