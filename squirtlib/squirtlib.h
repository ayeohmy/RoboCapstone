//it's sharelib! 
//lovely shared information for all of the subsystem controllers. 

//CAN message numbers
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

void canSetup(int spiPins[]); 
int sendMsg(int msgHdr, char payload); 
char getMsg(int msgHdr);

 