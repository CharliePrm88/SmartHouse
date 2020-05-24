#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

	/******************************************************
	 * 				Pacchetti							  *
	 * ***************************************************/


typedef struct casa{
	char nome[20];
	char led1[20];
	char led2[20];
	char led3[20];
	char tempo[20];
} casa;

typedef struct pacchetto{
	char header[6];
	char payload[20];
	}pacchetto;

pacchetto packet;
	/******************************************************************
	 * 				Funzioni di Supporto							  *
	 * ***************************************************************/
void inviaPacchetto(pacchetto packet, int fd){
	for(int i=0; i<sizeof(packet.payload); i++){
		char c=packet.payload[i];
		write(fd, &c, 1);
		usleep(50000);
	  }
	for(int i=0; i<sizeof(packet.header); i++){
		char c=packet.header[i];
		write(fd, &c, 1);
		usleep(50000);
		}
		
	}

void gestoreLuci(casa Casa, char *stanzaRichiesta, int fd){
	strcpy(packet.payload," ");
	sprintf(packet.payload,"%-19s"," ");
	if(strstr(Casa.led1,stanzaRichiesta)!=NULL){
		strcpy(packet.header,"1leds");
		inviaPacchetto(packet,fd);
		}
	if(strstr(Casa.led2,stanzaRichiesta)!=NULL){
		strcpy(packet.header,"2leds");
		inviaPacchetto(packet,fd);
		}
	if(strstr(Casa.led3,stanzaRichiesta)!=NULL){
		strcpy(packet.header,"3leds");
		inviaPacchetto(packet,fd);
		}
	}

casa configurazioneIniziale(casa Casa, int fd){
	//Casa
	printf("Inserisci il nome della casa: ");	
	scanf("%s",&Casa.nome);
	strcpy(packet.header, "house");
	sprintf(packet.payload,"%-19s",Casa.nome);
	inviaPacchetto(packet,fd);
	//Stanza1
	printf("Inserisci il nome della stanza n°1: ");
	scanf("%s",&Casa.led1);
	strcpy(packet.header,"1room");
	sprintf(packet.payload,"%-19s",Casa.led1);	
	inviaPacchetto(packet,fd);
	//Stanza2
	printf("Inserisci il nome della stanza n°2: ");
	scanf("%s",&Casa.led2);
	strcpy(packet.header,"2room");
	sprintf(packet.payload,"%-19s",Casa.led2);
	inviaPacchetto(packet,fd);
	//Stanza3
	printf("Inserisci il nome della stanza n°3: ");
	scanf("%s",&Casa.led3);
	strcpy(packet.header,"3room");
	sprintf(packet.payload,"%-19s",Casa.led3);
	inviaPacchetto(packet,fd);
	//Tempo Di acquisizione temperatura
	printf("Ogni quanti secondi vuoi rilevare la temperatura? ");
	scanf("%s",&Casa.tempo);
	strcpy(packet.header,"ctemp");
	sprintf(packet.payload,"%-19s",Casa.tempo);
	inviaPacchetto(packet,fd);
	return Casa;
}
casa richiediConfigurazioneIniziale(int fd, casa Casa){
	strcpy(packet.header, "oldho");
	sprintf(packet.payload,"%-19s"," ");
	inviaPacchetto(packet,fd);
	read(fd,Casa.nome,sizeof(packet.payload));
	read(fd,Casa.led1,sizeof(packet.payload));
	read(fd,Casa.led2,sizeof(packet.payload));
	read(fd,Casa.led3,sizeof(packet.payload));
	return Casa;
	}

float richiediTemperatura(int fd){
	//read(fd,&temperatura,1);
	}

int main(int argc,char** argv){
	if (argc!=2){
		printf("Inserisci come parametro il percorso dell'arduino Mega2560\n");
		return -1;
		}
	
	casa Casa;
	/**************************************************
	*  			SETTAGGIO DELLA PORTA SERIALE		  *
	**************************************************/
	const char *device = argv[1];
	struct termios tty;
	int fd= open(device, O_RDWR | O_NOCTTY | O_SYNC );
	if (fd<0) perror("error opening serial: ");
	tcgetattr(fd, &tty);
	if(tcgetattr(fd,&tty) != 0) perror("error from tcgetattr");
	cfsetospeed(&tty, B19200);
	cfsetispeed(&tty, B19200);
	tty.c_cflag &= ~PARENB;   
	tty.c_cflag &= ~CSTOPB;   
	tty.c_cflag &= ~CSIZE;	 
	tty.c_cflag |=  CS8;      
	tty.c_cflag &= ~CRTSCTS;      
	tty.c_cflag |= CREAD | CLOCAL;  
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);
	tty.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	tty.c_oflag &= ~OPOST;
	tty.c_cc[VTIME] = 10;
	if((tcsetattr(fd,TCSANOW,&tty)) != 0) perror("error setting attributes");
	
	/*************************************************************************
	*				Configurazione iniziale									 *
	*************************************************************************/
	printf("Vuoi fare la configurazione iniziale? y/n ");
	char config;
	scanf("%c",&config);
	if(config=='y') { 
		Casa=configurazioneIniziale(Casa,fd);
	}else{		
		//Richiedi i valori dalla eeprom e copiali in un typedef casa
		Casa=richiediConfigurazioneIniziale(fd,Casa);
	}
	
	/***************************************************************************
	 * 							fino a che non invia "x"					   *
	 * ************************************************************************/
	 
	char stanzaRichiesta[20]="c";
	float temperatura;
	pacchetto packet;
	while(strcmp(stanzaRichiesta,"x")!=0){
		printf("Nome della casa: %s\nDigita \"x\" per uscire.\nDigita il nome di una stanza per accendere la luce.\nLa temperatura attuale è di %f. \nTi ricordo i nomi delle stanze:\n 1. %s \n 2. %s \n 3. %s\n", 
											Casa.nome,temperatura, Casa.led1,Casa.led2,Casa.led3);
		scanf("%s",stanzaRichiesta);
		gestoreLuci(Casa,stanzaRichiesta,fd);
		//richiediTemperatura;
		//scrivi la stanzaRichiesta in un pacchetto
		//invialo all'arduino
	}
	close(fd);
	}
