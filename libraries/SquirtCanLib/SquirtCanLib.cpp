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
#include <avr/interrupt.h>

#define CAN_BAUD CAN_500KBPS

//MCP_CAN can(48); //Pointer to the can object
MCP_CAN can(53); //Pointer to the can object


void SquirtCanLib::canSetup(INT8U slaveSelectPin) {
	//*can = MCP_CAN(slaveSelectPin);
	can.begin(CAN_BAUD);
}

INT8U SquirtCanLib::sendMsg(INT32U msgHdr, INT8U payload) {
	return can.sendMsgBuf(msgHdr, 0, 1, &payload);
}

INT8U SquirtCanLib::getMsg(INT32U msgHdr) {
	cli();
	INT8U result;
	switch(msgHdr) {
		case CAN_MSG_HDR_DRINK_ORDER:
			result =  mailboxDrinkOrder;
			break;
		case CAN_MSG_HDR_READY_TO_MOVE:
			result =  mailboxReadyToMove;
			break;
		case CAN_MSG_HDR_RUNNING:
			result =  mailboxRunning;
			break;
		case CAN_MSG_HDR_UI_HEALTH:
			result =  mailboxUiHealth;
			break;
		case CAN_MSG_HDR_SERVING_STATUS:
			result =  mailboxServingStatus;
			break;
		case CAN_MSG_HDR_SERVING_HEALTH:
			result =  mailboxServingHealth;
			break;
		case CAN_MSG_HDR_STOCK_STATUS:
			result =  mailboxStockStatus;
			break;
		case CAN_MSG_HDR_PREP_STATUS:
			result =  mailboxPrepStatus;
			break;
		case CAN_MSG_HDR_PREP_HEALTH:
			result =  mailboxPrepHealth;
			break;
		case CAN_MSG_HDR_MOVING:
			result =  mailboxMoving;
			break;
		case CAN_MSG_HDR_AT_ROW:
			result =  mailboxAtRow;
			break;
		case CAN_MSG_HDR_DRIVE_HEALTH:
			result =  mailboxDriveHealth;
			break;
		default:
			result =  0x00;
			break;
	}
	sei();
	return result;
}

void SquirtCanLib::receivedMsg() {
	INT8U payload[1];
	INT8U len = 1;
	can.readMsgBuf(&len, payload); // Read data: len = data length, buf = data byte(s)
	INT32U msgHdr = can.getCanId();
	switch(msgHdr) {
		case CAN_MSG_HDR_DRINK_ORDER:
			mailboxDrinkOrder = payload[0];
			break;
		case CAN_MSG_HDR_READY_TO_MOVE:
			mailboxReadyToMove = payload[0];
			break;
		case CAN_MSG_HDR_RUNNING:
			mailboxRunning = payload[0];
			break;
		case CAN_MSG_HDR_UI_HEALTH:
			mailboxUiHealth = payload[0];
			break;
		case CAN_MSG_HDR_SERVING_STATUS:
			mailboxServingStatus = payload[0];
			break;
		case CAN_MSG_HDR_SERVING_HEALTH:
			mailboxServingHealth = payload[0];
			break;
		case CAN_MSG_HDR_STOCK_STATUS:
			mailboxStockStatus = payload[0];
			break;
		case CAN_MSG_HDR_PREP_STATUS:
			mailboxPrepStatus = payload[0];
			break;
		case CAN_MSG_HDR_PREP_HEALTH:
			mailboxPrepHealth = payload[0];
			break;
		case CAN_MSG_HDR_MOVING:
			mailboxMoving = payload[0];
			break;
		case CAN_MSG_HDR_AT_ROW:
			mailboxAtRow = payload[0];
			break;
		case CAN_MSG_HDR_DRIVE_HEALTH:
			mailboxDriveHealth = payload[0];
			break;
		default:
			break;
	}
}
 