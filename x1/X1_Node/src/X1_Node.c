#include <stdio.h>
#include <stdlib.h>
#include <data_types.h>
#include <x1_include.h>



void send_current_state(x1_states* cur_sta){
	message_data_t msg;
	msg.sending_node = NODE_I1;
	msg.msg_type = MSG_CURRENT_STATE;
	msg.data = *cur_sta;

	printf("Sent %d\n", msg.data);
	// Send msg
}

int main(void) {
	printf("Begin X1 State Machine Node\n");
	// sta to store current state
	x1_states sta = X1_STATE_1;

	// Update current state
	send_current_state(&sta);
	printf("State Machine Node Ended\n");
	return EXIT_SUCCESS;
}