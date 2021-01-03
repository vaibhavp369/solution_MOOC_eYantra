/*! \mainpage Week-3 Experiment-1: Interrupt Switch 
 *
 * @author     e-Yantra Team
 * @date       2020/02/27
 *
 * \subsection Aim
 * To turn on the Buzzer present on the Firebird-V when Interrupt Switch is pressed.
 *
 * \subsection Connections
 * Interrupt Switch				:	PE7 				<br>
 * Buzzer 						:	PC3					<br>
 */


//---------------------------------- HEADER FILES -----------------------------------------------------
#include "firebird_simulation.h"	// Header file included that contains macro definitions essential for Firebird V robot
#include <avr/interrupt.h>


//---------------------------------- FUNCTIONS ----------------------------------------------------------
// << TODO >> : Complete all the functions and their input arguments correctly as they might have errors

/**
 * @brief      Makes **ONLY** Buzzer pin as output and makes it LOW initially
 */
void buzzer_pin_config(void) {
	// << NOTE >> : Use Masking and Shift Operators here

	// Make **ONLY** Buzzer pin as output
	buzzer_ddr_reg |=(1<<buzzer_pin) ;

	// Turn **ONLY** the Buzzer pin LOW
	buzzer_port_reg &=~(1<<buzzer_pin) ;
}

/**
 * @brief      Makes **ONLY** Interrupt Switch pin as input and pulls it up internally
 */
void interrupt_sw_pin_config(void) {
	// << NOTE >> : Use Masking and Shift Operators here

	// Make **ONLY** Interrupt Switch pin as input
	interrupt_sw_ddr_reg &=~(1<<interrupt_sw_pin) ;

	// Make **ONLY** Interrupt Switch pin internally pull-up
	interrupt_sw_port_reg |=(1<<interrupt_sw_pin) ;
}

/**
 * @brief      Configures External Interrupt on the Interrupt Switch
 */
void interrupt_sw_config(void) {

	// all interrupts have to be disabled before configuring interrupts
	cli();	// Disables Interrupts Globally
	
	// Turn ON INT7 (alternative function of PE7 i.e Interrupt Switch Pin)
	EIMSK_reg =0x80 ;
	
	// Low level detection on INT7
	EICRB_reg =0x00;
	
	sei();	// Enable Interrupts Globally
}

void buzzer_on(void)
{
	buzzer_port_reg |=(1<<buzzer_pin) ;             // Turn **ONLY** the Buzzer pin HIGH
}

void buzzer_off(void)
{
	buzzer_port_reg &=~(1<<buzzer_pin) ;             // Turn **ONLY** the Buzzer pin LOW
}

/**
 * @brief      Interrupt Service Routine for INT7, which turns on the Buzzer when the Interrupt Switch is pressed
 */
ISR(interrupt_isr_vect) 

{ 
	  if(0x00 == (interrupt_sw_pin_reg & 0x80))
         {
		   buzzer_port_reg |=(1<<buzzer_pin);								// Turn on the Buzzer		   
		 }
}

//---------------------------------- MAIN ----------------------------------------------------------------
/**
 * @brief      Main Function
 *
 * @details    Initializes the Buzzer Pin and the Interrupt Switch Pin. Turns ON and configure the alternate function (INT7) for PE7.
 */
int main(void) {
	// << NOTE >> : You are not allowed to modify or change anything inside this function

	buzzer_pin_config();			// Make the Buzzer Pin and OUTPUT
	interrupt_sw_pin_config();		// Make the Interrupt Switch Pin as INPUT
	interrupt_sw_config();			// Turn ON and configure the Alternate Function (INT7) for PE7.
	while (1) {
	buzzer_off();					// Turn **ONLY** the Buzzer pin LOW
	}
}

