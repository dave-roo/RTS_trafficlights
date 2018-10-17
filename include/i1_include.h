#ifndef _I1_INCLUDE_H
#define _I1_INCLUDE_H

typedef enum {
	i1_State_0,
	i1_State_1,
	i1_State_2,
	i1_State_3,
	i1_State_4,
	i1_State_5,
	i1_State_6,
	i1_State_7,
	i1_State_8,
	i1_State_9,
	i1_State_10,
	i1_State_11,
	i1_State_12,
	i1_State_13,
	i1_State_14,
	i1_State_15,
	i1_State_16,
	i1_State_17,
	i1_State_18,
	i1_State_19
} i1_states;
typedef struct
{
	 int train_status;
	 int NE;
	 int SW;
	 int ES;
	 int WN;

	 int NS;
	 int EW;
	 pthread_mutex_t mutex;
} intersection_message;

typedef struct
{
	i1_states state;
	int lock_status;
	uint8_t peak_status;
	intersection_message msg;
	pthread_mutex_t mutex;
	sem_t sem;
} state_data;
enum lights {
	GREEN_STRT = 1,
	GREEN_TURN = 1,
	YELLOW = 1,
	RED = 1
};
#endif
