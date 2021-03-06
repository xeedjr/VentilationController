/*
 * nrf24Manager.cpp
 *
 *  Created on: 24 ���. 2018 �.
 *      Author: Bogdan
 */

#include "nrf24Manager.h"

#include "nrf24/RF24.h"
#include "nrf24/RF24HALChibios.h"

nrf24Manager::nrf24Manager() {
	// TODO Auto-generated constructor stub
	RF24* radio = nullptr;
	RF24HAL* radio_hal = new RF24HAL_Chibios();

	radio = new RF24(radio_hal);

	  radio->begin();                           // Setup and configure rf radio
	  radio->setChannel(2);
	  radio->setPALevel(RF24_PA_MAX);
	  radio->setDataRate(RF24_1MBPS);
	  radio->setAutoAck(1);                     // Ensure autoACK is enabled
	  radio->setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
	  radio->setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
//	  radio->printDetails();
	  radio->setPayloadSize(1);

      radio->openWritingPipe(addresses[0]);
      radio->openReadingPipe(1,addresses[1]);
      radio->startListening();

}

nrf24Manager::~nrf24Manager() {
	// TODO Auto-generated destructor stub
}

