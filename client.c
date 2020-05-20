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
	char header[5];
	char payload[20];
	}pacchetto;
	
	/******************************************************************
	 * 				Funzioni di Supporto							  *
	 * ***************************************************************/
void inviaPacchetto(pacchetto package, int fd){
	//Inserire lo sprintf per estendere il valore a 20 caratteri, fare un ciclo for per ogni attributo della struct
	for(int i=0; i<sizeof(package.payload); i++){
		char c=package.payload[i];
		write(fd, &c, 1);
		usleep(50000);
	  }
	for(int i=0; i<sizeof(package.header); i++){
		char c=package.header[i];
		write(fd, &c, 1);
		usleep(50000);
		}  
	}

casa configurazioneIniziale(casa Casa, int fd){
	pacchetto package;
	//Casa
	printf("Inserisci il nome della casa: ");	
	scanf("%s",&Casa.nome);
	strcpy(package.header,"house");
	sprintf(package.payload,"%-20s",Casa.nome);
	inviaPacchetto(package,fd);
	//Stanza1
	printf("Inserisci il nome della stanza n°1: ");
	scanf("%s",&Casa.led1);
	strcpy(package.header,"1room");
	sprintf(package.payload,"%-20s",Casa.led1);
	inviaPacchetto(package,fd);
	//Stanza2
	printf("Inserisci il nome della stanza n°2: ");
	scanf("%s",&Casa.led2);
	strcpy(package.header,"2room");
	sprintf(package.payload,"%-20s",Casa.led2);
	inviaPacchetto(package,fd);
	//Stanza3
	printf("Inserisci il nome della stanza n°3: ");
	scanf("%s",&Casa.led3);
	strcpy(package.header,"3room");
	sprintf(package.payload,"%-20s",Casa.led3);
	inviaPacchetto(package,fd);
	//Tempo Di acquisizione temperatura
	printf("Ogni quanti secondi vuoi rilevare la temperatura? ");
	scanf("%s",&Casa.tempo);
	strcpy(package.header,"ctemp");
	sprintf(package.payload,"%-20s",Casa.tempo);
	inviaPacchetto(package,fd);
	return Casa;
}
	
casa richiediConfigurazioneIniziale(int fd, casa Casa){
	pacchetto packet;
	strcpy(packet.header, "oldho");
	sprintf(packet.payload,"%-20s"," ");
	inviaPacchetto(packet,fd);
	char c=' ';
	for(int i=0; i<sizeof(packet.payload);i++){
		read(fd,&c,1);
		strcat(Casa.nome, &c);
		}
	for(int i=0; i<sizeof(packet.payload);i++){
		read(fd,&c,1);
		strcat(Casa.led1, &c);
		}
	for(int i=0; i<sizeof(packet.payload);i++){
		read(fd,&c,1);
		strcat(Casa.led2, &c);
		}
	for(int i=0; i<sizeof(packet.payload);i++){
		read(fd,&c,1);
		strcat(Casa.led3, &c);
		}
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
		}else{
			
			
	//Richiedi i valori dalla eeprom e copiali in un typedef casa
	richiediConfigurazioneIniziale(fd,Casa);
	
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
		//write(fd,stanzaRichiesta,1);
		
		//scrivi la stanzaRichiesta in un pacchetto
		//invialo all'arduino
	}
	close(fd);
	}
