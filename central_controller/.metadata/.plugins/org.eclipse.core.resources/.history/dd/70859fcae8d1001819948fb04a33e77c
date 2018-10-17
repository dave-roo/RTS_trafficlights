#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <hardware_include.h>

void* buton_thread(void* arg){
	keypad_data_t key, key_1;
	key = keypad_setup();
	if(key.err != KEYPAD_OK){
		printf("Initialisation failed\n");
	}else{
		printf("Keypad Setup Complete!\n");
	}
	while(1){
		volatile uint32_t val;
		key_1 = keypad_get_key(key);
		if(key_1.valid){
			val = key_1.key_pressed;
		}
		printf("Button %d pressed\n", val);
	}
}

int main(void) {
	pthread_t th1, th2;
	pthread_create(&th1, NULL, buton_thread, NULL);

	pthread_join(th1, NULL);

	return EXIT_SUCCESS;
}
