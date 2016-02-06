//it's sharelib! 
//lovely shared information for all of the subsystem controllers. 

/*
 * SquirtCanLib.h - Library used to send Project Squirt messages over a CAN 
 * network using MCP2515 CAN chips connected over SPI to Arduino boards. This
 * library acts as a wrapper for the MCP_CAN Library for Arduino by coryjflower
 * (url: https://github.com/coryjfowler/MCP_CAN_lib)
 *
 * Created by Zachary Dawson 
 * Team <1% Robot
 * for 16-474 Project Squirt
 */

//Define the library
#ifndef SquirtCanLib_h
#define SquirtCanLib_h

#include "Arduino.h"
 
class SquirtCanLib
{
	public:
		//CAN message header numbers
		static const int CAN_MSG_HDR_DRINK_ORDER = 0; 
		static const int CAN_MSG_HDR_READY_TO_MOVE = 1;
		static const int CAN_MSG_HDR_RUNNING = 2;
		static const int CAN_MSG_HDR_UI_HEALTH = 3;
		static const int CAN_MSG_HDR_SERVING_STATUS = 4;
		static const int CAN_MSG_HDR_SERVING_HEALTH = 5;
		static const int CAN_MSG_HDR_STOCK_STATUS = 6; 
		static const int CAN_MSG_HDR_PREP_STATUS = 7;
		static const int CAN_MSG_HDR_PREP_HEALTH = 8;
		static const int CAN_MSG_HDR_MOVING = 9;
		static const int CAN_MSG_HDR_AT_ROW = 10;
		static const int CAN_MSG_HDR_DRIVE_HEALTH = 11; 

		static const int ERROR_STUCK = 1;
		static const int ERROR_NOCUPS = 2;
		static const int ERROR_NODRINKS = 3;

		/*
		 * Called to initialize the SPI communications to the CAN chip
		 * Args:
		 *   int spiPins[] : pins for the given board to use for SPI
		 * Return:
		 *   void
		 */
		void canSetup(int spiPins[]); 

		/*
		 * Called to stage a message to be sent over the CAN network
		 * This call only blocks until the message is staged for transmission on the
		 * CAN chip, and may return before the message is actually sent.
		 * Args:
		 *   int msgHdr : number to use as the CAN message id (must use value from 
		 *     the #defines in squirtlib.h)
		 *   char payload : the 8-bit value to be transmitted as the message payload
		 * Return:
		 *   int :
		 *   0 : message successfully staged for transmission
		 *   1 : unable to stage as there is an unsent message already staged
		 *   2 : msgHdr is not a valid header as defined in squirtlib.h
		 */
		int sendMsg(int msgHdr, char payload); 

		/*
		 * Gets the most recent payload transmitted over the network with the specified
		 * message header.
		 * Args:
		 *   int msgHdr : CAN message id of the message type to read (must use value
		 *     from the #defines in squirtlib.h)
		 * Return:
		 *   unsigned char : 8 bits representing the payload sent in the most recently
		 *     received CAN message with the specified CAN message id.
		 *   0x00 : if no message with the specified CAN message id has been received
		 */
		unsigned char getMsg(int msgHdr);
};

#endif
 