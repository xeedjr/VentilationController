/*
 * BLExtractor.h
 *
 *  Created on: 15 окт. 2016 г.
 *      Author: Bogdan
 */

#ifndef BLEXTRACTOR_H_
#define BLEXTRACTOR_H_

#include <functional>
#include "cmsis_os.h"
#include "MailBox.h"
#include "SI7021.h"

class BLExtractor {

public:
	enum EventType {
		kChangeFanSpeed = 0,
		kMeasure,
		kUpdateValuesInMBRegisters,
		kReadMeasurements,
		kProtoMsg,
	};

	union Events {
		struct Header {
			EventType msgid;
		} hdr;
		struct ChangeFanSpeed {
			Header hdr;
			uint8_t speed;
		} change_fan_speed;
		struct ProtoMsg {
			Header hdr;
			uint8_t message [256];
			uint8_t len;
		} proto_msg;
	};

	MailBox<Events, 3> events_queue_;

	void put_event(Events ev);
private:
	static void BLExtractor_Thread (void const *argument);
	void Thread (void);

	osThreadId thread_ID = {0};
	osThreadDef(BLExtractor_Thread, osPriorityNormal, 1024);

	static void BLExtractor_Timer (void const *argument);
	void Timer (void);
	osTimerDef(timer0_handle, BLExtractor_Timer);

	SI7021 si7021_;

	uint8_t curr_fan_speed_ = 0;
	float humidity_ = 0;
	float temperature_ = 0;
	float humidity_level_on_ = 60;
	float humidity_level_off_ = 40;

	void on_change_fan_speed(Events::ChangeFanSpeed*);
	void on_proto_message(Events::ProtoMsg*);
	void on_read_measurements();

public:
	BLExtractor();
	virtual ~BLExtractor();

	void change_fan_speed(uint8_t speed) {
		Events ev;
		ev.hdr.msgid = kChangeFanSpeed;
		ev.change_fan_speed.speed = speed;
		events_queue_.push(ev);
	};
	void change_fan_speedI(uint8_t speed) {
		Events ev;
		ev.hdr.msgid = kChangeFanSpeed;
		ev.change_fan_speed.speed = speed;
		events_queue_.pushI(ev);
	};
	void proto_message();
	void read_measurements();

};

#endif /* BLEXTRACTOR_H_ */
