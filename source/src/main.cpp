//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "ch.hpp"
#include "hal.h"
#include "cmsis_os.h"
#include "mb.h"
#include "MBComunication.h"
#include "BLExtractor.h"
#include "ReleGPIO.h"
#include "Button.h"
#include "ButtonsMain.h"
#include "SensorButton.h"
#include "Servo.h"
#include "ESP8266Parser.h"
#include "BackupRegister.h"
#include "RfremoteNECReceiver.h"
#include "RfReceiverPT2262.h"
#include "HT12D.h"
#include "LEDBlinker.h"
#include "nrf24Manager.h"

using namespace chibios_rt;

std::unique_ptr<MBComunication> mb_comunication;
std::unique_ptr<BLExtractor> bl;
std::unique_ptr<ReleGPIO> rele1;
LEDBlinker led_blinker;


void extcb_gpioa_0(EXTDriver *extp, expchannel_t channel) {
	//rf_remote_nec.extcb_gpioa_0(extp, channel);
}


/*
 * I2C1 config.
 */
static const I2CConfig i2cfg1 = {
    OPMODE_I2C,
    400000,
    FAST_DUTY_CYCLE_2,
};

/*
 * GPT4 callback.
 */
static void gpt4cb(GPTDriver *gptp) {
}

static const GPTConfig gpt4cfg = {
	GPT_FREQ,    /* 10kHz timer clock.*/
  gpt4cb,   /* Timer callback.*/
  0,
  0
};

static const EXTConfig extcfg = {
  {
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL}
  }
};


eMBException
eMBFuncPacket( UCHAR * pucFrame, USHORT * usLen )
{
	if (mb_comunication) {
		return mb_comunication->eMBFuncPacket(pucFrame,
												usLen);
	} else {
	    return MB_EX_NONE;
	}

    return MB_EX_NONE;
}


/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  osKernelInitialize();

  stm32_backup_registers.write(1, 1);
  	uint16_t address = stm32_backup_registers.read(1);
  	uint8_t address1 = address & 0x0F;
	eMBInit( MB_RTU, address1, 1, 115200, MB_PAR_NONE );
	eMBRegisterCB( 25, eMBFuncPacket);

	xMBPortPollThreadInit();

	rele1.reset(new ReleGPIO(USER_RELE1_PORT, USER_RELE1));
	bl.reset(new BLExtractor);
	mb_comunication.reset(new MBComunication);

//	  esp.reset(new ESP8266Parser);

	eMBEnable();

	i2cStart(&I2CD1, &i2cfg1);
	//sdStart(&SD2, &sd2_config);
	gptStart(&GPTD4, &gpt4cfg);
	gptStartContinuous(&GPTD4, 2);

	led_blinker.init(INDICATE_PORT, (uint8_t)INDICATE, 500, 500);

	/*
	* Activates the EXT driver 1.
	*/
	extStart(&EXTD1, &extcfg);

	nrf24Manager* nrf_manager = new nrf24Manager();

  while (true) {
	  osDelay(1);
  }

  return 0;
}

// ----------------------------------------------------------------------------
