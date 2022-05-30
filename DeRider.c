#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <errno.h>

//struct con i dati dell'user
typedef struct user{
	char nome[30];
	char cognome[30];
	char username[30];
	char password[20];
	char indirizzo[100];
    int idUser;
} user;

//struct con i dati del corriere
typedef struct corriere{
	char nome[30];
	char cognome[30];
	char username[30];
	char password[20];
   	int idCorriere;
} corriere;

//prototipi delle funzioni
int statoAccesso(char *);
char* loginCorriere (void);
char* nuovoCorriere (void);
int FullRead(int sockfd, void *buffer, size_t count);
int FullWrite(int sockfd,void *buffer, size_t count);
void running_rider(int connfd, char *);


int main()
{
	int verificaAccesso;
	int sockfd;
	struct sockaddr_in servaddr,clientaddr;
	char corriereAttuale[30] = "Non riconosciuto";

	printf("Benvenuto all'accesso per corrieriInu!\n Procedi al login o alla registrazione.\n");
	verificaAccesso = statoAccesso(corriereAttuale);

	if(verificaAccesso==1)
	{
		printf("Connecting to main server\n");
		sockfd=socket(AF_INET,SOCK_STREAM,0);
		if(sockfd==-1)
		{
			perror("Error!Socket not created!\n");
			exit(1);
		}else{
			printf("Socket create successfully!\n");
		}
		servaddr.sin_family=AF_INET;
		servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
		servaddr.sin_port=htons(5585);// porta del rider DeInu

		if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
		{
            perror("Non ci sono consegne da effettuare al momento.\n");
			perror("Error! Connect returned with error!\n");
			exit(1);
		}else{
			printf("Deliver connected to the server properly!\n");
		}
		running_rider(sockfd, corriereAttuale);
		close(sockfd);
	}
}


void running_rider(int sockfd, char* username)
{
	int sceltarider,riderid,orderid,clientid,riderok;
	char messaggio[1000];
	user dati;
	corriere userData;
	char consegna[1000]="Lo stato della consegna è OK: Consegnato";

	srand(time(NULL));
	riderid=rand();

	while(1)
	{
		FullRead(sockfd,messaggio,sizeof(messaggio));
		printf("%s",messaggio);
		sceltarider=1;
		FullWrite(sockfd,(char *)&sceltarider,sizeof(sceltarider));
			//ID user
			FullRead(sockfd,(char *)&clientid,sizeof(clientid));
			printf("ID user %d \n",clientid);
			//DATI user
			FullRead(sockfd,(char *)&dati,sizeof(dati));
			printf("Ecco i dati del user #%d :\nNome: %s\nCognome: %s\nIndirizzo: %s\n",clientid,dati.nome,dati.cognome,dati.indirizzo);
			//ID ORDINE
			FullRead(sockfd,(char *)&orderid,sizeof(orderid));
			printf("ID dell'ordine: %d\n",orderid);

			//IDENTIFICHIAMO IL CORRIERE CONNESSO
			FILE *log;
                 	log = fopen("corrieri.txt","r");
               	        if (log == NULL)
                        {
                           fputs("Errore durante apertura del file.", stderr);
                           exit(1);
                        }

                    struct corriere C;

                    while(fread(&C,sizeof(C),1,log))
                    {
                        if(strcmp(username,C.username)==0)
                        {
                            strcpy(userData.nome, C.nome);
                            strcpy(userData.cognome, C.cognome);
                            strcpy(userData.username, C.username);
                            userData.idCorriere = C.idCorriere;
                        }
                   else
           		{
                printf("\nErrore nel recupero userData del corriere.\n");
            }
        }
   			fclose(log);


			riderid = userData.idCorriere;

			printf("Il tuo ID corriere: %d\n",riderid);
			FullWrite(sockfd,(char *)&riderid,sizeof(riderid));

			printf("Hai consegnato l'ordine? \n1)Si;\n2)No.\nEsito:");
			scanf("%d",&riderok);

			if(riderok==1)
			{
				printf("%s\n",consegna);
				FullWrite(sockfd,consegna,sizeof(consegna));
				close(sockfd);
				exit(0);
			}
			else if (riderok==2)
			{
                char NoConsegna[1000] = "Consegna NON effettuata.";
                FullWrite(sockfd,NoConsegna,sizeof(NoConsegna));
				printf("%s \n", NoConsegna);
				sceltarider=0;
				exit(0);
			}
			else
            {
               printf("Errore! Scelta non valida\n");
               exit(0);
            }
		}
	}


int statoAccesso (char *corriereAttuale)
{
    int opzione;
    printf("Scegli 1 per registrarti\nScegli 2 per accedere\n\n");
    scanf("%d",&opzione);
    getchar();

    if(opzione == 1)
        {
            char *registraCorriere = nuovoCorriere();
            strcpy(corriereAttuale, registraCorriere);
            return 1;
        }

    else if(opzione == 2)
       {
            char *utilizzatore = loginCorriere();
            strcpy(corriereAttuale, utilizzatore);
            return 1;
        }
    else
        {
           return -1;
        }
}


char* loginCorriere (void)
{
    char username[30],password[20];

    FILE *log;
    log = fopen("corrieri.txt","r");

    if (log == NULL)
    {
        fputs("Errore durante apertura del file.", stderr);
        exit(1);
    }

    struct corriere C;

    printf("\nInserisci le tue credenziali per il login\n\n");

    printf("Username:  ");
    scanf("%s",username);
    fflush(stdin);

    printf("\nPassword: ");
    scanf("%s",password);
    fflush(stdin);

    while(fread(&C,sizeof(C),1,log))
        {
        if(strcmp(username,C.username)==0 && strcmp(password,C.password)==0)
            {
               printf("\nAccesso riuscito!\n");
                char *user = C.username;
                printf("Uscita dalla login con username = %s \n", user);
                return user;
            }
        else
            {
                printf("\nCredenziali errate.\n");
            }
        }
    fclose(log);
}



char* nuovoCorriere(void)
{
    FILE *log;
    log=fopen("corrieri.txt","w");
    if (log == NULL)
    {
        fputs("Errore apertura file", stderr);
        exit(1);
    }

    struct corriere C;

    printf("\nAvvio procedura di registrazione..\n\n");

    printf("\nNome:\n");
    scanf("%s",C.nome);
    fflush(stdin);

    printf("\nCognome:\n");
    scanf("%s",C.cognome);
    fflush(stdin);

    printf("\nUsername:\n");
    scanf("%s",C.username);
    fflush(stdin);

    printf("\nPassword:\n");
    scanf("%s",C.password);
    fflush(stdin);

    srand(time(NULL));
    int sorgenteID = rand();
    C.idCorriere = sorgenteID;
    printf("Il suo ID utente sara': %d", C.idCorriere);


    fwrite(&C,sizeof(C),1,log);
    fclose(log);

    printf("\nInformazioni ricevute.\nBenvenuto, %s!\n\n",C.nome);
    getchar();

    char *user = C.username;
    printf("Registrato con successo l'utente: %s \n", user);
    return user;

}


int FullRead(int sockfd, void *buffer, size_t count)
{
	size_t nleft;
	ssize_t nread;
	int byte_read = 0;
	nleft = count;
	while(nleft>0)
	{
		if((nread=read(sockfd,buffer,nleft))<0)
		{
			if(errno==EINTR)
			{
				continue;
			}else{
				return(byte_read+nread);
			}
		}else if(nread==0)
		{
			break;
		}
		nleft=nleft-nread;
		buffer+=nread;
		byte_read=byte_read+nread;
	}
	return(byte_read);
}

int FullWrite(int sockfd, void *buffer,size_t count)
{
	size_t nleft;
	ssize_t nwritten;
	int written=0;
	nleft=count;
	while(nleft>0)
	{
		if((nwritten=write(sockfd,buffer,nleft))<0)
		{
			if(errno==EINTR)
			{
				continue;
			}else{
				return(written+nwritten);
			}
		}
		nleft=nleft-nwritten;
		buffer+=nwritten;
		written=written+nwritten;
	}
	return(written);
}
