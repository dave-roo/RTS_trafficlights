#include "hardware_include.h"
volatile uint8_t LCDi2cAdd = 0x3C;

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

   val &= BitsToRead; // mask bit
   //val = val >> (BitsToRead % 2);
   //return val;
   if(val==BitsToRead)
	   return 1;
   else
	   return 0;
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

    if (status != EOK){
	//    	printf("status = %s\n", strerror ( status ));
    }


    return status;
}

// Function to set cursor
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

// Initialise the LCD display
lcd_data_t lcd_setup(){
	lcd_data_t ret;

	int file;
	int error;
//	volatile uint8_t LCDi2cAdd = 0x3C;
	_Uint32t speed = 10000; // nice and slow (will work with 200000)

//	uint8_t	LCDdata[21] = {};

	// Open I2C resource and set it up
	if ((file = open("/dev/i2c1",O_RDWR)) < 0){	  // OPEN I2C1
		printf("Error while opening Device File.!!\n");
		ret.err = LCD_INIT_ERROR;
		return ret;
	}else
		printf("I2C1 Opened Successfully\n");

	error = devctl(file,DCMD_I2C_SET_BUS_SPEED,&(speed),sizeof(speed),NULL);  // Set Bus speed
	if (error)
	{
//		fprintf(stderr, "Error setting the bus speed: %d\n",strerror ( error ));
		exit(EXIT_FAILURE);
	}
	else
		printf("Bus speed set = %d\n", speed);

    uint8_t	LCDcontrol = 0x00;
//
	//   Initialise the LCD display via the I2C bus
	LCDcontrol = 0x38;  // data byte for FUNC_SET_TBL1
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x39; // data byte for FUNC_SET_TBL2
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x14; // data byte for Internal OSC frequency
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x79; // data byte for contrast setting
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x50; // data byte for Power/ICON control Contrast set
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x6C; // data byte for Follower control
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x0C; // data byte for Display ON
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x01; // data byte for Clear display
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	ret.file = file;
	ret.err = LCD_OK;
	return ret;
}

// Write two lines of data to the screen
lcd_error lcd_write_to_screen(lcd_data_t data, char* line_1, char* line_2){

	uint8_t	LCDdata[21] = {};

	int file = data.file;
	// Write line 1 to the screen
	SetCursor(file, LCDi2cAdd,0,0); // set cursor on LCD to first position first line
	strcpy(LCDdata, line_1);
	I2cWrite_(file, LCDi2cAdd, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C

	// Write line 2 to the screen
	SetCursor(file, LCDi2cAdd,1,0); // set cursor on LCD to first position second line
	strcpy(LCDdata,line_2);
	I2cWrite_(file, LCDi2cAdd, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C

	return LCD_OK;
}

// Setup the keypad
keypad_data_t keypad_setup(){
	keypad_data_t ret;
	uintptr_t control_module = mmap_device_io(AM335X_CONTROL_MODULE_SIZE,AM335X_CONTROL_MODULE_BASE);
	uintptr_t gpio1_base     = mmap_device_io(AM335X_GPIO_SIZE, AM335X_GPIO1_BASE);


	  if( (control_module)&&(gpio1_base) )
	  {
	    ThreadCtl( _NTO_TCTL_IO_PRIV , NULL);// Request I/O privileges;

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
	    out32(gpio1_base + GPIO_IRQSTATUS_SET_1, SD0);// Write 1 to GPIO_IRQSTATUS_SET_1
	    out32(gpio1_base + GPIO_IRQWAKEN_1, SD0);    // Write 1 to GPIO_IRQWAKEN_1
	    out32(gpio1_base + GPIO_FALLINGDETECT, SD0);    // set falling edge
	    out32(gpio1_base + GPIO_CLEARDATAOUT, SD0);     // clear GPIO_CLEARDATAOUT
	    out32(gpio1_base + GPIO_IRQSTATUS_1, SD0);      // clear any prior IRQs

	    struct sigevent event; // fill in "event" structure
	    memset(&event, 0, sizeof(event));
	    event.sigev_notify = SIGEV_INTR;  // Setup for external interrupt

	    int id = 0; // Attach interrupt Event to IRQ for GPIO1B  (upper 16 bits of port)
	    id = InterruptAttachEvent (GPIO1_IRQ, &event, _NTO_INTR_FLAGS_TRK_MSK);
//	    printf("%d\n", id);

	    // Main code starts here
	    printf( "Waiting For Interrupt 99 - key press on Jaycar (XC4602) keypad\n");
//	    int i = 0;
//	    for(;;)   // for loop that correctly decodes key press
//	    {
//	        InterruptWait( 0, NULL );   // block this thread until an interrupt occurs
//	        InterruptDisable();
//
//			volatile uint32_t word = 0;
//			//  confirm that SD0 is still low (that is a valid Key press event has occurred)
//			val = KeypadReadIObit(gpio1_base, SD0);  // read SD0 (means data is ready)
//
//			if(val == 0)  // start reading key value form the keypad
//			{
//				 word = 0;  // clear word variable
//
//				 delaySCL(); // wait a short period of time before reading the data Tw  (10 us)
//
//				 for(i=0;i<16;i++)           // get data from SD0 (16 bits)
//				 {
//					strobe_SCL(gpio1_base);  // strobe the SCL line so we can read in data bit
//
//					val = KeypadReadIObit(gpio1_base, SD0); // read in data bit
//					val = ~val & 0x01;                      // invert bit and mask out everything but the LSB
//					//printf("val[%u]=%u, ",i, val);
//					word = word | (val<<i);  // add data bit to word in unique position (build word up bit by bit)
//				 }
//				 //printf("word=%u\n",word);
////				 DecodeKeyValue(word);
//				 printf("Button %d\n", word);
//			}
//		    out32(gpio1_base + GPIO_IRQSTATUS_1, SD0); //clear IRQ
//		    InterruptUnmask(GPIO1_IRQ, id);
//		    InterruptEnable();
//	    }
//
//
//
//	     munmap_device_io(control_module, AM335X_CONTROL_MODULE_SIZE);


			ret.id = id;
			ret.control_module = control_module;
			ret.gpio1_base = gpio1_base;
			ret.err = KEYPAD_OK;
			ret.val = val;
	   }else{
		   ret.err = KEYPAD_INIT_ERROR;
	   }
	return ret;
}

// Blocking call to get the current keypad value
keypad_data_t keypad_get_key(keypad_data_t key){

    InterruptWait( 0, NULL );   // block this thread until an interrupt occurs
    InterruptDisable();

	volatile uint32_t word = 0;
	//  confirm that SD0 is still low (that is a valid Key press event has occurred)
	key.val = KeypadReadIObit(key.gpio1_base, SD0);  // read SD0 (means data is ready)

	if(key.val == 0)  // start reading key value form the keypad
	{
		key.valid = 1;
		 word = 0;  // clear word variable

		 delaySCL(); // wait a short period of time before reading the data Tw  (10 us)

		 for(int i=0;i<16;i++)           // get data from SD0 (16 bits)
		 {
			strobe_SCL(key.gpio1_base);  // strobe the SCL line so we can read in data bit

			key.val = KeypadReadIObit(key.gpio1_base, SD0); // read in data bit
			key.val = ~key.val & 0x01;                      // invert bit and mask out everything but the LSB
			//printf("val[%u]=%u, ",i, val);
			word = word | (key.val<<i);  // add data bit to word in unique position (build word up bit by bit)
		 }
		 key.key_pressed = word;
//		 printf("Button %d\n", word);
	}else{
		key.valid = 0;
	}
    out32(key.gpio1_base + GPIO_IRQSTATUS_1, SD0); //clear IRQ
    InterruptUnmask(GPIO1_IRQ, key.id);
    InterruptEnable();

	return key;
}
