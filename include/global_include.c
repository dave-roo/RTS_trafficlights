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
int timer_funct(int time_period){

	struct sigevent         event;
	struct itimerspec       itime;
	timer_t                 timer_id;
	int                     chid;
	int                     rcvid;
	my_message_t            msg;

	chid = ChannelCreate(0); // Create a communications channel

	event.sigev_notify = SIGEV_PULSE;

	// create a connection back to ourselves for the timer to send the pulse on
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
	if (event.sigev_coid == -1)
	{	
		perror(NULL);
		return 0;
	}
	//event.sigev_priority = getprio(0);  // this function is depreciated in QNX 700
	struct sched_param th_param;
	pthread_getschedparam(pthread_self(), NULL, &th_param);
	event.sigev_priority = th_param.sched_curpriority;    // old QNX660 version getprio(0);

	event.sigev_code = MY_PULSE_CODE;

	// create the timer, binding it to the event
	if (timer_create(CLOCK_REALTIME, &event, &timer_id) == -1)
	{
		perror (NULL);
		return 0;
	}

	// setup the timer (1.5s initial delay value, 1.5s reload interval)
	itime.it_value.tv_sec = time_period;			  // 1 second
	itime.it_value.tv_nsec = 1;    // 500 million nsecs = .5 secs
	itime.it_interval.tv_sec = time_period;          // 1 second
	itime.it_interval.tv_nsec = 1;

	// and start the timer!
	timer_settime(timer_id, 0, &itime, NULL);

	rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);

	// determine who the message came from
	if (rcvid == 0) // this process
	{
		// received a pulse, now check "code" field...
		if (msg.pulse.code == MY_PULSE_CODE) // we got a pulse
		{
			fflush(stdout); // make sure we print to the screen
		}
		// else other pulses ...
	}
	// else other messages ...


	//printf("Timer Terminated...\n\n");
	return 1;
	//return EXIT_SUCCESS;
}