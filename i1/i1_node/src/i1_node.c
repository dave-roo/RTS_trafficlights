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

#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <fcntl.h>
#include <share.h>
#include <semaphore.h>


#include <global_include.h>
#include <x1_include.h>
#include <i1_include.h>

#define BUF_SIZE 100


#include "messageController.h"
#define LOCAL_ATTACH_POINT "i1_group_14"







//#include "I1DataTypes.h"

intersection_message *message;



//TRANSITION LOGIC
//===================================================================================================
void SingleStep_Transition(state_data *td,intersection_message *msg){
	//printf("test\n");
	//printf("test\n");
	//printf("test\n");

	//state_data *td=(state_data*)data;
	//intersection_message *msg=(intersection_message*)msg_;





	sem_wait(&td->sem);

	//printf("i1 train status: %d\n",msg->train_status);


  	switch(td->state){
  		case i1_State_0:

  			if(msg->train_status == 1)td->state = i1_State_11;
  			else if(msg->NE == 0 && msg->SW == 0)td->state = i1_State_1;
  			else if(msg->NE == 0 && msg->SW == 1)td->state = i1_State_9;
  			else if(msg->NE == 1 && msg->SW == 0)td->state = i1_State_5;
  			else if(msg->NE == 1 && msg->SW == 1)td->state = i1_State_7;
  			else td->state = i1_State_1;
  			break;
  		case i1_State_1:
			td->state = i1_State_2;
			break;
  		case i1_State_2:
			td->state = i1_State_11;
			break;
  		case i1_State_3:


  			//printf("i1 3 train status: %d\n",msg->train_status);
  			if(td->lock_status == 1){
  				td->state = i1_State_3;

  				updateControllerLocked(td);
  			}
  			else if(msg->train_status == 1){td->state = i1_State_3;}
  			else if(msg->train_status == 0){td->state = i1_State_4;}
  			else {td->state = i1_State_4;}

			break;

  		case i1_State_4:
			td->state = i1_State_0;
			break;

  		case i1_State_5:
  			if(msg->train_status == 1)td->state = i1_State_18;
  			else td->state = i1_State_6;

  			break;
  		case i1_State_6:
  			if(msg->train_status == 1)td->state = i1_State_18;
  			else td->state = i1_State_1;
  			break;
  		case i1_State_7:
  			td->state = i1_State_8;
  			break;
  		case i1_State_8:
  			if(msg->train_status == 1)td->state = i1_State_11;
  			else td->state = i1_State_1;
  			break;
  		case i1_State_9:
  			if(msg->train_status == 1)td->state = i1_State_19;
  			else td->state = i1_State_10;
  			break;
  		case i1_State_10:
  			if(msg->train_status == 1)td->state = i1_State_19;
  			else td->state = i1_State_1;
  			break;



  		case i1_State_11:
  			if(td->lock_status == 1)td->state = i1_State_3;
  			else if(msg->train_status == 1)td->state = i1_State_3;
  			else if(msg->ES == 0 && msg->WN == 0)td->state = i1_State_3;
  			else if(msg->ES == 0 && msg->WN == 1)td->state = i1_State_16;
  			else if(msg->ES == 1 && msg->WN == 0)td->state = i1_State_12;
  			else if(msg->ES == 1 && msg->WN == 1)td->state = i1_State_14;
  			else td->state = i1_State_3;
  			break;
  		case i1_State_12:
  			td->state = i1_State_13;
  			break;
  		case i1_State_13:
  			td->state = i1_State_3;
  			break;
  		case i1_State_14:
  			td->state = i1_State_15;
  			break;
  		case i1_State_15:
  			td->state = i1_State_3;
  			break;
  		case i1_State_16:
  			td->state = i1_State_17;
  			break;
  		case i1_State_17:
  			td->state = i1_State_3;
  			break;


  		case i1_State_18:
  			td->state = i1_State_11;
  			break;
  		case i1_State_19:
  			td->state = i1_State_11;
  			break;
  		default:
  			break;

  	}
  	if(td->state != i1_State_11){
  		if(td->state != i1_State_3){
  			if(td->lock_status == 1){
  				td->state = i1_State_11;
  			}
  		}
  	}
  	updateController(td);
  	sem_post(&td->sem);
  }
void Print_State(state_data *td){
	//state_data *td=(state_data*)data;
	sem_wait(&td->sem);

  	switch(td->state){
  		case i1_State_0:
  			printf("i1_State_0\n");
  			break;
  		case i1_State_1:
  			printf("i1_State_1\n");
			break;
  		case i1_State_2:
  			printf("i1_State_2\n");
			break;
  		case i1_State_3:
  			printf("i1_State_3\n");
			break;
  		case i1_State_4:
  			printf("i1_State_4\n");
			break;
  		case i1_State_5:
			printf("i1_State_5\n");
			break;
		case i1_State_6:
			printf("i1_State_6\n");
			break;
		case i1_State_7:
			printf("i1_State_7\n");
			break;
		case i1_State_8:
			printf("i1_State_8\n");
			break;
		case i1_State_9:
			printf("i1_State_9\n");
			break;
		case i1_State_10:
			printf("i1_State_10\n");
			break;
		case i1_State_11:
			printf("i1_State_11\n");
			break;
		case i1_State_12:
			printf("i1_State_12\n");
			break;
		case i1_State_13:
			printf("i1_State_13\n");
			break;
		case i1_State_14:
			printf("i1_State_14\n");
			break;
		case i1_State_15:
			printf("i1_State_15\n");
			break;
		case i1_State_16:
			printf("i1_State_16\n");
			break;
		case i1_State_17:
			printf("i1_State_17\n");
			break;
		case i1_State_18:
			printf("i1_State_18\n");
			break;
		case i1_State_19:
			printf("i1_State_19\n");
			break;
		default:
			break;

  	}
  	sem_post(&td->sem);
  }
//LIGHTS
//===================================================================================================
    void *thread_states (void *data)
    {
    	state_data *td=(state_data*)data;
    	while(1){

    		message = get_All_Sensor();
    		SingleStep_Transition(td,message);
    		Print_State(td);
    		sleep(1);
    	}
    	pthread_exit(EXIT_SUCCESS);
    }
    void* i1_message_server_thread(void *data){

    	state_data *td = (state_data*) data;

    	name_attach_t *attach;
    	if ((attach = name_attach(NULL, LOCAL_ATTACH_POINT, 0)) == NULL){
    		printf("\nFailed to name_attach on LOCAL_ATTACH_POINT: %s \n", LOCAL_ATTACH_POINT);
    		printf("\n Possibly another server with the same name is already running !\n");
    		return 0;
    	}
    	printf("Server Listening for Clients on LOCAL_ATTACH_POINT: %s \n", LOCAL_ATTACH_POINT);

    	// Setup the sending and receiving message structs
    	message_data_t msg;
    	message_data_t msg_reply;
    	msg_reply.hdr.type = 0x01;
    	msg_reply.hdr.subtype = 0x00;


    	int rcvid=0;
    	while (1){
    		//pthread_mutex_lock(&td->mutex);
    		// Receive any message
    		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

    		// Message receive failed
    		if (rcvid == -1){
    			printf("\nFailed to MsgReceive\n");
    			break;
    		}

    		// Determimsg->NE the pulse code
    		if(rcvid == 0){
    			switch(msg.hdr.code){
    				case _PULSE_CODE_DISCONNECT:
    					printf("_PULSE_CODE_DISCONNECT\n");
    					break;
    				case _PULSE_CODE_UNBLOCK:
    					printf("_PULSE_CODE_UNBLOCK\n");
    					break;
    				case _PULSE_CODE_COIDDEATH:
    					printf("_PULSE_CODE_COIDDEATH\n");
    					break;
    				case _PULSE_CODE_THREADDEATH:
    					printf("_PULSE_CODE_THREADDEATH\n");
    					break;
    				default:
    					printf("Something else received\n");
    					break;
    			}
    		continue;
    		}
    		if(rcvid > 0){
    			printf("Message Received %d\n", msg.msg_type);
    			msg_reply.sending_node = NODE_I1;
    			msg_reply.receiving_node = msg.sending_node;
    			msg_reply.msg_type = msg.msg_type;
    			switch(msg.msg_type){
    				case MSG_CURRENT_STATE_REQUEST:
    					printf("Current State Request\n");

    					msg_reply.data = td->state;
    					break;
    				case MSG_CONTROL_STATE_LOCK:
    					printf("Locking State\n");
    					//pthread_mutex_lock(&td->mutex);
    					sem_wait(&td->sem);
    					td->lock_status = 1;
    					//pthread_mutex_unlock(&td->mutex);
    					sem_post(&td->sem);
    					printf("here-1\n");
    					break;
    				case MSG_CONTROL_STATE_RELEASE:
    					printf("Releasing State\n");
    					sem_wait(&td->sem);
    					td->lock_status = 0;
    					sem_post(&td->sem);
    					break;
    				default:
    					printf("Unknown Message Type\n");
    					break;
    			}

    			MsgReply(rcvid, EOK, &msg_reply, sizeof(msg_reply));
    		}
    		else{
    			printf("\nERROR: Server received something, but could not handle it correctly\n");
    		}
    		//pthread_mutex_unlock(&td->mutex);
    	}
    	printf("here-3\n");
    	// Remove the attach point name from the file system (i.e. /dev/name/local/<myname>)
    	name_detach(attach, 0);
    	return EXIT_SUCCESS;
    }
//MAIN
//===================================================================================================

int main(int argc, char *argv[]) {


	pthread_t  th1, msg_server;

	state_data td = {i1_State_0, 0};
	sem_init(&td.sem, 0, 1);
	pthread_mutex_init(&td.mutex,NULL);
	pthread_create (&th1, NULL, thread_states, &td);
	pthread_create (&msg_server, NULL, i1_message_server_thread, &td);
	pthread_exit(EXIT_SUCCESS);

	while(1){

	}

	return EXIT_SUCCESS;


}




