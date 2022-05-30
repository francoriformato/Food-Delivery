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

// struct con i dati dell’user
typedef struct user{
	char nome[30];
	char cognome[30];
	char username[30];
	char password[20];
	char indirizzo[100];
    int idUser;
} user;

// prototipi delle funzioni
int Login(char* );
char* login (void);
char* registrati (void);
void makeDelivery(int, char* );
int FullRead(int sockfd, void *buffer, size_t count);
int FullWrite(int sockfd,void *buffer, size_t count);

int main()
{
	int verificaLogin;
	int sockfd;
	struct sockaddr_in servaddr;
	char usernameAttuale[30] = "Non trovato";

	printf("Benvenuto su InuRestaurants, procedi pure al login o alla registrazione!\n");

	verificaLogin = Login(usernameAttuale);
    printf("Stai effettuando il login come..: %s\n", usernameAttuale);

	if(verificaLogin == 1)
	{
		printf("Connection to main server\n");
		sockfd=socket(AF_INET,SOCK_STREAM,0); // creazione del descrittore sockfd tramite la funzione int socket(int famiglia, int tipo, int protocollo)
		if(sockfd == -1)//la funzione socket restituisce un intero >=0 con successo, -1 in caso di errore
		{
			perror("Error!Socket not created!\n");
			exit(1);
		}else{
			printf("Socket create successfully!\n");
		}
        //valorizzazione della struttura sockaddr_in
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		servaddr.sin_port = htons(5080);//porta del server
		if(connect(sockfd,(struct sockaddr *)&servaddr, sizeof(servaddr))!=0)
		{
			perror("Error! Connect returned with error!\n");
			exit(1);
		}else{
			printf("Connected made properly!\n");
		}

		printf("Login completato come: %s\n", usernameAttuale);

		makeDelivery(sockfd, usernameAttuale);

		close(sockfd);
	}
	else
	{
       printf("Login failed\n!");
	}

	}

//function di login all'applicazione
int Login (char *usernameAttuale)
{
    int opzione;
    printf("Scegli 1 per registrarti\nScegli 2 per accedere\n\n");
    scanf("%d",&opzione);

    getchar();

    if(opzione == 1)
        {
            char *registraUser = registrati();
            strcpy(usernameAttuale, registraUser);
            return 1;
        }

    else if(opzione == 2)
        {
            char *utilizzatore = login();
            strcpy(usernameAttuale, utilizzatore);
            return 1;
        }

    else
        {
            return -1;
        }

}
//function che regola la login all'applicazione tramite file system
char* login (void)
{
    char username[30],password[20];
    FILE *log;

    log = fopen("login.txt","r");
    if (log == NULL)
    {
        fputs("Errore durante apertura del file.", stderr);
        exit(1);
    }

    struct user l;

    printf("\nInserisci le tue credenziali per il login\n\n");
    printf("Username:  ");
    scanf("%s",username);
    fflush(stdin);

    printf("\nPassword: ");
    scanf("%s",password);
    fflush(stdin);


    while(fread(&l,sizeof(l),1,log))
        {
        if(strcmp(username,l.username)==0 && strcmp(password,l.password)==0)

            {
                printf("\nLogin riuscito!\n");
                char *user = l.username;
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

//altra function che regola la procedura di registrazione per la login all'applicazione tramite file system
char* registrati(void)
{
    FILE *log;

    log=fopen("login.txt","w");
    if (log == NULL)
    {
        fputs("Errore apertura file", stderr);
        exit(1);
    }


    struct user l;

    printf("\nAvvio procedura di registrazione..\n\n");

    printf("\nNome:\n");
    scanf("%s",l.nome);
    fflush(stdin);

    printf("\nCognome:\n");
    scanf("%s",l.cognome);
    fflush(stdin);

    printf("\nUsername:\n");
    scanf("%s",l.username);
    fflush(stdin);

    printf("\nPassword:\n");
    scanf("%s",l.password);
    fflush(stdin);

    srand(time(NULL));
    int sorgenteID = rand();
    l.idUser = sorgenteID;
    printf("Il suo ID utente sara': %d", l.idUser);


    fwrite(&l,sizeof(l),1,log);
    fclose(log);

    printf("\nInformazioni ricevute.\nBenvenuto, %s!\n\n",l.nome);
    getchar();

    char *user = l.username;
    printf("Registrato con successo l'utente: %s \n", user);
    return user;
}

//procedura per gestire gli ordini tramite i due ristoranti
void makeDelivery(int sockfd, char* username)
{
	char buffer[1000];
	char delivery[1000];
	int IDrider, IDorder, IDuser;
	int select_Inu, select_item, i=2;
	float amont_due=0.0;

	user userData;

	srand(time(NULL));


	for(;;)
	{
		FullRead(sockfd,buffer,sizeof(buffer)); //tramite la FullRead arriva lista ristoranti dal server
		printf("%s\n", buffer);

		scanf("%d",&select_Inu); //scelta del ristorante
		FullWrite(sockfd,(char *)&select_Inu,sizeof(select_Inu)); //tramite la FullWrite invio la scelta del ristorante al server

		if(select_Inu ==1)
		{
            //contatto DeInu
			FullRead(sockfd,buffer,sizeof(buffer)); //tramite la FullRead contatto il ristorante DeInu
			printf("%s\n",buffer);
			FullRead(sockfd,buffer,sizeof(buffer)); //tramite la FullRead ricevo il menu dal server del ristorante contattato
			printf("%s\n",buffer);

			do{
				scanf("%d",&select_item);
				FullWrite(sockfd,(char *)&select_item,sizeof(select_item));//tramite la FullWrite invio la scelta dei piatti ordinati al server

				if((select_item<0)||(select_item>9)){ // controllo sulla lista del menu'
				printf("Hai digitato una scelta errata.\n");
				}
				if(select_item != 0)
				{
					printf("Scelta %d:\n",i);//stampo a video le opzioni prescelte
					i++;
				}
				//conferma ed invio del menu prescelto e dei dati dell'user
				if(select_item==0)
				{
					while('\n'!=getchar());


					FILE *log;

                    log = fopen("login.txt","r");
                    if (log == NULL)
                        {
                            fputs("Errore durante apertura del file.", stderr);
                            exit(1);
                        }

                    struct user l;


                    while(fread(&l,sizeof(l),1,log))
                    {
                        if(strcmp(username,l.username)==0)

                        {
                            printf("Trovati i userData del user\n");
                            strcpy(userData.nome, l.nome);
                            strcpy(userData.cognome, l.cognome);
                            strcpy(userData.username, l.username);
                            userData.idUser = l.idUser;

                        }
                    else
            {
                printf("\nErrore nel recupero userData user.\n");
            }
        }

    fclose(log);


					printf("L'ordine sara' effettuato dal user: %s %s, nome utente: %s, ID utente: %d\n", userData.nome, userData.cognome, userData.username, userData.idUser);
					IDuser = userData.idUser;

                    FullWrite(sockfd,(char *)&IDuser,sizeof(IDuser));//invio user id al server
					FullRead(sockfd,(char *)&amont_due,sizeof(amont_due));

					printf("il totale dell' ordine e': %5.2f\n",amont_due);

					printf("Inserisci l'indirizzo per la consegna\n");
					fgets(userData.indirizzo,sizeof(userData.indirizzo),stdin);

					FullWrite(sockfd,(char *)&userData,sizeof(userData)); // invio la struttura userData del user al server
					//ID ordine
					FullRead(sockfd,(char *)&IDorder,sizeof(IDorder)); // ricevo order id dal server
					printf("Il tuo order ID e': %d\n",IDorder);
					//ID rider
					FullRead(sockfd,(char *)&IDrider,sizeof(IDrider)); // ricevo rider id dal server
					printf("Il codice del rider che consegnerà l'ordine è il seguente: %d\n",IDrider);

                    FullRead(sockfd,(char *)&delivery,sizeof(delivery));
                    printf("%s \n",delivery);
                    exit(0);
				}
			}while(select_item!=0);
		}
		if(select_Inu==2) // scelta del ristorante 2(JapInu),procedura sostanzialmente identica a quella intrapresa per DeInu
		{
			FullRead(sockfd,buffer,sizeof(buffer));
			printf("%s\n",buffer);
			FullRead(sockfd,buffer,sizeof(buffer));
			printf("%s\n",buffer);


			do{
				scanf("%d",&select_item);
				FullWrite(sockfd,(char *)&select_item,sizeof(select_item));
				if((select_item<0)||(select_item>9)){
				printf("Hai digitato una scelta errata.\n");
				}
				if(select_item != 0)
				{
					printf("Scelta %d:\n",i);
					i++;
				}
				if(select_item==0)
				{
					while('\n'!=getchar());

					FILE *log;

                    log = fopen("login.txt","r");
                    if (log == NULL)
                        {
                            fputs("Errore durante apertura del file.", stderr);
                            exit(1);
                        }

                    struct user l;


                    while(fread(&l,sizeof(l),1,log))
                    {
                        if(strcmp(username,l.username)==0)

                        {
                            printf("Trovati i userData del user\n");
                            strcpy(userData.nome, l.nome);
                            strcpy(userData.cognome, l.cognome);
                            strcpy(userData.username, l.username);
                            userData.idUser = l.idUser;

                        }
                    else
            {
                printf("\nErrore nel recupero userData user.\n");
            }
        }

    fclose(log);


					printf("L'ordine sara' effettuato dal user: %s %s, nome utente: %s, ID utente: %d\n", userData.nome, userData.cognome, userData.username, userData.idUser);
					IDuser = userData.idUser;

                    FullWrite(sockfd,(char *)&IDuser,sizeof(IDuser));//tramite la FullWrite invio l'ID dell'user al server

					FullRead(sockfd,(char *)&amont_due,sizeof(amont_due)); //tramite la FullRead ricevo il totale dell’ordine effettuato dal server
					printf("Totale ordine: %5.2f\n",amont_due);

					printf("Inserisci l'indirizzo per la consegna\n");  // inserisco l'indirizzo per la consegna
					fgets(userData.indirizzo,sizeof(userData.indirizzo),stdin);

					FullWrite(sockfd,(char *)&userData,sizeof(userData));// tramite la FullWrite invio i dati del cliente al server


					FullRead(sockfd,(char *)&IDorder,sizeof(IDorder));//tramite la FullRead ricevo l'ID dell'ordine effettuato dal server
					printf("Il tuo order ID: %d\n",IDorder);


					FullRead(sockfd,(char *)&IDrider,sizeof(IDrider));//tramite la FullRead ricevo l'ID dell'rider dal server
					printf("Il codice del rider che consegnerà l'ordine è il seguente: %d\n",IDrider);
                    FullRead(sockfd,(char *)&delivery,sizeof(delivery));
                    printf("%s \n",delivery);

					exit(0);
				}
			}while(select_item!=0);
		}
		if(select_Inu==3)
		{
			printf("Uscita in corso...\n");
			exit(0);
		}
	}
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


