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
	char header[14];
	char nome[20];
	char led1[20];
	char led2[20];
	char led3[20];
	unsigned char tempo;
} casa;

typedef struct packAccensione{
	char header[14];
	char led[20];
	}packAccensione;
	
typedef struct packTemperatura{
	char header[14];
	float temperatura;
	}packTemperatura;
	
	/******************************************************************
	 * 				Funzioni di Supporto							  *
	 * ***************************************************************/

casa configurazioneIniziale(casa Casa, int fd){
	strcpy(Casa.header,"configurazione");
	printf("Inserisci il nome della casa ");	
	scanf("%s",&Casa.nome);
	printf("Inserisci il nome della stanza n°1: ");
	scanf("%s",&Casa.led1);
	printf("Inserisci il nome della stanza n°2: ");
	scanf("%s",&Casa.led2);
	printf("Inserisci il nome della stanza n°3: ");
	scanf("%s",&Casa.led3);
	printf("Ogni quanti secondi vuoi rilevare la temperatura? ");
	scanf("%d",&Casa.tempo);
	return Casa;
}

void inviaConfigurazioneIniziale(casa Casa, int fd){
	struct casa *buffer = (struct casa *)malloc(sizeof(struct casa));
	memset(buffer, 0, sizeof(struct casa));
	memcpy(buffer, &Casa, sizeof(struct casa));
	
	int ret=0;
		for (int i=0; i<sizeof(struct casa); i++){
        int bytes_sent=0;
        while(bytes_sent < 1){
            ret = write(fd, buffer+i, 1);
            if (ret == -1 && errno == EINTR) continue;
            if (ret == -1) perror("Error writing");
            bytes_sent += ret;
        }
        usleep(1000);
    }
	free(buffer);
	}
	
casa richiediConfigurazioneIniziale(int fd){
	//read(fd,&temperatura,1);
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
	if((tcsetattr(fd,TCSANOW,&tty)) != 0) perror("error setting attributes");
	
	/*************************************************************************
	*				Configurazione iniziale									 *
	*************************************************************************/
	printf("Vuoi fare la configurazione iniziale? y/n ");
	char config;
	scanf("%c",&config);
	if(config=='y') { 
		Casa=configurazioneIniziale(Casa,fd);
		inviaConfigurazioneIniziale(Casa,fd);
		//A questo punto devo scrivere Casa su un buffer, fare il checksum e inviarlo all'arduino
		}else{
			
			
	//Richiedi i valori dalla eeprom e copiali in un typedef casa
	
	
	}
	
	/***************************************************************************
	 * 							fino a che non invia "x"					   *
	 * ************************************************************************/
	 
	char stanzaRichiesta[20]="c";
	float temperatura;
	while(strcmp(stanzaRichiesta,"x")!=0){
		//richiedi la temperatura
		printf("Benvenuto a %s. Digita \"x\" per uscire. Digita il nome di una stanza per accendere la luce. La temperatura attuale è di %f. \n Ti ricordo i nomi delle stanze:\n 1. %s \n 2. %s \n 3. %s\n", 
											Casa.nome,temperatura, Casa.led1,Casa.led2,Casa.led3);
		scanf("%s",stanzaRichiesta);
		write(fd,stanzaRichiesta,sizeof(stanzaRichiesta));
		
		//scrivi la stanzaRichiesta in un buffer
		//invialo all'arduino
	}
	close(fd);
	}
