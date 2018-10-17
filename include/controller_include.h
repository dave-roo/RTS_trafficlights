#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_
#include "i1_include.h"
#include "x1_include.h"
#include <semaphore.h>

struct priority_struct{
	uint8_t signal_sent;
	uint8_t x1_ready;
	uint8_t i1_ready;
	uint8_t i2_ready;
};

// Struct to hold global struct for the controller
typedef struct{
    x1_states x1_current_state;
    uint8_t x1_connection_status;
    i1_states i1_current_state;
    uint8_t i1_connection_status;
    i1_states i2_current_state;
    uint8_t i2_connection_status;
    uint8_t updated;
    struct priority_struct priority;
    uint8_t peak;
    sem_t sem;
} controller_data_t;
#endif
