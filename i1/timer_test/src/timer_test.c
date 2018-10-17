/*
 * Demonstrate how to set up a timer that, on expiry,
 * sends us a pulse.  This example sets the first
 * expiry to 1.5 seconds and the repetition interval
 * to 1.5 seconds.
 */




#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>

#include <errno.h>


char *progname = "timer_per1.c";

enum states { State0, State1, State2};

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

typedef union
{
	struct _pulse   pulse;
    // your other message structures would go here too
} my_message_t;


int main(int argc, char *argv[])
{
	printf("##########################################################\n");
	printf("#                                                        #\n");
	printf("#              EXAMPLE Code                              #\n");
	printf("#                                                        #\n");
	printf("#         QNX implementation of POSIX timers             #\n");
	printf("#                                                        #\n");
	printf("#  Process runs a simple state machine {states: 0,1,2}   #\n");
	printf("#                                                        #\n");
	printf("##########################################################\n");

	enum states CurrentState = State0;

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
	   printf(stderr, "%s:  couldn't ConnectAttach to self!\n", progname);
	   perror(NULL);
	   exit(EXIT_FAILURE);
	}
	//event.sigev_priority = getprio(0);  // this function is depreciated in QNX 700
	struct sched_param th_param;
	pthread_getschedparam(pthread_self(), NULL, &th_param);
	event.sigev_priority = th_param.sched_curpriority;    // old QNX660 version getprio(0);

	event.sigev_code = MY_PULSE_CODE;

	// create the timer, binding it to the event
	if (timer_create(CLOCK_REALTIME, &event, &timer_id) == -1)
	{
	   printf (stderr, "%s:  couldn't create a timer, errno %d\n", progname, errno);
	   perror (NULL);
	   exit (EXIT_FAILURE);
	}

	// setup the timer (1.5s initial delay value, 1.5s reload interval)
	itime.it_value.tv_sec = 1;			  // 1 second
	itime.it_value.tv_nsec = 500000000;    // 500 million nsecs = .5 secs
	itime.it_interval.tv_sec = 1;          // 1 second
	itime.it_interval.tv_nsec = 500000000; // 500 million nsecs = .5 secs

	// and start the timer!
	timer_settime(timer_id, 0, &itime, NULL);

	/*
	* As of the timer_settime(), we will receive our pulse
	* in 1.5 seconds (the itime.it_value) and every 1.5
	* seconds thereafter (the itime.it_interval)
	*/
		// wait for message/pulse
	   rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);

	   // determine who the message came from
	   if (rcvid == 0) // this process
	   {
		   // received a pulse, now check "code" field...
		   if (msg.pulse.code == MY_PULSE_CODE) // we got a pulse
			{
				// Here we implement a simple state machine
				printf("In state = %d\n", CurrentState);
				switch (CurrentState)
				{
					case 0:
						 // do some work;
						 CurrentState = State1;
						 break;
					case 1:
						 // do some work;
						 CurrentState = State2;
						 break;
					case 2:
						 // do some work;
						 CurrentState = State0;
						 break;
				}
				fflush(stdout); // make sure we print to the screen
			}
			// else other pulses ...
	   }

	printf("\nSwitch statement got called times\n");

	printf("All good. Main Terminated...\n\n");
	return EXIT_SUCCESS;
}

