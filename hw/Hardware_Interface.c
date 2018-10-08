#include <stdlib.h>
#include <stdio.h>
#include <hw/inout.h>      // for in32() and out32();
#include <sys/mman.h>      // for mmap_device_io();
#include <stdint.h>        // for unit32 types
#include <sys/neutrino.h>  // for ThreadCtl( _NTO_TCTL_IO_PRIV , NULL)
#include <sched.h>
#include <sys/procmgr.h>
#include <stdint.h>
#include <fcntl.h>
#include <devctl.h>
#include <hw/i2c.h>
#include <errno.h>
#include <unistd.h>


#define AM335X_CONTROL_MODULE_BASE   (uint64_t) 0x44E10000
#define AM335X_CONTROL_MODULE_SIZE   (size_t)   0x00001448
#define AM335X_GPIO_SIZE             (uint64_t) 0x00001000
#define AM335X_GPIO1_BASE            (size_t)   0x4804C000

#define LED0          (1<<21)   // GPIO1_21
#define LED1          (1<<22)   // GPIO1_22
#define LED2          (1<<23)   // GPIO1_23
#define LED3          (1<<24)   // GPIO1_24

#define SD0 (1<<28)  // SD0 is connected to GPIO1_28
#define SCL (1<<16)  // SCL is connected to GPIO1_16

#define GPIO_OE        0x134
#define GPIO_DATAIN    0x138
#define GPIO_DATAOUT   0x13C

#define GPIO_IRQSTATUS_SET_1 0x38   // enable interrupt generation
#define GPIO_IRQWAKEN_1      0x48   // Wakeup Enable for Interrupt Line
#define GPIO_FALLINGDETECT   0x14C  // set falling edge trigger
#define GPIO_CLEARDATAOUT    0x190  // clear data out Register
#define GPIO_IRQSTATUS_1     0x30   // clear any prior IRQs

#define GPIO1_IRQ 99  // TRG page 465 list the IRQs for the am335x


#define P9_12_pinConfig 0x878 //  conf_gpmc_ben1 (TRM pp 1364) for GPIO1_28,  P9_12

// GPMC_A1_Configuration
#define PIN_MODE_0   0x00
#define PIN_MODE_1   0x01
#define PIN_MODE_2   0x02
#define PIN_MODE_3   0x03
#define PIN_MODE_4   0x04
#define PIN_MODE_5   0x05
#define PIN_MODE_6   0x06
#define PIN_MODE_7   0x07

// PIN MUX Configuration strut values  (page 1420 from TRM)
#define PU_ENABLE    0x00
#define PU_DISABLE   0x01
#define PU_PULL_UP   0x01
#define PU_PULL_DOWN 0x00
#define RECV_ENABLE  0x01
#define RECV_DISABLE 0x00
#define SLEW_FAST    0x00
#define SLEW_SLOW    0x01


#define DATA_SEND 0x40  // sets the Rs value high
#define Co_Ctrl   0x00  // mode to tell LCD we are sending a single command


// Function prototypes
int main(int argc, char *argv[]);
int  I2cWrite_(int fd, uint8_t Address, uint8_t mode, uint8_t *pBuffer, uint32_t NbData);
void SetCursor(int fd, uint8_t LCDi2cAdd, uint8_t row, uint8_t column);
void Initialise_LCD (int fd, _Uint32t LCDi2cAdd);

int synchronized = 1;
int screen = 0;
int button = 0;

typedef struct
{
	int fd;
	uint8_t Address;
	uint8_t mode;
	pthread_mutex_t mutex;
} LCD_connect;

void *LCDthread_A_ex (void *data)
{
	LCD_connect *td = (LCD_connect*) data;
	uint8_t	LCDdata1[21] = {};
	uint8_t	LCDdata2[21] = {};
	int i = 0;

	if(synchronized) pthread_mutex_lock(&td->mutex);     //lock the function to make sure the variables are protected

	if(screen == 1) {
		sprintf(LCDdata1,"Screen");
		sprintf(LCDdata2,"1");
	}
	else if(screen == 2) {
		sprintf(LCDdata1,"Screen");
		sprintf(LCDdata2,"2");
	}
	else{
		sprintf(LCDdata1,"--------------------");
		sprintf(LCDdata2,"--------------------");
	}

	SetCursor(td->fd, td->Address,0,0); // set cursor on LCD to first position first line
	I2cWrite_(td->fd, td->Address, DATA_SEND, &LCDdata1[0], sizeof(LCDdata1));		// write new data to I2C

	SetCursor(td->fd, td->Address,1,0); // set cursor on LCD to first position first line
	I2cWrite_(td->fd, td->Address, DATA_SEND, &LCDdata2[0], sizeof(LCDdata2));		// write new data to I2C

	if(synchronized) pthread_mutex_unlock(&td->mutex);
	usleep(1000000); // 1.0 seconds

	return 0;
}

void *update_screen()
{
	int file;
	int error;
	volatile uint8_t LCDi2cAdd = 0x3C;
	_Uint32t speed = 10000;
	LCD_connect td;

	uint8_t	LCDdata[21] = {};

	pthread_mutex_init(&td.mutex,NULL);

	if ((file = open("/dev/i2c1",O_RDWR)) < 0)	  // OPEN I2C1
		printf("Error while opening Device File.!!\n");
	else
		printf("I2C1 Opened Successfully\n");

	usleep(1);

	pthread_t  th1;
	pthread_mutex_lock(&td.mutex);
		td.fd     = file;
		td.Address= LCDi2cAdd;
	td.mode   = DATA_SEND;
    pthread_mutex_unlock(&td.mutex);
	pthread_create (&th1, NULL, LCDthread_A_ex, &td);

	pthread_join (th1, NULL);

	pthread_mutex_destroy(&td.mutex);

}

int main(int argc, char *argv[])
{

	initializeLCD();
	getKey();

	printf("\n complete");
	return EXIT_SUCCESS;
}

void initializeLCD(){
	int file;
	int error;
	volatile uint8_t LCDi2cAdd = 0x3C;
	_Uint32t speed = 10000; // nice and slow (will work with 200000)
	LCD_connect td;

	uint8_t	LCDdata[21] = {};

	// Create the mutex
	pthread_mutex_init(&td.mutex,NULL);		// pass NULL as the attr parameter to use the default attributes for the mutex

	// Open I2C resource and set it up
	if ((file = open("/dev/i2c1",O_RDWR)) < 0)	  // OPEN I2C1
		printf("Error while opening Device File.!!\n");
	else
		printf("I2C1 Opened Successfully\n");

	error = devctl(file,DCMD_I2C_SET_BUS_SPEED,&(speed),sizeof(speed),NULL);  // Set Bus speed
	if (error)
	{
		fprintf(stderr, "Error setting the bus speed: %d\n",strerror ( error ));
		exit(EXIT_FAILURE);
	}
	else
		printf("Bus speed set = %d\n", speed);

	pthread_create(NULL, NULL, update_screen, NULL);

	usleep(1);
}

// Writes to I2C
int  I2cWrite_(int fd, uint8_t Address, uint8_t mode, uint8_t *pBuffer, uint32_t NbData)
{
	i2c_send_t hdr;
    iov_t sv[2];
    int status, i;

    uint8_t LCDpacket[21] = {};  // limited to 21 characters  (1 control bit + 20 bytes)

    // set the mode for the write (control or data)
    LCDpacket[0] = mode;  // set the mode (data or control)

	// copy data to send to send buffer (after the mode bit)
	for (i=0;i<NbData+1;i++)
		LCDpacket[i+1] = *pBuffer++;

    hdr.slave.addr = Address;
    hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    hdr.len = NbData + 1;  // 1 extra for control (mode) bit
    hdr.stop = 1;

    SETIOV(&sv[0], &hdr, sizeof(hdr));
    SETIOV(&sv[1], &LCDpacket[0], NbData + 1); // 1 extra for control (mode) bit
      // int devctlv(int filedes, int dcmd,     int sparts, int rparts, const iov_t *sv, const iov_t *rv, int *dev_info_ptr);
    status = devctlv(fd, 		  DCMD_I2C_SEND, 2,          0,          sv,              NULL,           NULL);

    if (status != EOK)
    	printf("status = %s\n", strerror ( status ));

    return status;
}


void SetCursor(int fd, uint8_t LCDi2cAdd, uint8_t row, uint8_t column)
{
	uint8_t position = 0x80; // SET_DDRAM_CMD (control bit)
	uint8_t rowValue = 0;
	uint8_t	LCDcontrol = 0;
	if (row == 1)
		rowValue = 0x40;     // memory location offset for row 1
	position = (uint8_t)(position + rowValue + column);
	LCDcontrol = position;
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
}


void Initialise_LCD (int fd, _Uint32t LCDi2cAdd)
{
	uint8_t	LCDcontrol = 0x00;

	//   Initialise the LCD display via the I2C bus
	LCDcontrol = 0x38;  // data byte for FUNC_SET_TBL1
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x39; // data byte for FUNC_SET_TBL2
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x14; // data byte for Internal OSC frequency
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x79; // data byte for contrast setting
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x50; // data byte for Power/ICON control Contrast set
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x6C; // data byte for Follower control
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x0C; // data byte for Display ON
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x01; // data byte for Clear display
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
}


typedef union _CONF_MODULE_PIN_STRUCT   // See TRM Page 1420
{
  unsigned int d32;
  struct {    // name: field size
           unsigned int conf_mmode : 3;       // LSB
           unsigned int conf_puden : 1;
           unsigned int conf_putypesel : 1;
           unsigned int conf_rxactive : 1;
           unsigned int conf_slewctrl : 1;
           unsigned int conf_res_1 : 13;      // reserved
           unsigned int conf_res_2 : 12;      // reserved MSB
         } b;
} _CONF_MODULE_PIN;

void strobe_SCL(uintptr_t gpio_port_add) {
   uint32_t PortData;
   PortData = in32(gpio_port_add + GPIO_DATAOUT);// value that is currently on the GPIO port
   PortData &= ~(SCL);
   out32(gpio_port_add + GPIO_DATAOUT, PortData);// Clock low
   delaySCL();

   PortData  = in32(gpio_port_add + GPIO_DATAOUT);// get port value
   PortData |= SCL;// Clock high
   out32(gpio_port_add + GPIO_DATAOUT, PortData);
   delaySCL();
}

void delaySCL()  {// Small delay used to get timing correct for BBB
  volatile int i, a;
  for(i=0;i<0x1F;i++) // 0x1F results in a delay that sets F_SCL to ~480 kHz
  {   // i*1 is faster than i+1 (i+1 results in F_SCL ~454 kHz, whereas i*1 is the same as a=i)
     a = i;
  }
  // usleep(1);  //why doesn't this work? Ans: Results in a period of 4ms as
  // fastest time, which is 250Hz (This is to slow for the TTP229 chip as it
  // requires F_SCL to be between 1 kHz and 512 kHz)
}

uint32_t KeypadReadIObit(uintptr_t gpio_base, uint32_t BitsToRead)  {
   volatile uint32_t val = 0;
   val  = in32(gpio_base + GPIO_DATAIN);// value that is currently on the GPIO port

   val &= BitsToRead;
   if(val==BitsToRead)
	   return 1;
   else
	   return 0;
}

void DecodeKeyValue(uint32_t word)
{
	switch(word)
	{
		case 0x01:
			printf("Key  1 pressed\n");
			pthread_create(NULL, NULL, update_screen, NULL);
			button = 1;
			break;
		case 0x02:
			printf("Key  2 pressed\n");
			pthread_create(NULL, NULL, update_screen, NULL);
			button = 2;
			break;
		case 0x04:
			printf("Key  3 pressed\n");
			pthread_create(NULL, NULL, update_screen, NULL);
			button = 3;
			break;
		case 0x08:
			printf("Key  4 pressed\n");
			pthread_create(NULL, NULL, update_screen, NULL);
			break;
		case 0x10:
			printf("Key  5 pressed\n");
			pthread_create(NULL, NULL, update_screen, NULL);
			break;
		case 0x20:
			printf("Key  6 pressed\n");
			pthread_create(NULL, NULL, update_screen, NULL);
			break;
		case 0x40:
			printf("Key  7 pressed\n");
			pthread_create(NULL, NULL, update_screen, NULL);
			break;
		case 0x80:
			printf("Key  8 pressed\n");
			pthread_create(NULL, NULL, update_screen, NULL);
			break;
		case 0x100:
			printf("Key  9 pressed\n");
			pthread_create(NULL, NULL, update_screen, NULL);
			break;
		case 0x200:
			printf("Key 10 pressed\n");
			pthread_create(NULL, NULL, update_screen, NULL);
			break;
		case 0x400:
			printf("Key 11 pressed\n");
			pthread_create(NULL, NULL, update_screen, NULL);
			break;
		case 0x800:
			printf("Key 12 pressed\n");
			pthread_create(NULL, NULL, update_screen, NULL);
			break;
		case 0x1000:
			printf("Key 13 pressed\n");
			pthread_create(NULL, NULL, update_screen, NULL);
			break;
		case 0x2000:
			printf("Key 14 pressed\n");
			screen = 1;
			pthread_create(NULL, NULL, update_screen, NULL);
			break;
		case 0x4000:
			printf("Key 15 pressed\n");
			screen = 2;
			pthread_create(NULL, NULL, update_screen, NULL); // flash LED
			button = 15;
			break;
		case 0x8000:
			printf("Key 16 pressed\n");
			break;
		case 0x00:  // key release event (do nothing)
			break;
		default:
			printf("Key pressed could not be determined - %lu\n", word);
	}
}
/*
 *
 *
 * Here is the ISR
 *
 *
 *
 */
typedef struct
{
	int count_thread;
	uintptr_t gpio1_base;
	struct sigevent pevent; // remember to fill in "event" structure in main
}ISR_data;

// create global struct to share data between threads
ISR_data ISR_area_data;

const struct sigevent* Inthandler( void* area, int id )
{
	// 	"Do not call any functions in ISR that call kernerl - including printf()
	//struct sigevent *pevent = (struct sigevent *) area;
	ISR_data *p_ISR_data = (ISR_data *) area;

	InterruptMask(GPIO1_IRQ, id);  // Disable all hardware interrupt

	// must do this in the ISR  (else stack over flow and system will crash
	out32(p_ISR_data->gpio1_base + GPIO_IRQSTATUS_1, SD0); //clear IRQ

	// do this to tell us how many times this handler gets called
	p_ISR_data->count_thread++;
	// got IRQ.
	// work out what it came from

    InterruptUnmask(GPIO1_IRQ, id);  // Enable a hardware interrupt

    // return a pointer to an event structure (preinitialized
    // by main) that contains SIGEV_INTR as its notification type.
    // This causes the InterruptWait in "int_thread" to unblock.
	return (&p_ISR_data->pevent);
}


void getKey(){

	  uintptr_t control_module = mmap_device_io(AM335X_CONTROL_MODULE_SIZE,
	                                                               AM335X_CONTROL_MODULE_BASE);
	  uintptr_t gpio1_base = mmap_device_io(AM335X_GPIO_SIZE          , AM335X_GPIO1_BASE);

	  // initalise the global stuct
	  ISR_area_data.count_thread = 0;
	  ISR_area_data.gpio1_base = gpio1_base;

	  memset(&ISR_area_data.pevent, 0, sizeof(ISR_area_data.pevent));
	  SIGEV_INTR_INIT (&ISR_area_data.pevent);
	  ISR_area_data.pevent.sigev_notify = SIGEV_INTR;  // Setup for external interrupt

		// we also need to have the PROCMGR_AID_INTERRUPT and PROCMGR_AID_IO abilities enabled. For more information, see procmgr_ability().
	  ThreadCtl( _NTO_TCTL_IO_PRIV , 1);// Request I/O privileges  for QNX7;

	  procmgr_ability( 0, PROCMGR_AID_INTERRUPT | PROCMGR_AID_IO);


	  if( (control_module)&&(gpio1_base) )
	  {

	    volatile uint32_t val = 0;

	    // set DDR for LEDs to output and GPIO_28 to input
	    val = in32(gpio1_base + GPIO_OE); // read in current setup for GPIO1 port
	    val |= 1<<28;                     // set IO_BIT_28 high (1=input, 0=output)
	    out32(gpio1_base + GPIO_OE, val); // write value to input enable for data pins
	    val &= ~(LED0|LED1|LED2|LED3);    // write value to output enable
	    out32(gpio1_base + GPIO_OE, val); // write value to output enable for LED pins

	    val = in32(gpio1_base + GPIO_OE);
	    val &= ~SCL;                      // 0 for output
	    out32(gpio1_base + GPIO_OE, val); // write value to output enable for data pins


	    val = in32(gpio1_base + GPIO_DATAOUT);
	    val |= SCL;              // Set Clock Line High as per TTP229-BSF datasheet
	    out32(gpio1_base + GPIO_DATAOUT, val); // for 16-Key active-Low timing diagram


	    in32s(&val, 1, control_module + P9_12_pinConfig );
	    printf("Original pinmux configuration for GPIO1_28 = %#010x\n", val);

	    // set up pin mux for the pins we are going to use  (see page 1354 of TRM)
	    volatile _CONF_MODULE_PIN pinConfigGPMC; // Pin configuration strut
	    pinConfigGPMC.d32 = 0;
	    // Pin MUX register default setup for input (GPIO input, disable pull up/down - Mode 7)
	    pinConfigGPMC.b.conf_slewctrl = SLEW_SLOW;    // Select between faster or slower slew rate
	    pinConfigGPMC.b.conf_rxactive = RECV_ENABLE;  // Input enable value for the PAD
	    pinConfigGPMC.b.conf_putypesel= PU_PULL_UP;   // Pad pullup/pulldown type selection
	    pinConfigGPMC.b.conf_puden = PU_ENABLE;       // Pad pullup/pulldown enable
	    pinConfigGPMC.b.conf_mmode = PIN_MODE_7;      // Pad functional signal mux select 0 - 7

	    // Write to PinMux registers for the GPIO1_28
	    out32(control_module + P9_12_pinConfig, pinConfigGPMC.d32);
	    in32s(&val, 1, control_module + P9_12_pinConfig);   // Read it back
	    printf("New configuration register for GPIO1_28 = %#010x\n", val);

	    // Setup IRQ for SD0 pin ( see TRM page 4871 for register list)
	    out32(gpio1_base + GPIO_IRQSTATUS_SET_1, SD0);	// Write 1 to GPIO_IRQSTATUS_SET_1
	    out32(gpio1_base + GPIO_IRQWAKEN_1, SD0);    	// Write 1 to GPIO_IRQWAKEN_1
	    out32(gpio1_base + GPIO_FALLINGDETECT, SD0);    // set falling edge
	    out32(gpio1_base + GPIO_CLEARDATAOUT, SD0);     // clear GPIO_CLEARDATAOUT
	    out32(gpio1_base + GPIO_IRQSTATUS_1, SD0);      // clear any prior IRQs



	    int id = 0; // Attach interrupt Event to IRQ for GPIO1B  (upper 16 bits of port)


	    // Main code starts here
	    //The thread that calls InterruptWait() must be the one that called InterruptAttach().
	//    id = InterruptAttach (GPIO1_IRQ, Inthandler, &ISR_area_data, sizeof(ISR_area_data), _NTO_INTR_FLAGS_TRK_MSK | _NTO_INTR_FLAGS_NO_UNMASK | _NTO_INTR_FLAGS_END);
	    id = InterruptAttach (GPIO1_IRQ, Inthandler, &ISR_area_data, sizeof(ISR_area_data), _NTO_INTR_FLAGS_TRK_MSK );

	    InterruptUnmask(GPIO1_IRQ, id);  // Enable a hardware interrupt

	    int i = 0;
	    printf( "Main entering loop and will call InterruptWait\n");
	    for(;;)
	    {
	    	// Block main until we get a sigevent of type: 	ISR_area_data.pevent
	        InterruptWait( 0, NULL );   // block this thread until an interrupt occurs  (Wait for a hardware interrupt)

	        // printf("do interrupt work here...\n");

			volatile uint32_t word = 0;
			//  confirm that SD0 is still low (that is a valid Key press event has occurred)
			val = KeypadReadIObit(gpio1_base, SD0);  // read SD0 (means data is ready)

			if(val == 0)  // start reading key value form the keypad
			{
				 word = 0;  // clear word variable

				 delaySCL(); // wait a short period of time before reading the data Tw  (10 us)

				 for(i=0;i<16;i++)           // get data from SD0 (16 bits)
				 {
					strobe_SCL(gpio1_base);  // strobe the SCL line so we can read in data bit

					val = KeypadReadIObit(gpio1_base, SD0); // read in data bit
					val = ~val & 0x01;                      // invert bit and mask out everything but the LSB
					//printf("val[%u]=%u, ",i, val);
					word = word | (val<<i);  // add data bit to word in unique position (build word up bit by bit)
				 }
				 //printf("word=%u\n",word);
				 DecodeKeyValue(word);

				 printf("Interrupt count = %i\n", ISR_area_data.count_thread);
			}


	        //sched_yield();
	    }

	     munmap_device_io(control_module, AM335X_CONTROL_MODULE_SIZE);
	   }

}


