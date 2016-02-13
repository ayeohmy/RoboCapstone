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

#ifndef INT32U
#define INT32U unsigned long
#endif

#ifndef INT8U
#define INT8U byte
#endif

#include "Arduino.h"
#include <mcp_can.h>
 
class SquirtCanLib
{
	public:
		//CAN message header numbers
		static const INT32U CAN_MSG_HDR_DRINK_ORDER = 0; 
		static const INT32U CAN_MSG_HDR_READY_TO_MOVE = 1;
		static const INT32U CAN_MSG_HDR_RUNNING = 2;
		static const INT32U CAN_MSG_HDR_UI_HEALTH = 3;
		static const INT32U CAN_MSG_HDR_SERVING_STATUS = 4;
		static const INT32U CAN_MSG_HDR_SERVING_HEALTH = 5;
		static const INT32U CAN_MSG_HDR_STOCK_STATUS = 6; 
		static const INT32U CAN_MSG_HDR_PREP_STATUS = 7;
		static const INT32U CAN_MSG_HDR_PREP_HEALTH = 8;
		static const INT32U CAN_MSG_HDR_MOVING = 9;
		static const INT32U CAN_MSG_HDR_AT_ROW = 10;
		static const INT32U CAN_MSG_HDR_DRIVE_HEALTH = 11; 

		static const INT32U ERROR_STUCK = 1;
		static const INT32U ERROR_NOCUPS = 2;
		static const INT32U ERROR_NODRINKS = 3;

		/*
		 * Called to initialize the SPI communications to the CAN chip
		 * Args:
		 *   INT8U slaveSelectPin : pin number of the slave-select SPI line
		 *     to the CAN chip
		 * Return:
		 *   void
		 */
		void canSetup(INT8U slaveSelectPin); 

		/*
		 * Called to stage a message to be sent over the CAN network
		 * This call only blocks until the message is staged for transmission on the
		 * CAN chip, and may return before the message is actually sent.
		 * Args:
		 *   INT32U msgHdr : number to use as the CAN message id (must use value from 
		 *     the #defines in squirtlib.h)
		 *   INT8U payload : the 8-bit value to be transmitted as the message payload
		 * Return:
		 *   INT8U :
		 *   0 : message successfully staged for transmission
		 *   1 : unable to stage as there is an unsent message already staged
		 *   2 : msgHdr is not a valid header as defined in squirtlib.h
		 */
		INT8U sendMsg(INT32U msgHdr, INT8U payload); 

		/*
		 * Gets the most recent payload transmitted over the network with the specified
		 * message header. This function will NOT return any messages originating from
		 * the CAN chip this call is being made to.
		 * Args:
		 *   INT32U msgHdr : CAN message id of the message type to read (must use value
		 *     from the #defines in squirtlib.h)
		 * Return:
		 *   INT8U : 8 bits representing the payload sent in the most recently
		 *     received CAN message with the specified CAN message id.
		 *   0x00 : if no message with the specified CAN message id has been received
		 */
		INT8U getMsg(INT32U msgHdr);
		
		/*
		 * Function called on interrupt when the general interrupt pin is low,
		 * indicating that the CAN chip has received a new message. This 
		 * function will read in the message over SPI and store it in the 
		 * appropriate mailbox.
		 * 
		 * User must implement a wrapper function which calls receivedMsg and
		 * then call attachInterrupt with parameters of the wrapper function 
		 * and the pin connected to the CAN chip's general interrupt pin.
		 */
		void receivedMsg();
		
	private:
		/*
		 * Variables representing the local can mailboxes
		 */
		volatile INT8U mailboxDrinkOrder = 0; 
		volatile INT8U mailboxReadyToMove = 0;
		volatile INT8U mailboxRunning = 0;
		volatile INT8U mailboxUiHealth = 0;
		volatile INT8U mailboxServingStatus = 0;
		volatile INT8U mailboxServingHealth = 0;
		volatile INT8U mailboxStockStatus = 0; 
		volatile INT8U mailboxPrepStatus = 0;
		volatile INT8U mailboxPrepHealth = 0;
		volatile INT8U mailboxMoving = 0;
		volatile INT8U mailboxAtRow = 0;
		volatile INT8U mailboxDriveHealth = 0;
};

#endif
 