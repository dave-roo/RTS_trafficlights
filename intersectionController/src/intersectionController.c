/*
 * thread_ex2.c
 *
 * Simple example of creating multiple threads.
 *
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <mqueue.h>
#include <sys/stat.h>

#include <errno.h>
#include <sys/iofunc.h>

#include <sys/netmgr.h>
#include <sync.h>
#include <sys/neutrino.h>
#define BUF_SIZE 100


#include "messageController.h"


enum states {
	State_0,
	State_1,
	State_2,
	State_3,
	State_4,
	State_5,
	State_6,
	State_7,
	State_8,
	State_9,
	State_10,
	State_11,
	State_12,
	State_13,
	State_14,
	State_15,
	State_16,
	State_17,
	State_18,
	State_19

};



typedef struct
{
	enum states state;
	pthread_mutex_t mutex;
} state_data;


intersection_message *message;



//TRANSITION LOGIC
//===================================================================================================
void SingleStep_Transition(enum states *CurState, int ne, int sw, int es, int wn, int ts){
  	switch(*CurState){
  		case State_0:
  			if(ts == 1)*CurState = State_11;
			if(ne == 0 && sw == 0)*CurState = State_1;
			if(ne == 0 && sw == 1)*CurState = State_9;
			if(ne == 1 && sw == 0)*CurState = State_5;
			if(ne == 1 && sw == 1)*CurState = State_7;
  			break;
  		case State_1:
			*CurState = State_2;
			break;
  		case State_2:
			*CurState = State_11;
			break;
  		case State_3:
  			if(ts == 0)*CurState = State_4;
  			if(ts == 1)*CurState = State_3;
			break;

  		case State_4:
			*CurState = State_0;
			break;

  		case State_5:
  			if(ts == 0)*CurState = State_6;
  			if(ts == 1)*CurState = State_18;
  			break;
  		case State_6:
  			if(ts == 0)*CurState = State_1;
  			if(ts == 1)*CurState = State_18;
  			break;
  		case State_7:
  			*CurState = State_8;
  			break;
  		case State_8:
  			if(ts == 0)*CurState = State_1;
  			if(ts == 1)*CurState = State_11;
  			break;
  		case State_9:
  			if(ts == 0)*CurState = State_10;
  			if(ts == 1)*CurState = State_19;
  			break;
  		case State_10:
  			if(ts == 0)*CurState = State_1;
  			if(ts == 1)*CurState = State_19;
  			break;




  		case State_11:
  			if(ts == 1)*CurState = State_3;
  			if(es == 0 && wn == 0)*CurState = State_3;
  			if(es == 0 && wn == 1)*CurState = State_16;
  			if(es == 1 && wn == 0)*CurState = State_12;
  			if(es == 1 && wn == 1)*CurState = State_14;
  			break;
  		case State_12:
  			*CurState = State_13;
  			break;
  		case State_13:
  			*CurState = State_3;
  			break;
  		case State_14:
  			*CurState = State_15;
  			break;
  		case State_15:
  			*CurState = State_3;
  			break;
  		case State_16:
  			*CurState = State_17;
  			break;
  		case State_17:
  			*CurState = State_3;
  			break;


  		case State_18:
  			*CurState = State_11;
  			break;
  		case State_19:
  			*CurState = State_11;
  			break;

  	}
  }
void Print_State(enum states *CurState){
  	switch(*CurState){
  		case State_0:
  			printf("State_0\n");
  			break;
  		case State_1:
  			printf("State_1\n");
			break;
  		case State_2:
  			printf("State_2\n");
			break;
  		case State_3:
  			printf("State_3\n");
			break;
  		case State_4:
  			printf("State_4\n");
			break;
  		case State_5:
			printf("State_5\n");
			break;
		case State_6:
			printf("State_6\n");
			break;
		case State_7:
			printf("State_7\n");
			break;
		case State_8:
			printf("State_8\n");
			break;
		case State_9:
			printf("State_9\n");
			break;
		case State_10:
			printf("State_10\n");
			break;
		case State_11:
			printf("State_11\n");
			break;
		case State_12:
			printf("State_12\n");
			break;
		case State_13:
			printf("State_13\n");
			break;
		case State_14:
			printf("State_14\n");
			break;
		case State_15:
			printf("State_15\n");
			break;
		case State_16:
			printf("State_16\n");
			break;
		case State_17:
			printf("State_17\n");
			break;
		case State_18:
			printf("State_18\n");
			break;
		case State_19:
			printf("State_19\n");
			break;

  	}
  }
//LIGHTS
//===================================================================================================
    void *thread_states (void *data)
    {
    	state_data *td=(state_data*)data;
    	while(1){


    		/*
    		 * here we should have get_All_Sensor();
    		 */
    		//intersection_message m = {.train_status = 0, .NE = 1, .SW = 0, .ES = 0, .WN = 0}; // change this struct to test different messages - will need function to really get messages later
    		message = get_All_Sensor();

    		SingleStep_Transition(&td->state,message->NE,message->SW,message->ES,message->WN,message->train_status);
    		Print_State(&td->state);
    		sleep(2);
    	}
    	pthread_exit(EXIT_SUCCESS);
    }

//MAIN
//===================================================================================================

int main(int argc, char *argv[]) {
	pthread_t  th1;

	state_data td2 = {State_0};

	pthread_mutex_init(&td2.mutex,NULL);
	pthread_create (&th1, NULL, thread_states, &td2);
	pthread_exit(EXIT_SUCCESS);

	while(1){

	}

	return EXIT_SUCCESS;


}





