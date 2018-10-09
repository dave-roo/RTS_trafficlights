#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <global_include.h>
#include <x1_include.h>
#include <pthread.h>

#define LOCAL_ATTACH_POINT "c1_group_14"

void* sensor_send_thread(void*);

int main(void) {
	printf("\n****** Software Messaging Client for Controller Server ******\n\n");
	printf("  a -> Update Current State\n  b -> Send bad request\n '' -> NYI\n\n");
	pthread_t char_thread;
	pthread_create(&char_thread, NULL, sensor_send_thread, NULL);
	pthread_join(char_thread, NULL);
	return EXIT_SUCCESS;
}

// Returns the response
message_data_t send_message(message_data_t* msg){
	msg->hdr.type = 0x00;
	msg->hdr.subtype = 0x00;
	message_data_t msg_reply;
	int server_coid;
	if ((server_coid = name_open(LOCAL_ATTACH_POINT, 0)) == -1){
		msg_reply.msg_type = MSG_ERROR;
		msg_reply.data = MSG_CONNECTION_ERROR;
		return msg_reply;
	}
	if (MsgSend(server_coid, msg, sizeof(*msg), &msg_reply, sizeof(msg_reply)) == -1){
		printf("Message sending error\n");
		msg_reply.msg_type = MSG_ERROR;
		msg_reply.data = MSG_SENDING_ERROR;
		return msg_reply; // error
	}else{
		return msg_reply; // success
	}
}

void* sensor_send_thread(void* data){
	message_data_t msg;
	message_data_t reply;

    msg.sending_node = NODE_X1; // set message signal
    msg.receiving_node = NODE_CONTROLLER;
    msg.msg_type = MSG_CURRENT_STATE_REQUEST;

	// Main Loop
	while(1){
		char x = getchar();
		while(getchar() != '\n'); // Get remaining characters and discard
		switch(x){
			case 'a':
				printf("Enter the state number\n");
				msg.msg_type = MSG_CURRENT_STATE_UPDATE;
				msg.data = getchar();
				while(getchar() != '\n');
				reply = send_message(&msg);
				if(reply.msg_type == MSG_ERROR){
					switch(reply.data){
						case MSG_CONNECTION_ERROR:
							printf("Connection Error!\n");
							break;
						case MSG_SENDING_ERROR:
							printf("Message did not send\n");
							break;
						case MSG_BAD_REQUEST:
							printf("Server rejected this message type\n");
							break;
					}
				}else{
					printf("Success! Received from node %d, data: %d\n", reply.sending_node, reply.data);
				}
				break;
			case 'b':
				printf("Sending bad request\n");
				msg.msg_type = MSG_CONTROL_STATE_LOCK;
				reply = send_message(&msg);

				// Handle Errors
				if(reply.msg_type == MSG_ERROR){
					switch(reply.data){
						case MSG_CONNECTION_ERROR:
							printf("Connection Error!\n");
							break;
						case MSG_SENDING_ERROR:
							printf("Message did not send\n");
							break;
						case MSG_BAD_REQUEST:
							printf("Controller rejected this message type\n");
							break;
					}
				}else{
					printf("Success! Received from node %d, data: %d\n", reply.sending_node, reply.data);
				}
				break;
				break;
			default:
				break;
		}
	}
}
