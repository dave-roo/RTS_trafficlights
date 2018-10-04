#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_
#include "x1_include.h"
#include <semaphore.h>

// Struct to hold global struct for the controller
typedef struct{
    x1_states x1_current_state;
    sem_t sem;
} controller_data_t;
#endif
