#include "stateController.h"

void switch_peak(state_data * td){
switch(td->state){

  		case i1_State_0:// ALL RED STATE

  			timer_funct(RED);
  			if(td->lock_status == 1)td->state = i1_State_1;
  			else if(td->msg.train_status == 1)td->state = i1_State_11;
  			else if(td->msg.NE == 0 && td->msg.SW == 0)td->state = i1_State_1;
  			else if(td->msg.NE == 0 && td->msg.SW == 1)td->state = i1_State_9;
  			else if(td->msg.NE == 1 && td->msg.SW == 0)td->state = i1_State_5;
  			else if(td->msg.NE == 1 && td->msg.SW == 1)td->state = i1_State_7;
  			else td->state = i1_State_1;
  			td->msg.NE = 0;td->msg.SW = 0;

  			break;

  		case i1_State_1://GREEN NS
  			timer_funct(GREEN_STRT);
  			if(td->lock_status == 1){
				td->state = i1_State_1;

				updateControllerLocked(td);
			}
  			else td->state = i1_State_2;
			break;

  		case i1_State_2://YELLOW NS
  			timer_funct(YELLOW);
			td->state = i1_State_11;
			break;
  		case i1_State_3://GREEN EW

  			timer_funct(GREEN_STRT);
  			if(td->msg.train_status == 1){td->state = i1_State_3;}
  			else if(td->msg.train_status == 0){td->state = i1_State_4;}
  			else {td->state = i1_State_4;}
			break;

  		case i1_State_4://YELLOW EW

  			timer_funct(YELLOW);
			td->state = i1_State_0;
			break;

  		case i1_State_5://GREEN TURN NE

  			timer_funct(GREEN_TURN);
  			if(td->msg.train_status == 1)td->state = i1_State_18;
  			else td->state = i1_State_6;
  			break;

  		case i1_State_6://YELLOW TURN

  			timer_funct(YELLOW);
  			if(td->msg.train_status == 1)td->state = i1_State_18;
  			else td->state = i1_State_1;
  			break;
  		case i1_State_7://GREEN TURN NE & SW

  			timer_funct(GREEN_TURN);
  			td->state = i1_State_8;
  			break;
  		case i1_State_8://YELLOW TURN

  			timer_funct(YELLOW);
  			if(td->msg.train_status == 1)td->state = i1_State_11;
  			else td->state = i1_State_1;
  			break;
  		case i1_State_9://GREEN TURN SW

  			timer_funct(GREEN_TURN);
  			if(td->msg.train_status == 1)td->state = i1_State_19;
  			else td->state = i1_State_10;
  			break;
  		case i1_State_10://YELLOW TURN

  			timer_funct(YELLOW);
  			if(td->msg.train_status == 1)td->state = i1_State_19;
  			else td->state = i1_State_1;
  			break;


  		case i1_State_11:// ALL RED STATE

  			timer_funct(RED);

  			if(td->msg.train_status == 1){
				if(td->msg.ES == 0 && td->msg.WN == 1){
					td->state = i1_State_16;
				}
				else td->state = i1_State_3;
  			}
  			else if(td->msg.ES == 0 && td->msg.WN == 0)td->state = i1_State_3;
  			else if(td->msg.ES == 0 && td->msg.WN == 1)td->state = i1_State_16;
  			else if(td->msg.ES == 1 && td->msg.WN == 0)td->state = i1_State_12;
  			else if(td->msg.ES == 1 && td->msg.WN == 1)td->state = i1_State_14;
  			else td->state = i1_State_3;
  			td->msg.ES = 0;td->msg.WN = 0;
  			break;

  		case i1_State_12://GREEN TURN ES

  			timer_funct(GREEN_TURN);
  			td->state = i1_State_13;
  			break;
  		case i1_State_13://YELLOW TURN

  			timer_funct(YELLOW);
  			td->state = i1_State_3;
  			break;
  		case i1_State_14://GREEN TURN ES & WN

  			timer_funct(GREEN_TURN);
  			td->state = i1_State_15;
  			break;
  		case i1_State_15://YELLOW TURN

  			timer_funct(YELLOW);
  			td->state = i1_State_3;
  			break;
  		case i1_State_16://GREEN TURN WN

  			timer_funct(GREEN_TURN);
  			td->state = i1_State_17;
  			break;
  		case i1_State_17://YELLOW TURN

  			timer_funct(YELLOW);
  			td->state = i1_State_3;
  			break;


  		case i1_State_18://YELLOW TURN-TO-ALL

  			timer_funct(YELLOW);
  			td->state = i1_State_11;
  			break;
  		case i1_State_19://YELLOW TURN-TO-ALL

  			timer_funct(YELLOW);
  			td->state = i1_State_11;
  			break;
  		default:
  			break;

  	}
  	if(td->lock_status == 1){
  		if(td->state == i1_State_11){
			td->state = i1_State_0;
		}
	}

}



/*
 * loop in state one if one EW related
 * ignore train
 *
 * no turning - turn when safe
 *
 */
void switch_off_peak(state_data * td){
switch(td->state){

  		case i1_State_0:// ALL RED STATE

  			timer_funct(RED);
  			if(td->lock_status == 1)td->state = i1_State_1;
  			else if(td->msg.train_status == 1)td->state = i1_State_11;
  			else if(td->msg.NE == 0 && td->msg.SW == 0)td->state = i1_State_1;
  			else if(td->msg.NE == 0 && td->msg.SW == 1)td->state = i1_State_9;
  			else if(td->msg.NE == 1 && td->msg.SW == 0)td->state = i1_State_5;
  			else if(td->msg.NE == 1 && td->msg.SW == 1)td->state = i1_State_7;
  			else td->state = i1_State_1;
  			td->msg.NE = 0;td->msg.SW = 0;

  			break;

  		case i1_State_1://GREEN NS
  			timer_funct(GREEN_STRT);
  			if(td->lock_status == 1){
				td->state = i1_State_1;
				updateControllerLocked(td);
			}
  			else if(td->msg.EW == 1 || td->msg.ES == 1 || td->msg.WN == 1) td->state = i1_State_2;
  			else td->state = i1_State_1;
			break;

  		case i1_State_2://YELLOW NS
  			timer_funct(YELLOW);
			td->state = i1_State_11;
			break;
  		case i1_State_3://GREEN EW

  			timer_funct(GREEN_STRT);
  			if(td->msg.train_status == 1){td->state = i1_State_3;}
  			else if(td->msg.train_status == 0){td->state = i1_State_4;}
  			else {td->state = i1_State_4;}
			break;

  		case i1_State_4://YELLOW EW

  			timer_funct(YELLOW);
			td->state = i1_State_0;
			break;

  		case i1_State_5://GREEN TURN NE

  			timer_funct(GREEN_TURN);
  			if(td->msg.train_status == 1)td->state = i1_State_18;
  			else td->state = i1_State_6;
  			break;

  		case i1_State_6://YELLOW TURN

  			timer_funct(YELLOW);
  			if(td->msg.train_status == 1)td->state = i1_State_18;
  			else td->state = i1_State_1;
  			break;
  		case i1_State_7://GREEN TURN NE & SW

  			timer_funct(GREEN_TURN);
  			td->state = i1_State_8;
  			break;
  		case i1_State_8://YELLOW TURN

  			timer_funct(YELLOW);
  			if(td->msg.train_status == 1)td->state = i1_State_11;
  			else td->state = i1_State_1;
  			break;
  		case i1_State_9://GREEN TURN SW

  			timer_funct(GREEN_TURN);
  			if(td->msg.train_status == 1)td->state = i1_State_19;
  			else td->state = i1_State_10;
  			break;
  		case i1_State_10://YELLOW TURN

  			timer_funct(YELLOW);
  			if(td->msg.train_status == 1)td->state = i1_State_19;
  			else td->state = i1_State_1;
  			break;


  		case i1_State_11:// ALL RED STATE

  			timer_funct(RED);
  			if(td->msg.train_status == 1){
				if(td->msg.ES == 1 && td->msg.WN == 0){
					td->state = i1_State_12;
				}
				else td->state = i1_State_3;
  			}
  			else if(td->msg.ES == 0 && td->msg.WN == 0)td->state = i1_State_3;
  			else if(td->msg.ES == 0 && td->msg.WN == 1)td->state = i1_State_16;
  			else if(td->msg.ES == 1 && td->msg.WN == 0)td->state = i1_State_12;
  			else if(td->msg.ES == 1 && td->msg.WN == 1)td->state = i1_State_14;
  			else td->state = i1_State_3;
  			td->msg.ES = 0;td->msg.WN = 0;td->msg.EW = 0;
  			break;

  		case i1_State_12://GREEN TURN ES

  			timer_funct(GREEN_TURN);
  			td->state = i1_State_13;
  			break;
  		case i1_State_13://YELLOW TURN

  			timer_funct(YELLOW);
  			td->state = i1_State_3;
  			break;
  		case i1_State_14://GREEN TURN ES & WN

  			timer_funct(GREEN_TURN);
  			td->state = i1_State_15;
  			break;
  		case i1_State_15://YELLOW TURN

  			timer_funct(YELLOW);
  			td->state = i1_State_3;
  			break;
  		case i1_State_16://GREEN TURN WN

  			timer_funct(GREEN_TURN);
  			td->state = i1_State_17;
  			break;
  		case i1_State_17://YELLOW TURN

  			timer_funct(YELLOW);
  			td->state = i1_State_3;
  			break;


  		case i1_State_18://YELLOW TURN-TO-ALL

  			timer_funct(YELLOW);
  			td->state = i1_State_11;
  			break;
  		case i1_State_19://YELLOW TURN-TO-ALL

  			timer_funct(YELLOW);
  			td->state = i1_State_11;
  			break;
  		default:
  			break;

  	}
	if(td->lock_status == 1){
  		if(td->state == i1_State_11){
			td->state = i1_State_0;
		}
	}

}
