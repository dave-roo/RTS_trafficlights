// Header file for common data types

#ifndef _DATA_TYPE_H
#define _DATA_TYPE_H
#include <stdint.h>

// Message types for data between nodes
typedef enum{
    MSG_TRAIN_SIGNAL = 0,
	MSG_CURRENT_STATE,
	MSG_CONTROL_STATE_LOCK,
	MSG_CONTROL_STATE_RELEASE
    // Any others that need to be added here
} message_type;


typedef enum{
    NODE_I1 = 0,
    NODE_I2,
    NODE_X1,
    NODE_CONTROLLER
} node_id;

typedef struct{
    node_id sending_node;
    message_type msg_type; 
    uint8_t data;
} message_data_t;

#endif
