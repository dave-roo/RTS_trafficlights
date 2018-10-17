#ifndef _I1DATATYPE_H_
#define _I1DATATYPE_H_

enum states {
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
};



typedef struct
{
	enum states state;
	pthread_mutex_t mutex;
} state_data;



#endif
