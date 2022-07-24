/*
 *
 *  Module: ULTRASONIC
 *
 * File Name: ultrasonic.c
 *
 * Description: Source file for the ultrasonic driver
 *
 * Created on: Jun 2, 2022
 *
 * Author: MAHMOUD EL-FEKY
 */

#include "ultrasonic.h"
#include "gpio.h"
#include "icu.h"
#include "util/delay.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the value of total time required from sending wave until getting it */
uint16 g_echo_time=0;

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/


void Ultrasonic_init(void){
	/*
	 * define a structure variable for dynamic configurations of the ICU driver*/
	Icu_ConfigType s_Icu_Cofig ={F_CPU_8,RISING};
	/*Initialize the ICU driver as required*/
	ICU_init(&s_Icu_Cofig);
	/*Setup the ICU call back function which will be called as soon as inturrupt is raised*/
	ICU_setCallBack(Ultrasonic_edgeProcessing);
	/*
	 * Setup the direction for the trigger pin of ultrasonic as output pin through the GPIO driver.
	 * output low to it in order to disable all processes now*/
	GPIO_setupPinDirection(ULTRASONIC_TRIG_PORT_ID,ULTRASONIC_TRIG_PIN_ID,PIN_OUTPUT);
	GPIO_writePin(ULTRASONIC_TRIG_PORT_ID,ULTRASONIC_TRIG_PIN_ID,LOGIC_LOW);
}

void Ultrasonic_Trigger(void){
	/*
	 * Send the Trigger pulse to the ultrasonic.
	 * after this action HC-SR04 automatically sends eight 40 kHz sound waves
	 * and the microcontroller waits for rising edge output at the Echo pin.
	 */
	GPIO_writePin(ULTRASONIC_TRIG_PORT_ID,ULTRASONIC_TRIG_PIN_ID,LOGIC_HIGH);
	_delay_us(ULTRASONIC_TRIG_DELAY);
	GPIO_writePin(ULTRASONIC_TRIG_PORT_ID,ULTRASONIC_TRIG_PIN_ID,LOGIC_LOW);
}


uint16 Ultrasonic_readDistance(void){
	/* Send the trigger pulse to ultrasonic trigger pin by using Ultrasonic_Trigger function.
	 * At this this moment the measurements is started by the ICU .
	 */
	Ultrasonic_Trigger();
	/*here we Return The measured distance in Centimeter by using g_echo_time
	 which is edited in Ultrasonic_edgeProcessing function that is executed by the ISR of the ICU*/
	/*this form of value is resulted of these mathematical steps:-
	 * Sound velocity = 340.00 m/s = 34000 cm/s
	 * The distance of Object (in cm) = (343000 *Time)/2 = 17150 * Time
	 * Then time to execute 1 instruction is 1 us. as we use AVR with 8 MHZ and prescaler F_CPU/8
	 * so The distance of Object (in cm) = 17150 x (TIMER value) x 1 x 10^-6 cm
	 * 									 = 0.0174 x (TIMER value) cm
	 * 									 = (TIMER value) / 57.4 cm
	 *Note TIMER VALUE is the pulse width time calculated by the ICU.*/
	return (g_echo_time *.0174);

}


void Ultrasonic_edgeProcessing(void){
	static uint8 counter=0;
	/*when the counter=0 that is mean this is the first time we enter ISR and reflect that ICU find rising edge*/
	if(counter==0){
		/*When the rising edge capture occurs at the Echo pin which is connected to an input of ATmega16,
		 *clear Timer of ATmega16 and change edge detection on ICU to wait for a falling edge on the Echo pin.*/
		ICU_clearTimerValue();
		ICU_setEdgeDetectionType(FALLING);
		++counter;
	}
	/*when the counter=1 that is mean this is the second time we enter ISR and reflect that ICU find falling edge*/
	else if (counter==1){
		/*save the value of the ICU register on g_echo_time
		 * and change edge detection on ICU to wait for a rising edge on the Echo pin
		 * then clear value of counter variable to start from beginning again for new distance measuring.*/
		g_echo_time=ICU_getInputCaptureValue();
		ICU_setEdgeDetectionType(RISING);
		counter=0;
	}
}

