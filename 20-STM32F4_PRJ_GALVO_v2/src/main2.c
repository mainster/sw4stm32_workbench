/**
 * @file        main.c
 *
 * @date        24 Jan 2017
 * @author      Manuel Del Basso (mainster)
 * @email       manuel.delbasso@gmail.com
 *
 * @ide         System Workbench ac6 (eclipse stm32)
 * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 *
 * @brief       Extended "Hello World" example.
 *
 * Focuses the include strategy of toplevel user library files (e.g. "md_stm32f4_disco.h")
 */
#include "stm32f4xx.h"
#include "stm324x9i_eval.h"
#include "md_stm32f4_disco.h"


/**
 * @brief   briefTypeDescripton
 *
 * DetailedExplanation.
 * enclosing_method_arguments
 *
 */
typedef struct test_t {
	int member1;
	char *member2;
} test;

/**
 * @brief   briefTypeDescripton
 *
 * DetailedExplanation.
 * enclosing_method_arguments
 *
 */
int main(int a, char **arg) {
	/* Initialize System */
	SystemInit();

	/* Initialize leds on board */
	MD_DISCO_LedInit();

	/* Initialize button on board */
	MD_DISCO_ButtonInit();

    while(1) {
		/* If button pressed, do stuff all the time button is pressed */
    	if (MD_DISCO_ButtonPressed()) {
			/* Turn on leds */
    		MD_DISCO_LedOn(LED_RED | LED_GREEN);
    	} else {
			/* Turn off leds */
    		MD_DISCO_LedOff(LED_RED | LED_GREEN);
    	}

		/* Do the stuff only once when button is pressed */
		if (MD_DISCO_ButtonOnPressed()) {
			/* Do here stuff only once */
			/* This function will return 0 until you release button and press it again */
			/* For example, you can send data here to USART, but only once when button is pressed */
			GPIOD->BSRRL = LED_BLUE;
		}

		/* Do the stuff only once when button is released */
		if (MD_DISCO_ButtonOnReleased()) {
			/* DO here stuff only once */
			/* This function will return 0 until you press button and release it again */
			GPIOD->BSRRH = LED_BLUE;
		}
    }
}
