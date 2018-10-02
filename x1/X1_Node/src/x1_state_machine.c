#include "x1_state_machine.h"

/* State machine output thread used to handle the outputs of the state machine
 * -> This wil be converted to hardware when implemented on the beaglebone to output
 * 	  to the LCD
 */
void* x1_state_machine_outputs(void* arg){
	// Cast the global struct
	sm_data_t* msg = (sm_data_t*) arg;

	// Last state variable so that the print statements don't spam the console (should
	// probably remove this on hardware..?)
	sem_wait(&msg->sem); // Protect Data
	x1_states last_state = msg->current_state;
	sem_post(&msg->sem); // Release

	while(1){
		sem_wait(&msg->sem);
		if(last_state != msg->current_state){
			switch(msg->current_state){
				case X1_STATE_0:
					printf("X1_STATE_0\n");
					break;
				case X1_STATE_1:
					printf("X1_STATE_1\n");
					break;
				case X1_STATE_2:
					printf("X1_STATE_2\n");
					break;
				case X1_STATE_3:
					printf("X1_STATE_3\n");
					break;
				case X1_STATE_4:
					printf("X1_STATE_4\n");
					break;
				case X1_STATE_5:
					printf("X1_STATE_5\n");
					break;
				case X1_STATE_6:
					printf("X1_STATE_6\n");
					break;
				case X1_STATE_7:
					printf("X1_STATE_7\n");
					break;
			}
		// Update the last state variable
		last_state = msg->current_state;
		}
		sem_post(&msg->sem);
	}
}

/* State machine input thread
 * -> This will handle the input variables and determine the next state conditions
 * -> COnditional wait on the signal flag
 */
void* x1_state_machine(void* arg){
	// Cast the global struct
	sm_data_t* msg = (sm_data_t*) arg;
	while(1){
		// Protect the data when the switch case is executed
		sem_wait(&msg->sem);
		if(msg->sensor_received){
			switch(msg->current_state){
				case X1_STATE_0:
					// Logic for next state
					// Check for priority message first
					if(msg->priority.set){
						// This flag is used to determine if the release flag has just been received
						msg->priority.last_state = X1_STATE_0;
						msg->priority.x1_reset = 1;
					}
					if(msg->priority.x1_reset){
						if(!msg->priority.set){
							// If reset but not set we know the release signal was just received
							msg->current_state = msg->priority.last_state;
							msg->priority.x1_reset = 0;
							break;
						}else{
							// Remain in this state until released
							break;
						}
					}


					if((msg->signal & 1 << X1_SIGNAL_Ein) && (msg->signal & 1 << X1_SIGNAL_Win)){ // E&W signal
						msg->current_state = X1_STATE_3;
					}else if(msg->signal & 1 << X1_SIGNAL_Ein){ // Ein Signal
						msg->current_state = X1_STATE_1;
					}else if(msg->signal & 1 << X1_SIGNAL_Win){ // Win Signal
						msg->current_state = X1_STATE_2;
					}
					break;

				case X1_STATE_1:
					// Logic for next state

					// Check for priority message first
					if(msg->priority.set){
						msg->priority.x1_reset = 1;
						msg->priority.last_state = msg->current_state;
						msg->current_state = X1_STATE_7;
						break;
					}

					if(msg->signal & 1 << X1_SIGNAL_Win){ // Win - second train detected
						msg->current_state = X1_STATE_3;
					}else if(msg->signal & 1 << X1_SIGNAL_BoomGateDown){
						msg->current_state = X1_STATE_4; // Boom gate is down
					}

					break;

				case X1_STATE_2:
					// Logic for next state

					// Check for priority message first
					if(msg->priority.set){
						msg->priority.x1_reset = 1;
						msg->priority.last_state = msg->current_state;
						msg->current_state = X1_STATE_7;
						break;
					}

					if(msg->signal & 1 << X1_SIGNAL_Ein){ // Ein - second train detected
						msg->current_state = X1_STATE_3;
					}else if(msg->signal & 1 << X1_SIGNAL_BoomGateDown){
						msg->current_state = X1_STATE_5; // Boom gate is down
					}
					break;

				case X1_STATE_3:
					// Logic for next state

					// Check for priority message first
					if(msg->priority.set){
						msg->priority.x1_reset = 1;
						msg->priority.last_state = msg->current_state;
						msg->current_state = X1_STATE_7;
						break;
					}

					if(msg->signal & 1 << X1_SIGNAL_BoomGateDown){ // Ein - second train detected
						msg->current_state = X1_STATE_6;
					}
					break;

				case X1_STATE_4:
					// Logic for next state

					// Check for priority message first
					if(msg->priority.set){
						msg->priority.x1_reset = 1;
						msg->priority.last_state = X1_STATE_1; // Need to return to state1 as boom gate is up
						msg->current_state = X1_STATE_7;
						break;
					}

					if(msg->signal & 1 << X1_SIGNAL_Win){ // Win - second train detected
						msg->current_state = X1_STATE_6;
					}else if(msg->signal & 1 << X1_SIGNAL_Eout){// Eout - Train clear of crossing
						msg->current_state = X1_STATE_7;
					}
					break;

				case X1_STATE_5:
					// Logic for next state

					// Check for priority message first
					if(msg->priority.set){
						msg->priority.x1_reset = 1;
						msg->priority.last_state = X1_STATE_2; // Need to return to state2 as boom gate is up
						msg->current_state = X1_STATE_7;
						break;
					}

					if(msg->signal & 1 << X1_SIGNAL_Ein){ // Win - second train detected
						msg->current_state = X1_STATE_6;
					}else if(msg->signal & 1 << X1_SIGNAL_Wout){// Eout - Train clear of crossing
						msg->current_state = X1_STATE_7;
					}
					break;

				case X1_STATE_6:
					// Logic for next state

					// Check for priority message first
					if(msg->priority.set){
						msg->priority.x1_reset = 1;
						msg->priority.last_state = X1_STATE_3; // Need to return to state3 as boom gate is up
						msg->current_state = X1_STATE_7;
						break;
					}

					if((msg->signal & 1 << X1_SIGNAL_Wout) && (msg->signal & 1 << X1_SIGNAL_Eout)){ // Win - second train detected
						msg->current_state = X1_STATE_7;
					}

					// Only needed for sensor thread, but will still work anyway
					else if(msg->signal & 1 << X1_SIGNAL_Wout){
						msg->current_state = X1_STATE_4;
					}else if(msg->signal & 1 << X1_SIGNAL_Eout){
						msg->current_state = X1_STATE_5;
					}
					break;

				case X1_STATE_7:

					if(msg->priority.set){
						if(~msg->signal & 1 << X1_SIGNAL_BoomGateDown){
							msg->current_state = X1_STATE_0;
							break;
						}
						// stay in this state until gate goes up
						break;
					}

					// Logic for next state
					if((msg->signal & 1 << X1_SIGNAL_Ein) && (msg->signal & 1 << X1_SIGNAL_Win)){ // E&W signal
						msg->current_state = X1_STATE_6;
					}else if(msg->signal & 1 << X1_SIGNAL_Ein){ // Ein Signal
						msg->current_state = X1_STATE_4;
					}else if(msg->signal & 1 << X1_SIGNAL_Win){ // Win Signal
						msg->current_state = X1_STATE_5;
					}else if(~msg->signal & 1 << X1_SIGNAL_BoomGateDown){
						msg->current_state = X1_STATE_0;
					}
					break;

				default:
					printf("State machine error\n");
					msg->current_state = X1_STATE_0;
					break;
			}
			msg->sensor_received = 0;
		}
		sem_post(&msg->sem);
	}
}
