/*
 * thread_ex2.c
 *
 * Simple example of creating multiple threads.
 *
*/
#include "messageController.h"


struct intersection_message *get_All_Sensor()
{

		intersection_message m = {.train_status = 1, .NE = 1, .SW = 0, .ES = 0, .WN = 0}; // change this struct to test different messages - will need function to really get messages later
		//turning sensors are within same node
		//train is external

		mess = &m;
		return mess;
}

/*
 * this function connects to 'server' which is a node that has info we want
 * this function connects once and returns reply value or errors
 * error handling should we done by higher level function
 */
int getTrainStatus(char *sname)
{
	message_data_t msg;
	message_data_t reply;

    msg.sending_node = NODE_I1; // set message signal
    msg.receiving_node = NODE_X1;
    msg.msg_type = MSG_TRAIN_SIGNAL;


    int server_coid;
    int index = 0;
    int return_value = -1;
    printf("  ---> Trying to connect to server named: %s\n", sname);
    if ((server_coid = name_open(sname, 0)) == -1)
    {
        printf("\n    ERROR, could not connect to server!\n\n");
        return_value = -2;//server connect error
    }

    printf("Connection established to: %s\n", sname);

	// the data we are sending is in msg.data
	printf("Client (ID:%d), sending data packet with the integer value: %d \n", msg.sending_node, msg.data);
	fflush(stdout);

	if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1)
	{
		printf(" Error data NOT sent to server\n");
		return_value = -1; // reply data error
	}
	else
	{ // now process the reply
		printf("   -->Reply is: '%s'\n", reply.data);
		return_value = reply.data;
	}
    name_close(server_coid);

    return return_value;
}

