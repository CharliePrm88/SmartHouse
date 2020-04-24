#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>

#define MAX_BUF 256
#define BAUD 19600
#define MYUBRR (F_CPU/16/BAUD-1)

void UART_init(void){
  // Set baud rate
  UBRR0H = (uint8_t)(MYUBRR>>8);
  UBRR0L = (uint8_t)MYUBRR;

  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); /* 8-bit data */ 
  UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);   /* Enable RX and TX */  

}

void UART_putChar(uint8_t c){
  // wait for transmission completed, looping on status bit
  while ( !(UCSR0A & (1<<UDRE0)) );

  // Start transmission
  UDR0 = c;
}

uint8_t UART_getChar(void){
  // Wait for incoming data, looping on status bit
  while ( !(UCSR0A & (1<<RXC0)) );
  
  // Return the data
  return UDR0;
    
}

// reads a string until the first newline or 0
// returns the size read
uint8_t UART_getString(uint8_t* buf){
  uint8_t* b0=buf; //beginning of buffer
  while(1){
    uint8_t c=UART_getChar();
    *buf=c;
    ++buf;
    // reading a 0 terminates the string
    if (c==0)
      return buf-b0;
    // reading a \n  or a \r return results
    // in forcedly terminating the string
    if(c=='\n'||c=='\r'){
      *buf=0;
      ++buf;
      return buf-b0;
    }
  }
}

void UART_putString(uint8_t* buf){
  while(*buf){
    UART_putChar(*buf);
    ++buf;
  }
}

void ledOn1(uint8_t* buf){
const uint8_t pin1=(1<<7);
  // we configure the pin as output
  UART_putString((uint8_t*)"Hai acceso il pin ");
  UART_putString(buf);
  DDRB |= pin1;
    if (PORTB==pin1)
      PORTB=0;
    else
      PORTB=pin1;
}

void ledOn2(uint8_t* buf){
const uint8_t pin2=(1<<0);
  // we configure the pin as output
  UART_putString((uint8_t*)"Hai acceso il pin ");
  UART_putString(buf);
  DDRA |= pin2;
    if (PORTA==pin2)
      PORTA=0;
    else
      PORTA=pin2;
}

void ledOn3(uint8_t* buf){
const uint8_t mask=(1<<2);
  // we configure the pin as output
  UART_putString((uint8_t*)"Hai acceso il pin ");
  UART_putString(buf);
  DDRA |= mask;
    if (PORTA==mask)
      PORTA=0;
    else
      PORTA=mask;
}

int main(void){
  UART_init();
  //UART_putString((uint8_t*)"Arduino On");
  uint8_t buf[MAX_BUF];
  while(1) {
    UART_getString(buf);
  //UART_putString((uint8_t*)"hai scritto\n");
    if(strncmp((char*)buf,"1\n",2)==0) ledOn1(buf);
    if(strncmp((char*)buf,"2\n",2)==0) ledOn2(buf);
    if(strncmp((char*)buf,"3\n",2)==0) ledOn3(buf);
    UART_putString(buf);
  }
}
