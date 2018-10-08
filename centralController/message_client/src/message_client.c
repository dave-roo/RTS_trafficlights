#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <global_include.h>
#include <pthread.h>

void* sensor_send_thread(void*);

int main(void) {
	printf("\n****** Software Messaging Client for X1 Server ******\n\n");
	printf("  l -> Send lock signal\n  r -> Release lock signal\n  s -> Request current state\n\n");
	pthread_t char_thread;
	pthread_create(&char_thread, NULL, sensor_send_thread, NULL);
	pthread_join(char_thread, NULL);
	return EXIT_SUCCESS;
}

// Returns the response
message_data_t send_message(message_data_t* msg){
	message_data_t msg_reply;
	int server_coid;
	if ((server_coid = name_open(C1_QNET_ATTACH_POINT, 0)) == -1){
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

    msg.sending_node = NODE_I2; // set message signal
    msg.receiving_node = NODE_X1;
    msg.msg_type = MSG_CURRENT_STATE;

	// Main Loop
	while(1){
		char x = getchar();
		while(getchar() != '\n'); // Get remaining characters and discard
		switch(x){
			case 'l':
				msg.msg_type = MSG_CONTROL_STATE_LOCK;
				reply = send_message(&msg);
				if(reply.msg_type == MSG_ERROR){
					if(reply.data == MSG_CONNECTION_ERROR){
						printf("Connection Error!\n", reply.data);
					}
				}else{
					printf("Success! Received from node %d, data: %d\n", reply.sending_node, reply.data);
				}
				break;
			case 'r':
				msg.msg_type = MSG_CONTROL_STATE_RELEASE;
				reply = send_message(&msg);
				if(reply.msg_type == MSG_ERROR){
					if(reply.data == MSG_CONNECTION_ERROR){
						printf("Connection Error!\n", reply.data);
					}
				}else{
					printf("Success! Received from node %d, data: %d\n", reply.sending_node, reply.data);
				}
				break;
			case 's':
				msg.msg_type = MSG_CURRENT_STATE;
				reply = send_message(&msg);
				if(reply.msg_type == MSG_ERROR){
					if(reply.data == MSG_CONNECTION_ERROR){
						printf("Connection Error!\n", reply.data);
					}
				}else{
					printf("Success! Received from node %d, data: %d\n", reply.sending_node, reply.data);
				}
				break;
			default:
				break;
		}
	}
}