#include "stateStrings.h"


char* toString(state_data * td){
	switch(td->state){
	  		case i1_State_0:
	  			return("All Red           ");
	  			break;
	  		case i1_State_1:
	  			return("North & South Green");
				break;
	  		case i1_State_2:
	  			return("North & South Yellow");
				break;
	  		case i1_State_3:
	  			return("East & West Green");
				break;
	  		case i1_State_4:
	  			return("East & West Yellow");
				break;
	  		case i1_State_5:
				return("North & N-Turning");
				break;
			case i1_State_6:
				return("N-Turning Yellow");
				break;
			case i1_State_7:
				return("N & S Turning");
				break;
			case i1_State_8:
				return("N & S Yellow");
				break;
			case i1_State_9:
				return("South & S-Turning");
				break;
			case i1_State_10:
				return("S-Turning Yellow");
				break;
			case i1_State_11:
				return("All Red           ");
				break;
			case i1_State_12:
				return("East & E-Turning");
				break;
			case i1_State_13:
				return("E-Turning Yellow");
				break;
			case i1_State_14:
				return("E & W Turning");
				break;
			case i1_State_15:
				return("E & W Yellow");
				break;
			case i1_State_16:
				return("West & W-Turning");
				break;
			case i1_State_17:
				return("W-Turning Yellow");
				break;
			case i1_State_18:
				return("North Yellow");
				break;
			case i1_State_19:
				return("South Yellow");
				break;
			default:
				return ("");
				break;
	}
}
