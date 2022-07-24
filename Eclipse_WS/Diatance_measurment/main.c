/*
*	Name:			main.c
 *	Author:			MAHMOUD EL-FEKY
 *	Description:	Distance Measurement System based on Ultrasonic Sensor "HC-SR04"
 *  Created on: 	JUN 2, 2022
 */

#include<avr/interrupt.h> 		/* to use sei function */
#include "lcd.h"				/*To use LCD driver*/
#include "ultrasonic.h"			/*To use ultrasonic driver*/
#include"util/delay.h"			/* to use delay functions */

void main(){
	/*Enable global interrupt bit */
	sei();
	/*Start LCD initialization function with Static configurations*/
	LCD_init();
	/*Start Ultrasonic initialization function with Static configurations*/
	Ultrasonic_init();
	/* Display this string "DISTANCE:     CM" at the first row */
	LCD_displayString("DISTANCE:     CM");
	/*Declare variable to hold the final distance value on it */
	uint16 final_diatance=0;
	/*start super loop*/
	while(1){

		/*get distance value from the ultrasonic sensor by using ICU and store it on final_diatance*/
		final_diatance= Ultrasonic_readDistance();

		/*move LCD cursor to the col 10 at the first row to write final_diatance on LCD */
		LCD_moveCursor(0,10);

		/*render final_diatance on LCD */
		LCD_intgerToString(final_diatance);
		if(final_diatance<100)
			LCD_displayCharacter(' ');

		/*to avoid multiple trigger interference*/
		_delay_ms(100);

	}

}

