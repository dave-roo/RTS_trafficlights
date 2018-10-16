#include <stdio.h>
#include <stdlib.h>
#include <hardware_include.h>

int main(void) {
	// lcd and keypad structs
	lcd_data_t lcd;
	keypad_data_t key;

	// LCD Setup function
	lcd = lcd_setup();
	if(lcd.err != LCD_OK){
		printf("Failed to initialise LCD\n");
		return EXIT_FAILURE;
	}else{
		printf("LCD Setup complete\n");
	}

	// This function writes to line 1 and 2 of the screen
	lcd_write_to_screen(lcd, "Hello", "World");

	// Keypad Setup function
	key = keypad_setup();
	if(key.err != KEYPAD_OK){
		printf("Initialisation failed\n");
	}else{
		printf("Keypad Setup Complete!\n");
	}

	// Read the struct from the keypad_get_key function and determine if valid
	// could be improved but not enough time
	while(1){
		keypad_data_t data;
		data = keypad_get_key(key);
		if(data.valid){
			printf("Button Value:%d\n", data.key_pressed);
		}

	}

	// Unmap this device when main is terminated
	munmap_device_io(key.control_module, AM335X_CONTROL_MODULE_SIZE);
	return EXIT_SUCCESS;
}
