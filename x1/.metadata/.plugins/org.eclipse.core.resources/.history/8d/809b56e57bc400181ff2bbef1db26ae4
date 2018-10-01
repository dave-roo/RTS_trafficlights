#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Global includes
#include <data_types.h>
#include <x1_include.h>

// Struct to pass global data
typedef struct{
	x1_states current_state;
	uint8_t sensor_received;
	uint8_t signal;
	sem_t sem;
} sm_data_t;

// Func prototypes
void* sensor_send_thread(void*);
void* x1_state_machine(void*);

int main(void) {
	printf("Begin X1 State Machine Node\n");

//	Initialise global variable
	sm_data_t global_data;
	global_data.current_state = X1_STATE_0;
	global_data.sensor_received = 0; // Ensure this is zero
	global_data.signal = 0;
	sem_init(&global_data.sem, 0, 1);

//	Start Sensor thread
	pthread_t sensor_thread, state_machine_thread;
	pthread_create(&sensor_thread, NULL, sensor_send_thread, &global_data);
	pthread_create(&state_machine_thread, NULL, x1_state_machine, &global_data);
	pthread_join(sensor_thread, NULL);
	pthread_join(state_machine_thread, NULL);
	printf("State Machine Node Ended\n");
	return EXIT_SUCCESS;
}

// Thread to handle the sensor inputs to determine the next state
void* x1_state_machine(void* arg){
	sm_data_t* msg = (sm_data_t*) arg;
	while(1){
		sem_wait(&msg->sem);
		switch(msg->current_state){
			case X1_STATE_0:
//				printf("State 0, CloseBoom:0, E_Light:R, W_Light:R\n");

				// Logic for next state
				if(msg->sensor_received){
					if((msg->signal & 1 << X1_SIGNAL_Ein) && (msg->signal & 1 << X1_SIGNAL_Win)){ // E&W signal
						msg->current_state = X1_STATE_3;
						printf("Current State 0, Next State 3\n");
					}else if(msg->signal & 1 << X1_SIGNAL_Ein){ // Ein Signal
						msg->current_state = X1_STATE_1;
						printf("Current State 0, Next State 1\n");
					}else if(msg->signal & 1 << X1_SIGNAL_Win){ // Win Signal
						msg->current_state = X1_STATE_2;
						printf("Current State 0, Next State 2\n");
					}
				}
				break;

			case X1_STATE_1:
//				printf("State 1, CloseBoom:1, E_Light:R, W_Light:R\n");

				// Logic for next state
				if(msg->sensor_received){
					if(msg->signal & 1 << X1_SIGNAL_Win){ // Win - second train detected
						msg->current_state = X1_STATE_3;
						printf("Current State 1, Next State 3\n");
					}else if(msg->signal & 1 << X1_SIGNAL_BoomGateDown){
						msg->current_state = X1_STATE_4; // Boom gate is down
						printf("Current State 1, Next State 4\n");
					}
				}
				break;
			case X1_STATE_2:
//				printf("State 2, CloseBoom:1, E_Light:R, W_Light:R\n");

				// Logic for next state
				if(msg->sensor_received){
					if(msg->signal & 1 << X1_SIGNAL_Ein){ // Ein - second train detected
						msg->current_state = X1_STATE_3;
						printf("Current State 2, Next State 3\n");
					}else if(msg->signal & 1 << X1_SIGNAL_BoomGateDown){
						msg->current_state = X1_STATE_5; // Boom gate is down
						printf("Current State 2, Next State 5\n");
					}
				}
				break;
			case X1_STATE_3:
//				printf("State 3, CloseBoom:1, E_Light:R, W_Light:R\n");

				// Logic for next state
				if(msg->sensor_received){
					if(msg->signal & 1 << X1_SIGNAL_BoomGateDown){ // Ein - second train detected
						msg->current_state = X1_STATE_6;
						printf("Current State 3, Next State 6\n");
					}
				}
				break;
			case X1_STATE_4:
//				printf("State 4, CloseBoom:1, E_Light:G, W_Light:R\n");

				// Logic for next state
				if(msg->sensor_received){
					if(msg->signal & 1 << X1_SIGNAL_Win){ // Win - second train detected
						msg->current_state = X1_STATE_6;
						printf("Current State 4, Next State 6\n");
					}else if(msg->signal & 1 << X1_SIGNAL_Eout){// Eout - Train clear of crossing
						msg->current_state = X1_STATE_7;
						printf("Current State 4, Next State 7\n");
					}
				}
				break;
			case X1_STATE_5:
//				printf("State 5, CloseBoom:1, E_Light:R, W_Light:G\n");

				// Logic for next state
				if(msg->sensor_received){
					if(msg->signal & 1 << X1_SIGNAL_Ein){ // Win - second train detected
						msg->current_state = X1_STATE_6;
						printf("Current State 5, Next State 6\n");
					}else if(msg->signal & 1 << X1_SIGNAL_Wout){// Eout - Train clear of crossing
						msg->current_state = X1_STATE_7;
						printf("Current State 5, Next State 7\n");
					}
				}
				break;
			case X1_STATE_6:
//				printf("State 6, CloseBoom:1, E_Light:G, W_Light:G\n");

				// Logic for next state
				if(msg->sensor_received){
					if((msg->signal & 1 << X1_SIGNAL_Wout) && (msg->signal & 1 << X1_SIGNAL_Eout)){ // Win - second train detected
						msg->current_state = X1_STATE_7;
						printf("Current State 6, Next State 7\n");
					}

					// Only needed for sensor thread, but will still work anyway
					else if(msg->signal & 1 << X1_SIGNAL_Wout){
						msg->current_state = X1_STATE_4;
						printf("Current State 6, Next State 4\n");
					}else if(msg->signal & 1 << X1_SIGNAL_Eout){
						msg->current_state = X1_STATE_5;
						printf("Current State 6, Next State 5\n");
					}
				}
				break;
			case X1_STATE_7:
//				printf("State 7, CloseBoom:0, E_Light:R, W_Light:R\n");
				// Logic for next state
				if(msg->sensor_received){
					if((msg->signal & 1 << X1_SIGNAL_Ein) && (msg->signal & 1 << X1_SIGNAL_Win)){ // E&W signal
						msg->current_state = X1_STATE_6;
						printf("Current State 7, Next State 6\n");
					}else if(msg->signal & 1 << X1_SIGNAL_Ein){ // Ein Signal
						msg->current_state = X1_STATE_4;
						printf("Current State 7, Next State 4\n");
					}else if(msg->signal & 1 << X1_SIGNAL_Win){ // Win Signal
						msg->current_state = X1_STATE_5;
						printf("Current State 7, Next State 5\n");
					}else if(~msg->signal & 1 << X1_SIGNAL_BoomGateDown){
						msg->current_state = X1_STATE_0;
					}
				}
				break;
			default:
				printf("State machine error\n");
				msg->current_state = X1_STATE_0;
				break;
		}
		msg->sensor_received = 0;
		sem_post(&msg->sem);
	}
}

/*
 * Thread to get char and send to x1 node. (will be replaced by a hardware thread)
 *
 */
void* sensor_send_thread(void* data){
	sm_data_t* sens_data = (sm_data_t*) data;

	// Main Loop
	while(1){
		char x = getchar();
		getchar(); // get the newline

		// Clear the sensor data except the latching boomgate sensor (only needed for sensor thread)
		sem_wait(&sens_data->sem);
		sens_data->signal &= (1 << X1_SIGNAL_BoomGateDown);
		sem_post(&sens_data->sem);

		switch(x){
			// East approching signal
			case 'e':

				sem_wait(&sens_data->sem);
				sens_data->sensor_received = 1;
				sens_data->signal |= 1 << X1_SIGNAL_Ein;
				sem_post(&sens_data->sem);

				printf("Train Approaching from the East\n");
				break;

			// East Leaving signal
			case 'd':

				sem_wait(&sens_data->sem);
				sens_data->sensor_received = 1;
				sens_data->signal |= 1 << X1_SIGNAL_Eout;
				sem_post(&sens_data->sem);

				printf("Train clear on the East line\n");
				break;
			// West Approaching signal
			case 'w':

				sem_wait(&sens_data->sem);
				sens_data->sensor_received = 1;
				sens_data->signal |= 1 << X1_SIGNAL_Win;
				sem_post(&sens_data->sem);

				printf("Train Approaching from the West\n");
				break;
			// West Departing signal
			case 's':

				sem_wait(&sens_data->sem);
				sens_data->sensor_received = 1;
				sens_data->signal |= 1 << X1_SIGNAL_Wout;
				sem_post(&sens_data->sem);

				printf("Train clear on the West line\n");
				break;
			// Boom gate down signal
			case 'b':

				sem_wait(&sens_data->sem);
				sens_data->sensor_received = 1;
				sens_data->signal |= 1 << X1_SIGNAL_BoomGateDown;
				sem_post(&sens_data->sem);

				printf("Boom gate down\n");
				break;
			// Boom gate up signal
			case 'u':

				sem_wait(&sens_data->sem);
				sens_data->sensor_received = 1;
				sens_data->signal &= ~(1 << X1_SIGNAL_BoomGateDown);
				sem_post(&sens_data->sem);

				printf("Boom gate up\n");
				break;
			default:
				printf("Unrecognised character, please try again\n");
				break;
		}
		printf("Signal Value:%#x Current State:%#x\n", sens_data->signal, sens_data->current_state);
	}
}
