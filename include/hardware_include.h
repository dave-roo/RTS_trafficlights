#ifndef _HARDWARE_INCLUDE_H_
#define _HARDWARE_INCLUDE_H_
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <hw/inout.h>      // for in32() and out32();
#include <sys/mman.h>      // for mmap_device_io();
#include <sys/neutrino.h>  // for ThreadCtl( _NTO_TCTL_IO_PRIV , NULL)
#include <sched.h>
#include <sys/procmgr.h>
#include <fcntl.h>
#include <devctl.h>
#include <hw/i2c.h>
#include <errno.h>
#include <unistd.h>

// Defines from example project
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

// Enum for LCD Errors
typedef enum{
    LCD_OK,
    LCD_INIT_ERROR,
    LCD_UNKNOWN_ERROR    
} lcd_error;

// Enum for Keypad error types
typedef enum{
    KEYPAD_OK,
    KEYPAD_INIT_ERROR,
    KEYPAD_UNKNOWN_ERROR
} keypad_error;

// LCD Struct
typedef struct{
	lcd_error err;
	int file;
} lcd_data_t;

// Keypad struct
typedef struct{
    keypad_error err;
    uint32_t key_pressed;
    uintptr_t control_module;
    uintptr_t gpio1_base;
    int id;
    volatile uint32_t val;
    uint8_t valid;
} keypad_data_t;

// Function prototypes
lcd_data_t lcd_setup();
lcd_error lcd_write_to_screen(lcd_data_t data, char* line_1, char* line_2);

keypad_data_t keypad_setup();
keypad_data_t keypad_get_key(keypad_data_t);


#endif
