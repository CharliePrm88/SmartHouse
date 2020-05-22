#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay_basic.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define BAUD 19200
#define MYUBRR (F_CPU/16/BAUD-1)

/********************************************************************
 *              Variabili Globali                                   *
 *******************************************************************/
const uint8_t pin2=(1<<0);
const uint8_t pin3=(1<<2);
float EEMEM temperatura;
char EEMEM nomeCasa[20];
char EEMEM nomeStanza1[20];
char EEMEM nomeStanza2[20];
char EEMEM nomeStanza3[20];
uint16_t EEMEM tempoAcquisizione = 5;
int bufferpointer=0;
char buffer[25];

/*********************************************************************
 *                    Pacchetti                                      *
 * ******************************************************************/

typedef struct casa{
	char nome[20];
	char led1[20];
	char led2[20];
	char led3[20];
	char tempo[20];
} casa;

typedef struct pacchetto{
	char header[5];
	char payload[20];
	}pacchetto;
/*********************************************************************
 *                    Funzioni                                       *
 * ******************************************************************/

void UART_init(void){
  // Set baud rate
  UBRR0H = (uint8_t)(MYUBRR>>8);
  UBRR0L = (uint8_t)MYUBRR;
  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); /* 8-bit data */ 
  UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);   /* Enable RX and TX */  
  UCSR0B |= (1 << TXCIE0); 
  sei();
  }

void ledOn1(void){
const uint8_t pin1=(1<<7);
  DDRB |= pin1;
    if (PORTB==pin1)
      PORTB=0;
    else
      PORTB=pin1;
}

void ledOn2(void){
    if (PORTA==pin2 || PORTA==(pin2|pin3)){
        PORTA=PORTA-pin2;
        }
    else{
      PORTA|=pin2;
      }
}

void ledOn3(void){
    if (PORTA==pin3 || PORTA==(pin2|pin3)){
      PORTA=PORTA-pin3;
        }
    else{
      PORTA|=pin3;
  }
}

int analogPortA0(void)
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

void temp(void){
    float voltage,temperature;
    int sensorVal;
    sensorVal =analogPortA0();
    voltage= (sensorVal/1024.0f)*5.0f;
    temperature=(voltage-0.5f)*100.0f;
    eeprom_update_float( &temperatura , round(temperature * 10) / 10.0f);
    /*
    for(int i=0; i<4; i++) {
        while ( !(UCSR0A & (1<<UDRE0)) );
        UDR0 = tempc[i];
        }*/
    }
    
    void stampaTemperatura(void){
      char tempc[5];
      float temp1 = eeprom_read_float(&temperatura);
      dtostrf(temp1, 2, 1, tempc );
      printf("%s\n",tempc); //va sostituito
      }
    
    /**************************************************************
     *                Work in progress                            *
     *************************************************************/
     void sendConfig(void){
      memset(buffer,0,sizeof(buffer));

      casa vecchiaCasa;
      eeprom_read_block(vecchiaCasa.nome,nomeCasa,20);
      eeprom_read_block(vecchiaCasa.led1,nomeStanza1,20);
      eeprom_read_block(vecchiaCasa.led2,nomeStanza2,20);
      eeprom_read_block(vecchiaCasa.led3,nomeStanza3,20);
      for(int i=0; i<20; i++){UDR0=vecchiaCasa.nome[i];_delay_ms(50);}
      for(int i=0; i<20; i++){UDR0=vecchiaCasa.led1[i];_delay_ms(50);}
      for(int i=0; i<20; i++){UDR0=vecchiaCasa.led2[i]; _delay_ms(50);}
      for(int i=0; i<20; i++){UDR0=vecchiaCasa.led3[i]; _delay_ms(50);}
      char test[20];
        eeprom_read_block(test,nomeCasa,sizeof(nomeCasa));
        printf("read_block: %s\n",test);
        eeprom_read_block(test,nomeStanza1,sizeof(nomeStanza1));
        printf("read_block: %s\n",test);
        eeprom_read_block(test,nomeStanza2,sizeof(nomeStanza2));
        printf("read_block: %s\n",test);
        eeprom_read_block(test,nomeStanza3,sizeof(nomeStanza3));
        printf("read_block: %s\n",test);
        printf("Buffer: %s BufferPointer: %i\n",buffer,bufferpointer);
      }
      
    void gestorePacchettiIncoming(pacchetto packet){
      if(strstr(packet.header,"hou")!=NULL){
        eeprom_update_block(packet.payload,nomeCasa,sizeof(packet.payload));
        memset(buffer,0,sizeof(buffer));
        printf("Buffer: %s BufferPointer: %i\n",buffer,bufferpointer);
        }
      else if (strstr(packet.header, "1roo")!=NULL){
        eeprom_update_block(packet.payload,nomeStanza1,sizeof(packet.payload));
        memset(buffer,0,sizeof(buffer));
        printf("Buffer: %s BufferPointer: %i\n",buffer,bufferpointer);
        } 
      else if (strstr(packet.header, "2roo")!=NULL){
        eeprom_update_block(packet.payload,nomeStanza2,sizeof(packet.payload));
        memset(buffer,0,sizeof(buffer));
        printf("Buffer: %s BufferPointer: %i\n",buffer,bufferpointer);
        }
      else if (strstr(packet.header, "3roo")!=NULL){
        eeprom_update_block(packet.payload,nomeStanza3,sizeof(packet.payload));
        ledOn3();
        memset(buffer,0,sizeof(buffer));
        printf("Buffer: %s BufferPointer: %i\n",buffer,bufferpointer);
        }
      else if (strstr(packet.header, "ctem")!=NULL){
        eeprom_update_word(&tempoAcquisizione,atoi(packet.payload));
        memset(buffer,0,sizeof(buffer));
        printf("Buffer: %s BufferPointer: %i\n",buffer,bufferpointer);
        }
      else if(strstr(packet.header,"oldh")!=NULL){
        sendConfig();
        ledOn3();
        }
     //printf("Buffer: %s BufferPointer: %i\n",buffer,bufferpointer);

      }
  
    ISR(USART0_RX_vect)
{
      char c=UDR0;
      buffer[bufferpointer]=c;
      //printf("%i %s\n",bufferpointer,buffer);
      bufferpointer++;

    if (bufferpointer>=sizeof(buffer))	{bufferpointer = 0;}
}

/****************************************************************
 *                    Main                                      *
 * *************************************************************/

int main(void){
  pacchetto package;
  UART_init();
  printf_init();
  DDRA |= pin2;
  DDRA |= pin3;
  while(1) {
   // printf("%i %s\n",bufferpointer,buffer);
    strlcpy(package.header, buffer+sizeof(package.payload), sizeof(package.header));
    strlcpy(package.payload, buffer, sizeof(package.payload));
    gestorePacchettiIncoming(package);
    }
}
