#ifndef _X1_INCLUDE_H_
#define _X1_INCLUDE_H_
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

// States for railway crossing
typedef enum{
	X1_STATE_0 =0,
	X1_STATE_1,
	X1_STATE_2,
	X1_STATE_3,
	X1_STATE_4,
	X1_STATE_5,
	X1_STATE_6,
	X1_STATE_7
} x1_states ;

// Signal Types for X1
typedef enum{
	X1_SIGNAL_NONE = 0,
	X1_SIGNAL_Ein,
	X1_SIGNAL_Win,
	X1_SIGNAL_Eout,
	X1_SIGNAL_Wout,
	X1_SIGNAL_BoomGateDown
} x1_signals ;

// Priority struct
struct sm_priority_t{
	x1_states last_state; // This is used for priority traffic
	uint8_t set;
	uint8_t x1_reset; // This is used as a conditional variable for the state machine
};

// Struct for global data
typedef struct{
	x1_states current_state;
	uint8_t sensor_received;
	uint8_t signal;
	sem_t sem;
	struct sm_priority_t priority;
} sm_data_t;

# endif
