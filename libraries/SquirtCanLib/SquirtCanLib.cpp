//squirtlib.c: the functions for everyone 

/*
 * SquirtCanLib.cpp - Library used to send Project Squirt messages over a CAN 
 * network using MCP2515 CAN chips connected over SPI to Arduino boards. This
 * library acts as a wrapper for the MCP_CAN Library for Arduino by coryjflower
 * (url: https://github.com/coryjfowler/MCP_CAN_lib)
 *
 * Created by Zachary Dawson 
 * Team <1% Robot
 * for 16-474 Project Squirt
 */

#include "Arduino.h"
#include "SquirtCanLib.h"
#include <mcp_can.h>

#define CAN_BAUD CAN_250KBPS

MCP_CAN *can; //Pointer to the can object

void SquirtCanLib::canSetup(INT8U slaveSelectPin) {
	*can = MCP_CAN(slaveSelectPin);
	can->begin(CAN_BAUD);
}

INT8U SquirtCanLib::sendMsg(INT32U msgHdr, INT8U payload) {
	return can->sendMsgBuf(msgHdr, 0, 1, &payload);
}

INT8U SquirtCanLib::getMsg(INT32U msgHdr) {
	switch(msgHdr) {
		case CAN_MSG_HDR_DRINK_ORDER:
			return mailboxDrinkOrder;
			break;
		case CAN_MSG_HDR_READY_TO_MOVE:
			return mailboxReadyToMove;
			break;
		case CAN_MSG_HDR_RUNNING:
			return mailboxRunning;
			break;
		case CAN_MSG_HDR_UI_HEALTH:
			return mailboxUiHealth;
			break;
		case CAN_MSG_HDR_SERVING_STATUS:
			return mailboxServingStatus;
			break;
		case CAN_MSG_HDR_SERVING_HEALTH:
			return mailboxServingHealth;
			break;
		case CAN_MSG_HDR_STOCK_STATUS:
			return mailboxStockStatus;
			break;
		case CAN_MSG_HDR_PREP_STATUS:
			return mailboxPrepStatus;
			break;
		case CAN_MSG_HDR_PREP_HEALTH:
			return mailboxPrepHealth;
			break;
		case CAN_MSG_HDR_MOVING:
			return mailboxMoving;
			break;
		case CAN_MSG_HDR_AT_ROW:
			return mailboxAtRow;
			break;
		case CAN_MSG_HDR_DRIVE_HEALTH:
			return mailboxDriveHealth;
			break;
		default:
			return 0x00;
			break;
	}
}

void SquirtCanLib::receivedMsg() {
	INT8U payload;
	INT8U len = 1;
	can->readMsgBuf(&len, &payload); // Read data: len = data length, buf = data byte(s)
	INT32U msgHdr = can->getCanId();
	switch(msgHdr) {
		case CAN_MSG_HDR_DRINK_ORDER:
			mailboxDrinkOrder = payload;
			break;
		case CAN_MSG_HDR_READY_TO_MOVE:
			mailboxReadyToMove = payload;
			break;
		case CAN_MSG_HDR_RUNNING:
			mailboxRunning = payload;
			break;
		case CAN_MSG_HDR_UI_HEALTH:
			mailboxUiHealth = payload;
			break;
		case CAN_MSG_HDR_SERVING_STATUS:
			mailboxServingStatus = payload;
			break;
		case CAN_MSG_HDR_SERVING_HEALTH:
			mailboxServingHealth = payload;
			break;
		case CAN_MSG_HDR_STOCK_STATUS:
			mailboxStockStatus = payload;
			break;
		case CAN_MSG_HDR_PREP_STATUS:
			mailboxPrepStatus = payload;
			break;
		case CAN_MSG_HDR_PREP_HEALTH:
			mailboxPrepHealth = payload;
			break;
		case CAN_MSG_HDR_MOVING:
			mailboxMoving = payload;
			break;
		case CAN_MSG_HDR_AT_ROW:
			mailboxAtRow = payload;
			break;
		case CAN_MSG_HDR_DRIVE_HEALTH:
			mailboxDriveHealth = payload;
			break;
		default:
			break;
	}
}
 