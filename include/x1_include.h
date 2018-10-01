#ifndef _X1_INCLUDE_H_
#define _X1_INCLUDE_H_
#include <semaphore.h>

// States for railway crossing
typedef enum {
	X1_STATE_0 =0,
	X1_STATE_1,
	X1_STATE_2,
	X1_STATE_3,
	X1_STATE_4,
	X1_STATE_5,
	X1_STATE_6,
	X1_STATE_7
} x1_states ;

typedef enum{
	X1_SIGNAL_NONE = 0,
	X1_SIGNAL_Ein,
	X1_SIGNAL_Win,
	X1_SIGNAL_Eout,
	X1_SIGNAL_Wout,
	X1_SIGNAL_BoomGateDown
} x1_signals ;

// Struct to pass global data
typedef struct{
	x1_states current_state;
	uint8_t sensor_received;
	uint8_t signal;
	sem_t sem;
} sm_data_t;


# endif
