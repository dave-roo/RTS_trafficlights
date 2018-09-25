#ifndef _DATA_TYPE_H
#define _DATA_TYPE_H

// Message types for data between nodes
typedef enum{
    TRAIN_SIGNAL = 0,
    CURRENT_STATE,
    CONTROL_STATE_LOCK,
    CONTROL_STATE_RELEASE
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

// State machine enum eg for data 
enum states{
    STATE_0,
    STATE_1,
    STATE_2, 
    STATE_3,
    STATE_4,
    // eg ...
};

// eg to create struct for NS green light
void request_x1_ns_green(){
    message_data_t msg_tx;
    msg_tx.sending_node = NODE_CONTROLLER;
    msg_tx.msg_type = CONTROL_STATE_LOCK;
    msg_tx.data = STATE_2;
    // send message
}

// possible function call to release a state
void release_x1(){
    message_data_t msg_tx;
    msg_tx.sending_node = NODE_CONTROLLER;
    msg_tx.msg_type = CONTROL_STATE_RELEASE;
    msg_tx.data = NULL;
    // send message
}

#endif