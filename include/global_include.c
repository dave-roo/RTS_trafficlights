#include "global_include.h"
#include <stdio.h>

// Returns the response
message_data_t send_message(message_data_t* msg, char* attach_point){
	msg->hdr.type = 0x00;
	msg->hdr.subtype = 0x00;
	message_data_t msg_reply;
	msg_reply.msg_type = MSG_ERROR;
	msg_reply.data = MSG_NO_VALID_RESPONSE;
	int server_coid;
	if ((server_coid = name_open(attach_point, 0)) == -1){
		msg_reply.msg_type = MSG_ERROR;
		msg_reply.data = MSG_CONNECTION_ERROR;
		ConnectDetach(server_coid);
		return msg_reply;
	}
	if (MsgSend(server_coid, msg, sizeof(*msg), &msg_reply, sizeof(msg_reply)) == -1){
		msg_reply.msg_type = MSG_ERROR;
		msg_reply.data = MSG_SENDING_ERROR;
		ConnectDetach(server_coid);
		return msg_reply; // error
	}else{
		msg_reply.msg_type = msg->msg_type;
		msg_reply.data = msg->data;
		ConnectDetach(server_coid);
		return msg_reply; // success
	}
}
