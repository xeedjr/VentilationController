/*
 * MBComunication.cpp
 *
 *  Created on: 14 ���. 2016 �.
 *      Author: Bogdan
 */
#include <string.h>
#include <memory>
#include "MBComunication.h"
#include "mbframe.h"

extern std::unique_ptr<BLExtractor> bl;

MBComunication::MBComunication() {
}

MBComunication::~MBComunication() {
	// TODO Auto-generated destructor stub
}

void MBComunication::send_notification_temp_humidity(float hum,
														float temp,
														uint32_t speed) {

}

void MBComunication::send_resp_OK(uint8_t cmd_tag) {

}

void MBComunication::put_message_for_send(MBMessage& message) {

}

eMBException
MBComunication::eMBFuncPacket( UCHAR * pucFrame, USHORT * usLen ) {
    //memcpy( &pucFrame[MB_PDU_DATA_OFF], &ucMBSlaveID[0], ( size_t )usMBSlaveIDLen );
    //*usLen = ( USHORT )( MB_PDU_DATA_OFF + usMBSlaveIDLen );

	/// copy input message
	if (*usLen > MB_PDU_DATA_OFF) {
/*		ev.ev_type = BLExtractor::Events::kProtoMsg;
		memcpy(ev.events.proto_msg.message,
				&pucFrame[MB_PDU_DATA_OFF],
				*usLen - MB_PDU_DATA_OFF);
		ev.events.proto_msg.len = (uint8_t)(*usLen - MB_PDU_DATA_OFF);
		bl->put_event(ev);
*/
	};

	/// fill out packet if exist
	if (out_message_len_ != 0) {
		memcpy(&pucFrame[MB_PDU_DATA_OFF], out_messages_, out_message_len_);
		*usLen = ( USHORT )( MB_PDU_DATA_OFF + out_message_len_ );
		out_message_len_ = 0;
	};

    return MB_EX_NONE;
}
