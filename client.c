#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
typedef struct casa{
	char nome[40];
	char led1[20];
	char led2[20];
	char led3[20];
	int tempo;
} casa;



casa configurazioneIniziale(casa Casa){
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
	//A questo punto devo scrivere il typedef su un buffer, fare il checksum e inviarlo all'arduino
	return Casa;
}

int main(int argc,char** argv){
	if (argc!=2){
		printf("Inserisci come parametro il percorso dell'arduino Mega2560\n");
		return -1;
		}
	const char *device = argv[1];
	casa Casa;
	char config;
	struct termios tty;
	int fd= open(device, O_RDWR | O_NOCTTY | O_SYNC );
	if (fd<0) perror("error opening serial: ");
	tcgetattr(fd, &tty);
	if(tcgetattr(fd,&tty) != 0) perror("error from tcgetattr");
	cfsetospeed(&tty, B19200);
	cfsetispeed(&tty, B19200);
	tty.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
		tty.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
		tty.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
		tty.c_cflag |=  CS8;      /* Set the data bits = 8                                 */
	
		tty.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
		tty.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */ 
		
		
		tty.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
		tty.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

		tty.c_oflag &= ~OPOST;/*No Output Processing*/

	if((tcsetattr(fd,TCSANOW,&tty)) != 0) perror("error setting attributes");
	
	
	printf("Vuoi fare la configurazione iniziale? y/n ");
	scanf("%c",&config);
	
	char stanzaRichiesta[20]="c";
	float temperatura;
	if(config=='y') { Casa=configurazioneIniziale(Casa);}
	//Richiedi i valori dalla eeprom e copiali in un typedef casa
	
	while(strcmp(stanzaRichiesta,"x")!=0){
		//richiedi la temperatura
		printf("Benvenuto a %s. Digita \"x\" per uscire. Digita il nome di una stanza per accendere la luce. La temperatura attuale è di %f. \n Ti ricordo i nomi delle stanze:\n 1. %s \n 2. %s \n 3. %s\n", 
											Casa.nome,temperatura, Casa.led1,Casa.led2,Casa.led3);
		scanf("%s",stanzaRichiesta);
		write(fd,stanzaRichiesta,sizeof(stanzaRichiesta));
		read(fd,&temperatura,1);
		//scrivi la stanzaRichiesta in un buffer, fai il checksum e invialo all'arduino
	}
	close(fd);
	}
