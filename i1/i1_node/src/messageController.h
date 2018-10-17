/*
 * messageCOntroller.h
 *
 *  Created on: 2Oct.,2018
 *      Author: user
 */

#ifndef SRC_MESSAGECONTROLLER_H_
#define SRC_MESSAGECONTROLLER_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <mqueue.h>
#include <sys/stat.h>

#include <errno.h>
#include <sys/iofunc.h>

#include <sys/netmgr.h>
#include <sync.h>
#include <sys/neutrino.h>


#include <global_include.h>
#include <x1_include.h>
#include <i1_include.h>
//#include "I1DataTypes.h"
#define BUF_SIZE 100



typedef struct
{
	 int train_status;
	 int NE;
	 int SW;
	 int ES;
	 int WN;
	 pthread_mutex_t mutex;
} intersection_message;

typedef struct
{
	struct _pulse hdr; // Our real data comes after this header
	int ClientID; // our data (unique id from client)
    int data;     // our data
} my_data;

typedef struct
{
	struct _pulse hdr; // Our real data comes after this header
    char buf[BUF_SIZE];// Message we send back to clients to tell them the messages was processed correctly.
} my_reply;

//#define X1_QNET_ATTACH_POINT  "/net/s3484378-01/dev/name/local/myname"
intersection_message *mess;


struct intersection_message *get_All_Sensor();
int client(char *sname);
int updateController(state_data *td);
int updateControllerLocked(state_data *td);
//int main(int argc, char *argv[]);


#endif /* SRC_MESSAGECONTROLLER_H_ */
