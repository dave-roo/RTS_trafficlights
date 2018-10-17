/*
 * thread_ex2.c
 *
 * Simple example of creating multiple threads.
 *
*/
#include "messageController.h"


int get_All_Sensor()
{

		int ret = -1;
		ret = getTrainStatus();
		if(ret<0) ret = 0;
		if(ret==1)printf("====> Train\n");
		return ret;

}

/*
 * this function connects to 'server' which is a node that has info we want
 * this function connects once and returns reply value or errors
 * error handling should we done by higher level function
 */
int getTrainStatus()
{
	message_data_t msg;
	message_data_t reply;

    msg.sending_node = NODE_I2; // set message signal
    msg.receiving_node = NODE_X1;
    msg.msg_type = MSG_TRAIN_SIGNAL;
    msg.data = 1;


    int server_coid;
    int index = 0;
    int return_value = -1;

    if ((server_coid = name_open(X1_QNET_ATTACH_POINT, 0)) == -1)
    {
        return_value = -2;//server connect error
    }
    else {
		msg.hdr.type = 0x00;
		msg.hdr.subtype = 0x00;
		fflush(stdout);

		if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1)
		{
			return_value = -1; // reply data error
		}
		else
		{
			return_value = reply.data;
		}
    }
    name_close(server_coid);


    //reply =  send_message(&msg, X1_QNET_ATTACH_POINT);
   /* printf("%d", reply.msg_type);
    printf("%d", reply.data);
    printf("%d", reply.sending_node);
    printf("%d", reply.receiving_node);
*/
    if(reply.msg_type == MSG_ERROR){
    	return -1;
    }

    return reply.data;

//    return return_value;
}



int updateController(state_data *td)
{

	message_data_t msg;
	message_data_t reply;

    msg.sending_node = NODE_I2; // set message signal
    msg.receiving_node = NODE_CONTROLLER;
    msg.msg_type = MSG_CURRENT_STATE_UPDATE;

    msg.data = td->state;

    //printf("state update: %d\n",  msg.data );

    int server_coid;
    int index = 0;
    int return_value = -1;
    //printf("  ---> Trying to connect to server named: %s\n", sname);


    if ((server_coid = name_open(C1_QNET_ATTACH_POINT, 0)) == -1)
    {
        //printf("\n    ERROR, could not connect to server!\n\n");
        return_value = -2;//server connect error
    }
    else {
		//printf("Connection established to: %s\n", sname);
		msg.hdr.type = 0x00;
		msg.hdr.subtype = 0x00;

		// the data we are sending is in msg.data
		//printf("Client (ID:%d), sending data packet with the integer value: %d \n", msg.sending_node, msg.data);
		fflush(stdout);

		if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1)
		{
			//printf(" Error data NOT sent to server\n");
			return_value = -1; // reply data error
		}
		else
		{ // now process the reply
			//printf("   -->Reply good\n");
			//printf("   -->Reply is: '%d'\n", reply.data);
			return_value = reply.data;
		}
    }
    name_close(server_coid);

    return return_value;
}

int updateControllerLocked(state_data *td)
{

	message_data_t msg;
	message_data_t reply;

    msg.sending_node = NODE_I2; // set message signal
    msg.receiving_node = NODE_CONTROLLER;
    msg.msg_type = MSG_CONTROL_STATE_LOCKED;
    msg.data = 1;

    //printf("state update: %d\n",  msg.data );

    int server_coid;
    int index = 0;
    int return_value = -1;
    //printf("  ---> Trying to connect to server named: %s\n", sname);


    if ((server_coid = name_open(C1_QNET_ATTACH_POINT, 0)) == -1)
    {
        //printf("\n    ERROR, could not connect to server!\n\n");
        return_value = -2;//server connect error
    }
    else {
		//printf("Connection established to: %s\n", sname);
		msg.hdr.type = 0x00;
		msg.hdr.subtype = 0x00;

		// the data we are sending is in msg.data
		//printf("Client (ID:%d), sending data packet with the integer value: %d \n", msg.sending_node, msg.data);
		fflush(stdout);

		if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1)
		{
			//printf(" Error data NOT sent to server\n");
			return_value = -1; // reply data error
		}
		else
		{ // now process the reply
			//printf("   -->Reply good\n");
			//printf("   -->Reply is: '%d'\n", reply.data);
			return_value = reply.data;
		}
    }
    name_close(server_coid);

    return return_value;
}

intersection_message getHardwareSensors(intersection_message *msg){

}
