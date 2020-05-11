#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#define MAX_BUF 256
#define BAUD 19600
#define MYUBRR (F_CPU/16/BAUD-1)
const uint8_t pin2=(1<<0);
const uint8_t pin3=(1<<2);

void UART_init(void){
  // Set baud rate
  UBRR0H = (uint8_t)(MYUBRR>>8);
  UBRR0L = (uint8_t)MYUBRR;
  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); /* 8-bit data */ 
  UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);   /* Enable RX and TX */  
  UCSR0B |= (1 << TXCIE0); 
  sei();
  }

void ledOn1(){
const uint8_t pin1=(1<<7);
  DDRB |= pin1;
    if (PORTB==pin1)
      PORTB=0;
    else
      PORTB=pin1;
}

void ledOn2(){
    if (PORTA==pin2 || PORTA==(pin2|pin3)){
        PORTA=PORTA-pin2;
        }
    else{
      PORTA|=pin2;
      }
}

void ledOn3(){
    if (PORTA==pin3 || PORTA==(pin2|pin3)){
      PORTA=PORTA-pin3;
        }
    else{
      PORTA|=pin3;
  }
}

int analogPortA0()
{
  ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //ADCStatusRegister = Scelgo la porta da leggere
  ADCSRB = 0;
  ADMUX = 1 << REFS0; //ADC Multiplexer Selection Register = Seleziono il registro
  ADMUX &= ~(1 << MUX4) & ~(1 << MUX3);
  ADCSRA |= (1 << ADEN); //Enable la conversione
  ADCSRB &= ~(1 << MUX5);
  ADMUX &= ~(1 << MUX2) & ~(1 << MUX1) & ~(1 << MUX0);
  uint8_t mask = 0;
  mask |= (1 << ADSC); //inizia la conversione
  ADCSRA |= (1 << ADSC); 
  while (ADCSRA & mask); //while la conversione è terminata
  const uint8_t low = ADCL;
  const uint8_t high = ADCH;
  return (high << 8) | low;
}

void temp(){
    char tempc[5];
    float voltage,temperature;
    int sensorVal;
    sensorVal =analogPortA0();
    voltage= (sensorVal/1024.0f)*5.0f;
    temperature=(voltage-0.5f)*100.0f;
    dtostrf( temperature, 2, 1, tempc );
    for(int i=0; i<4; i++) {
        while ( !(UCSR0A & (1<<UDRE0)) );
        UDR0 = tempc[i];
        }
    }
    
    ISR(USART0_RX_vect)
{
    char c =UDR0;
    switch(c){
        case '1': ledOn1(c);
        break;
        case '2': ledOn2(c);
        break;
        case '3': ledOn3(c);
        break;
        }
    temp();
}

int main(void){
  printf_init();
  UART_init();
  DDRA |= pin2;
  DDRA |= pin3;
  while(1) {
    /*UART_getString(buf);
    if(strncmp((char*)buf,"1\n",2)==0) ledOn1(buf);
    if(strncmp((char*)buf,"2\n",2)==0) ledOn2(buf);
    if(strncmp((char*)buf,"3\n",2)==0) ledOn3(buf);*/
    
  }
}
