/*
 * BLExtractor.cpp
 *
 *  Created on: 15 ���. 2016 �.
 *      Author: Bogdan
 */

#include <memory>
#include <string.h>
#include "BLExtractor.h"
#include "ReleGPIO.h"
#include "MBComunication.h"

extern std::unique_ptr<ReleGPIO> rele1;
extern std::unique_ptr<MBComunication> mb_comunication;


BLExtractor::BLExtractor() {
	thread_ID = osThreadCreate(osThread(BLExtractor_Thread), this);
	if (thread_ID == nullptr) {
		/// Error
		chSysHalt("Error create thread");
	}

	si7021_.init(&I2CD1);

	/// create period read humidity sensor data
	osTimerId timer0 = osTimerCreate(osTimer(timer0_handle), osTimerPeriodic, (void *)this);
	osTimerStart (timer0, 100);
}

BLExtractor::~BLExtractor() {
	// TODO Auto-generated destructor stub
}

void BLExtractor::on_change_fan_speed(Events::ChangeFanSpeed* msg) {
	curr_fan_speed_ = msg->speed;
	if (msg->speed == 0) {
		/// Off
		rele1->Set(true);
	} else {
		/// On
		rele1->Set(false);
	}

	/// response execute code
	//mb_comunication->send_resp_OK(Commands_change_speed_tag);
}

void BLExtractor::on_proto_message(Events::ProtoMsg* message) {
/*	switch(msg.which_interface) {
	case MBMessage_commands_tag:
		switch(msg.interface.commands.which_cmd) {
		case Commands_change_speed_tag:
			{
				ev_pair.ev_type = Events::kChangeFanSpeed;
				ev_pair.events.change_fan_speed.speed = msg.interface.commands.cmd.change_speed.fan_speed;
				put_event(ev_pair);
			}
			break;
		}
		break;
	}
	*/
}

void BLExtractor::on_read_measurements() {
	float temperature = 0, hummidity = 0;
	float percent = 0.5;

	si7021_.read_temperature(temperature);
	si7021_.read_humidity(hummidity);

	if (((hummidity < (humidity_ - 0.9)) || ((humidity_ + 0.9) < hummidity)) ||
		((temperature < (temperature_ - 0.5)) || ((temperature_ + 0.5) < temperature))) {
		/// values changed
		/// Store
		humidity_ = hummidity;
		temperature_ = temperature;

		/// Send notification
		mb_comunication->send_notification_temp_humidity(humidity_,
				temperature_,
				curr_fan_speed_);
	}
}

void BLExtractor::put_event(Events ev) {

}

void BLExtractor::BLExtractor_Thread (void const *argument) {
	BLExtractor* p = (BLExtractor*)(argument);

	p->Thread();
}

void BLExtractor::Thread (void) {
	Events ev;

	while(1) {
		events_queue_.pop(ev);

		switch(ev.hdr.msgid) {
		case kChangeFanSpeed:
			on_change_fan_speed(&ev.change_fan_speed);
			break;
		case kMeasure:
			on_proto_message(&ev.proto_msg);
			break;
		case kUpdateValuesInMBRegisters:
			break;
		case kReadMeasurements:
			on_read_measurements();
			break;
		case kProtoMsg:
			break;
		}
	}
}

void BLExtractor::BLExtractor_Timer (void const *argument) {
	BLExtractor* p = (BLExtractor*)(argument);

	p->Timer();
}

void BLExtractor::Timer (void) {
	Events ev;
	ev.hdr.msgid = kReadMeasurements;
	events_queue_.push(ev);
}
