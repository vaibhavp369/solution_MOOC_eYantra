/*! \mainpage Week-2 Experiment-2: ADC Interfacing with Interrupt
 *
 * @author     e-Yantra Team
 * @date       2020/01/31
 *
 * \subsection Aim
 * To get the 10-bit ADC result from the 2nd Sharp Sensor in Single Conversion Mode using ADC conversion Interrupt.
 * Convert the ADC data from this sensor to its corresponding voltage w.r.t. AVCC, i.e. 5V and
 * display and send the ADC data and the corresponding voltage on LCD and UART Serial Terminal.
 *
 * \subsection Connections
 * 2nd Sharp Sensor					:  PK2 (ADC Channel 10) 			<br>
 *
 */


//---------------------------------- HEADER FILES -----------------------------------------------------
#include "firebird_simulation.h"		// Header file included that contains macro definitions essential for Firebird V robot
#include <stdbool.h>					// Standard C Library for Boolean Type
#include <util/delay.h>					// Standard AVR Delay Library
#include <avr/interrupt.h>				// Standard AVR Interrupt Library
#include <stdio.h>						// Standard C Library for standard input output
#include "lcd.h"						// LCD Header file included that contains function definitions essential to deal with LCD
#include "uart.h"						// UART Header file included that contains function definitions essential to deal with UART


//------------------------------------- MACROS ----------------------------------------------------------
// If any
#define ADC_TO_VOLT 0.00488758553275 
//---------------------------------- GLOBAL VARIABLES ---------------------------------------------------
// If any
unsigned int adc_10bit_data;						// To store 10-bit ADC converted data of a sensor

//---------------------------------- FUNCTIONS ----------------------------------------------------------
// << TODO >> : Complete all the functions as per the instructions given in form of comments


/**
 * @brief      Makes **ONLY** 2nd Sharp sensor pin as input and dectivates pull-up for **ONLY** 2nd Sharp sensor pin
 */
void sharp_sensor_pin_config(){
	// << NOTE >> : Use Masking and Shift Operators here
	
	// Make **ONLY** 2nd Sharp sensor pin as input
	sharp_sensor_ddr_reg	&=~(1<<sharp_sensor_pin);
	
	// Deactivate pull-up for **ONLY** 2nd Sharp sensor pin
	sharp_sensor_port_reg	&=~(1<<sharp_sensor_pin);
}


/**
 * @brief      Initializes the Analog-to-Digital converter inside the micro-controller
 */
void adc_init(){
	// << NOTE >> : Use Masking and Shift Operators here
	
	// Clear the Global Interrupt Flag bit in SREG
	cli();
	
	// In ADCSRA, enable ADC, ADC Interrupt and pre-scalar = 64 
	//				and clear ADC start conversion bit, auto trigger enable bit, interrupt flag bit and interrupt enable bit
	ADCSRA_reg	|=((1<<ADEN_bit)|(1<<ADPS2_bit)|(1<<ADPS1_bit)|(1<<ADIE_bit));
	ADCSRA_reg  &=~((1<<ADSC_bit)|(1<<ADATE_bit)|(1<<ADIF_bit));
	
	// In ADCSRB, disable Analog Comparator Multiplexer, MUX5 bit and ADC Auto Trigger Source bits
	ADCSRB_reg	&=~((1<<ACME_bit)|(1<<MUX5_bit)|(1<<ADTS2_bit)|(ADTS1_bit)|(ADTS0_bit));
	
	// In ADMUX, set the Reference Selection bits to use the AVCC as reference, and clear the channel selection bits MUX[4:0] initially
	ADMUX_reg	= 0x40 ;
	
	// In ADMUX, disable or enable the ADLAR bit to read 10-bit ADC result based on your logic
	ADMUX_reg	&=~(1<<ADLAR_bit) ;
	
	// In ACSR, disable the Analog Comparator by writing 1 to ACD_bit
	ACSR_reg	|=(1<<ACD_bit) ;
	
	// Set the Global Interrupt Flag bit in SREG
	
	sei();
	
}


/**
 * @brief      Sets the MUX[5:0] bits according to the sensor's channel number as input
 */
void select_adc_channel( unsigned char channel_num ){
	// << NOTE >> : Use Masking and Shift Operators here
	
	// set the MUX[5:0] bits to select the ADC channel number
	if(channel_num >=0 && channel_num <=7)
	{
		ADCSRB_reg &=~(1<<MUX5_bit);
		ADMUX_reg = channel_num;
		ADMUX_reg &=0x07;
	}
	
	else if(channel_num >=8 && channel_num <=15)
	{
		ADCSRB_reg |=(1<<MUX5_bit);
		ADMUX_reg = channel_num;
		ADMUX_reg &=0x07;
	}
}


/**
 * @brief      Starts the ADC by setting the ADSC bit in ADCSRA register
 */
void start_adc(void){
	// << NOTE >> : Use Masking and Shift Operators here
	
	// set the ADSC bit in ADCSRA register
	ADCSRA_reg |= (1<<ADSC_bit);
}


/**
 * @brief      Reset ADC config registers, ADCSRB and ADMUX registers
 */
void reset_adc_config_registers(void){
	// << NOTE >> : Use Masking and Shift Operators here

	// clear the MUX5 bit for next conversion
	ADCSRB_reg = 0x00;

	// clear the MUX[4:0] bits for next conversion
	ADMUX_reg = 0x00;
}


// read the ADC data after the conversion is complete
ISR( ADC_vect ){
					  
						
	  adc_10bit_data |= ADCH_reg;
	  adc_10bit_data = (adc_10bit_data<<8);
	  adc_10bit_data |= ADCL_reg;					 
}



/**
 * @brief      Convert the analog readings to 10-bit digital format from the sensor's ADC channel number as input
 */
void convert_analog_channel_data( unsigned char sensor_channel_number ){
	
	// << NOTE >> : You are not allowed to modify or change anything inside this function
	
	select_adc_channel( sensor_channel_number );
	
	start_adc();
}


//---------------------------------- MAIN ----------------------------------------------------------------
/**
 * @brief      Main Function
 *
 * @details    First Initializes the 2nd Sharp sensor and sends its ADC converted data on LCD and UART
 */
int main(void) {
	
	// << NOTE >> : You are not allowed to modify or change anything inside this function except a part of while loop
	
	sharp_sensor_pin_config();			// Initialize the 2nd Sharp sensor
	
	adc_init();							// Initialize the ADC
	
	lcd_port_config();					// Initialize the LCD port
	lcd_init();							// Initialize the LCD
	
	uart_init(UBRR_VALUE);				// Initialize the UART
	
	// To store 10-bit data of 2nd Sharp sensor
	int sharp_sensor_data;
	
	// To store the voltage reading corresponding to the ADC converted data from 2nd Sharp sensor
	float adc_data_voltage_reading ;
	
	// To create string for transmitting 2nd Sharp sensor raw data and corresponding voltage over the UART and LCD
	char tx_raw_adc_data_buffer[25], tx_voltage_buffer[25], lcd_print_raw_adc_data_string[25], lcd_print_voltage_string[25];
	
	convert_analog_channel_data( sharp_sensor_channel );
	
	while(1)
	{
		if ( ( ADCSRA_reg & ( 1 << ADSC_bit ) ) != 0x40 )
		{
			// ------------------------------- //

			sharp_sensor_data = adc_10bit_data;

			adc_data_voltage_reading = ADC_TO_VOLT*sharp_sensor_data ;

			// ------------------------------- //
			
			sprintf(lcd_print_raw_adc_data_string, "Raw data: %04d", sharp_sensor_data);
			lcd_string(1, 1, lcd_print_raw_adc_data_string);
			
			sprintf(tx_raw_adc_data_buffer, "Raw data: %04d\t", sharp_sensor_data);
			uart_tx_string(tx_raw_adc_data_buffer);
			
			sprintf(lcd_print_voltage_string, "Voltage: %0.2f V", adc_data_voltage_reading);
			lcd_string(2, 1, lcd_print_voltage_string);
			
			sprintf(tx_voltage_buffer, "Voltage: %0.2f V\n", adc_data_voltage_reading);
			uart_tx_string(tx_voltage_buffer);
			
			convert_analog_channel_data( sharp_sensor_channel );
		}
	}
}
//---------------------------------- END ------------------------------------------------------------------