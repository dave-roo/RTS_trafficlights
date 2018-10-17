// Header file for common data types

#ifndef _DATA_TYPE_H
#define _DATA_TYPE_H
#include <stdint.h>

// Message types for data between nodes
typedef enum{
    MSG_TRAIN_SIGNAL = 0,
	MSG_CURRENT_STATE_REQUEST, 	// Node requesting state from other node
	MSG_CURRENT_STATE_UPDATE,	// Node updating controller when state change
	MSG_CONTROL_STATE_LOCK,		// Controller Requesting state lock
	MSG_CONTROL_STATE_RELEASE,	// Controller releasing priority lock
	MSG_ERROR					// msg error
    // Any others that need to be added here
} message_type;


typedef enum{
    NODE_I1 = 0,
    NODE_I2,
    NODE_X1,
    NODE_CONTROLLER
} node_id;

typedef struct{
	struct _pulse hdr;
    node_id sending_node;
    node_id receiving_node;
    message_type msg_type;
    uint8_t data;
} message_data_t;

#define X1_QNET_ATTACH_POINT  "/net/vm2/dev/name/local/x1_group_14" // CHANGE TO MATCH HOSTNAME OF X1
#define CONTROLLER_QNET_ATTACH_POINT  "/net/vm1/dev/name/local/c1_group_14" // CHANGE TO MATCH HOSTNAME OF CONTROLLER

#endif
