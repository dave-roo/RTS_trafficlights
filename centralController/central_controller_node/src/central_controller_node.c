#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <global_include.h>
#include <x1_include.h>
#include <controller_include.h>

#define LOCAL_ATTACH_POINT "c1_group_14"

/*Function Declarations*/
void* c1_message_server_thread(void*);
void c1_global_init(controller_data_t*);

int main(void) {
	// Create Global Struct
	controller_data_t global_data;
	c1_global_init(&global_data);

	pthread_t c1_message_thread;
	pthread_create(&c1_message_thread, NULL, c1_message_server_thread, NULL);

	pthread_join(c1_message_thread, NULL);

}

/* Controller Message Server Thread - This will handle all messages received from
 * the intersections and railway crossing. This will be responsible for receiving
 * state changes from any node and updating on the controller screen accordingly.
 * Arguments:
 * 	-> controller_data_t
 * @returns
 * 	-> Null
 */
void* c1_message_server_thread(void* arg){
	controller_data_t* controller_data = (controller_data_t*) arg;

	name_attach_t *attach;
	if ((attach = name_attach(NULL, LOCAL_ATTACH_POINT, 0)) == NULL){
		printf("\nFailed to name_attach on: %s \n", LOCAL_ATTACH_POINT);
		printf("\n Possibly another server with the same name is already running !\n");
		return 0;
	}
	printf("Server Listening for Clients on: %s \n", LOCAL_ATTACH_POINT);

	// Setup the sending and receiving message structs
	message_data_t msg;
	message_data_t msg_reply;
	msg_reply.sending_node = NODE_CONTROLLER;
	msg_reply.hdr.type = 0x01;
	msg_reply.hdr.subtype = 0x00;

	int rcvid=0;
	while (1){
		// Receive any message
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

		// Message receive failed
		if (rcvid == -1){
			printf("\nFailed to MsgReceive\n");
			break;
		}

		// Determine the pulse code
		if(rcvid == 0){
			switch(msg.hdr.code){
				case _PULSE_CODE_DISCONNECT:
					printf("_PULSE_CODE_DISCONNECT\n");
					break;
				case _PULSE_CODE_UNBLOCK:
					printf("_PULSE_CODE_UNBLOCK");
					break;
				case _PULSE_CODE_COIDDEATH:
					printf("_PULSE_CODE_COIDDEATH");
					break;
				case _PULSE_CODE_THREADDEATH:
					printf("_PULSE_CODE_THREADDEATH");
					break;
				default:
					printf("Something else received\n");
					break;
			}
		continue;
		}
		if(rcvid > 0){
			printf("Message Received %d\n", msg.sending_node);
			msg_reply.receiving_node = msg.sending_node;
			msg_reply.msg_type = msg.msg_type;

			/* TODO
			 * -> Update controller_data_t struct with intersection node state machine enums
			 * ->
			 */




			switch(msg.msg_type){
				case MSG_TRAIN_SIGNAL:
					printf("Signal Message Request\n");
					break;
				case MSG_CURRENT_STATE:
					printf("Current State Request\n");
					break;
				case MSG_CONTROL_STATE_LOCK:
					printf("Locking State\n");
					break;
				case MSG_CONTROL_STATE_RELEASE:
					printf("Releasing State Request\n");
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
	}
	// Remove the attach point name from the file system (i.e. /dev/name/local/<myname>)
	name_detach(attach, 0);
	return (int*) EXIT_FAILURE;
}


/* Function to initialise the controller global struct
 */
void c1_global_init(controller_data_t* data){
	data->x1_current_state = X1_STATE_0;
	// Initialise the global semaphore
	sem_init(&data->sem, 0, 1);
}
