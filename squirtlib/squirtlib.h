//it's sharelib! 
//lovely shared information for all of the subsystem controllers. 

/*
 * Library used to send Project Squirt messages over a CAN network using
 * MCP2515 CAN chips connected over SPI to Arduino boards. This library acts
 * as a wrapper for the MCP_CAN Library for Arduino by coryjflower (url :
 * https://github.com/coryjfowler/MCP_CAN_lib)
 */

//CAN message header numbers
#define CAN_MSG_HDR_DRINK_ORDER 0 
#define CAN_MSG_HDR_READY_TO_MOVE 1
#define CAN_MSG_HDR_RUNNING 2
#define CAN_MSG_HDR_UI_HEALTH 3
#define CAN_MSG_HDR_SERVING_STATUS 4 
#define CAN_MSG_HDR_SERVING_HEALTH 5 
#define CAN_MSG_HDR_STOCK_STATUS 6 
#define CAN_MSG_HDR_PREP_STATUS 7 
#define CAN_MSG_HDR_PREP_HEALTH 8 
#define CAN_MSG_HDR_MOVING 9 
#define CAN_MSG_HDR_AT_ROW 10
#define CAN_MSG_HDR_DRIVE_HEALTH 11 

#define ERROR_STUCK 1 
#define ERROR_NOCUPS 2 
#define ERROR_NODRINKS 3

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

 