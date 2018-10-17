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

#include "stateController.h"
#include <stateStrings.h>
#include <global_include.h>
#include <x1_include.h>
#include <i1_include.h>
#include <hardware_include.h>

#define BUF_SIZE 100


#include "messageController.h"
#define LOCAL_ATTACH_POINT "i2_group_14"







//#include "I1DataTypes.h"

intersection_message *message;



//TRANSITION LOGIC
//===================================================================================================
void SingleStep_Transition(state_data *td){


	sem_wait(&td->sem);
	if(td->peak_status==1)switch_peak(td);
	else switch_off_peak(td);

	//switch_peak(td);
	updateController(td);
  	sem_post(&td->sem);
  }
void Print_State(state_data *td, lcd_data_t *lcd){

	sem_wait(&td->sem);
	printf("%s\n", toString(td));
	lcd_write_to_screen(*lcd, "Intersection 2", toString(td));
  	sem_post(&td->sem);
  }
//LIGHTS
//===================================================================================================
    void *state_machine (void *data)
    {
    	state_data *td=(state_data*)data;

    	//set lcd
    	lcd_data_t lcd;

    	lcd = lcd_setup();

		if(lcd.err != LCD_OK){
			printf("Failed to initialise LCD\n");
			return EXIT_FAILURE;
		}else{
			printf("LCD Setup complete\n");
		}


    	while(1){
    		//sem_wait(&td->sem);
    			td->msg.train_status = get_All_Sensor();
    		//sem_post(&td->sem);
    		SingleStep_Transition(td);
    		Print_State(td, &lcd);
    		//sleep(1);
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

    		// DetermitNE the pulse code
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
    					printf("====> Current State Request\n");
    					sem_wait(&td->sem);
    						msg_reply.data = td->state;
    					sem_post(&td->sem);
    					break;
    				case MSG_CONTROL_STATE_LOCK:
    					printf("====> Locking State\n");
    					sem_wait(&td->sem);
    						td->lock_status = 1;
    					sem_post(&td->sem);
    					break;
    				case MSG_CONTROL_STATE_RELEASE:
    					printf("====> Releasing State\n");
    					sem_wait(&td->sem);
    						td->lock_status = 0;
    					sem_post(&td->sem);
    					break;
    				case MSG_CONTROL_PEAK:
    					printf("====> Change Peak/Off-Peak\n");
						//sem_wait(&td->sem);
							td->peak_status = msg.data;
						//sem_post(&td->sem);
						break;
    				default:
    					printf("====> Unknown Message Type\n");
    					break;
    			}

    			MsgReply(rcvid, EOK, &msg_reply, sizeof(msg_reply));
    		}
    		else{
    			printf("\nERROR: Server received something, but could not handle it correctly\n");
    		}
    		//pthread_mutex_unlock(&td->mutex);
    	}
    	//printf("here-3\n");
    	// Remove the attach point name from the file system (i.e. /dev/name/local/<myname>)
    	name_detach(attach, 0);
    	return EXIT_SUCCESS;
    }




void* input_thread(void* data){

	state_data *td = (state_data*) data;

	keypad_data_t key;
	// Keypad Setup function
	key = keypad_setup();
	if(key.err != KEYPAD_OK){
		printf("Initialisation failed\n");
	}else{
		printf("Keypad Setup Complete!\n");
	}

	// Read the struct from the keypad_get_key function and determine if valid
	// could be improved but not enough time
	while(1){
		keypad_data_t data;
		data = keypad_get_key(key);
		if(data.valid){

			switch(data.key_pressed){
				case BUTTON_2:
					printf("====> SENSOR: West-to-North sensor\n");
					sem_wait(&td->sem);
						td->msg.WN = 1;
					sem_post(&td->sem);
					break;
				case BUTTON_5:
					printf("====> SENSOR: South-to-West\n", data.key_pressed);
					sem_wait(&td->sem);
						td->msg.SW = 1;
					sem_post(&td->sem);
					break;
				case BUTTON_7:
					printf("====> SENSOR: North-to-East\n", data.key_pressed);
					sem_wait(&td->sem);
						td->msg.NE = 1;
					sem_post(&td->sem);
					break;
				case BUTTON_10:
					printf("====> SENSOR: East-to-South\n", data.key_pressed);
					sem_wait(&td->sem);
						td->msg.ES = 1;
					sem_post(&td->sem);
					break;


				case BUTTON_13:
					printf("====> SENSOR: East & West\n", data.key_pressed);
					sem_wait(&td->sem);
						td->msg.EW = 1;
					sem_post(&td->sem);
					break;
				case BUTTON_16:
					printf("====> SENSOR: East & West\n", data.key_pressed);
					sem_wait(&td->sem);
						td->msg.EW = 1;
					sem_post(&td->sem);
					break;

				case BUTTON_4:
					printf("====> Change Peak/Off-Peak\n", data.key_pressed);
					sem_wait(&td->sem);
						td->peak_status = !td->peak_status;
					sem_post(&td->sem);
					break;
			}
		}

	}
}

//MAIN
//===================================================================================================

int main(int argc, char *argv[]) {


	pthread_t  state_machine_, i1_message_server_thread_, input_thread_;


	state_data td = {i1_State_0, 0, 0};
	sem_init(&td.sem, 0, 1);
	pthread_mutex_init(&td.mutex,NULL);
	pthread_create (&state_machine_, NULL, state_machine, &td);
	pthread_create (&i1_message_server_thread_, NULL, i1_message_server_thread, &td);

	pthread_create (&input_thread_, NULL, input_thread, &td);

	pthread_join(state_machine_, NULL);
	pthread_join(i1_message_server_thread_, NULL);
	pthread_join(input_thread_, NULL);

	while(1){

	}

	return EXIT_SUCCESS;


}




