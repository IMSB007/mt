#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdint.h> // needed for uint8_t
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>


#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD -1
volatile char ReceivedChar;
volatile uint8_t ADCvalue;



int USART0SendByte(char u8Data, FILE *stream)
{

	while(!(UCSR0A&(1<<UDRE0))){};
	// Transmit data
	UDR0 = u8Data;
	return 0;
}




//set stream pointer
FILE usart0_str = FDEV_SETUP_STREAM(USART0SendByte,NULL,  _FDEV_SETUP_RW);

int main( void )
{
	stdin = stdout = &usart0_str;
	/*Set baud rate */
	UBRR0H = (MYUBRR >> 8);
	UBRR0L = MYUBRR;
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0); // Enable receiver and transmitter
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ00); // Set frame: 8data, 1 stp

	
	ADMUX = 0; // use ADC0
	ADMUX |= (1 << REFS0); // use AVcc as the reference
	ADMUX |= (1 << ADLAR); // Right adjust for 8 bit resolution
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 prescale for 16Mhz
	ADCSRA |= (1 << ADATE); // Set ADC Auto Trigger Enable
	ADCSRB = 0; // 0 for free running mode
	ADCSRA |= (1 << ADEN); // Enable the ADC
	ADCSRA |= (1 << ADIE); // Enable Interrupts
	ADCSRA |= (1 << ADSC); // Start the ADC conversion
	
	_delay_ms(2000);
	printf("AT\r\n");
	_delay_ms(2000);
	printf("AT+CWMODE=1\r\n");
	_delay_ms(2000);
	printf("AT+CIPMUX=1\r\n");
	//_delay_ms(2000);
	//printf("AT+CIPMODE=0\r\n");
	_delay_ms(2000);
	printf("AT+CWJAP=\"Alex\",\"0123456789\"\r\n");
	_delay_ms(8000);

	//printf("temperature %u F\r\n ", ((ADCvalue*9)/10)+32 );
	while(1)
	{
		ADCvalue = ADCH;
		printf("AT+CIPSTART=0,\"TCP\",\"api.thingspeak.com\",80\r\n");
		_delay_ms(2000);
		printf("AT+CIPSEND=0,44\r\n");
		_delay_ms(2000);
		printf("GET /update?key=TSY7E3VWRQ8D6JGG&field1=%u\r\n",((ADCvalue*9)/10)+32 );
		_delay_ms(2000);
		printf("AT+CIPCLOSE\r\n");
		_delay_ms(2000);
	}
	return 1;
}