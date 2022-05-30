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

// struct con i dati dell’utente
typedef struct user{
	char nome[30];
	char cognome[30];
	char username[30];
	char password[20];
	char indirizzo[100];
    int idUser;
} user;

// struct con i dati del corriere
typedef struct rider{
	char nome[30];
	char cognome[30];
	char username[30];
	char password[20];
   	int idRider;
} rider;

//prototipo delle funzioni
int FullRead(int sockfd, void *buffer, size_t count);
int FullWrite(int sockfd,void *buffer, size_t count);
void DeInu(int sockfd);
void JapInu(int sockfd);
void menu(int connfd);

int main()
{
	int sockfd,connfd; //Descrittori socket
	int size; //variabile di tipo intero per la dimensione del descrittore connfd
	pid_t child;  // crea un nuovo processo figlio copia esatta del processo chiamante (padre)

	struct sockaddr_in servaddr, clientaddr;  //struct di tipo sockaddr_in per il server e il client

	sockfd = socket(AF_INET,SOCK_STREAM,0);  //creazione della socket sul descrittore sockfd
	if(sockfd == -1)// controllo della socket
	{
		perror("Error!Socket not created\n");
		exit(0);
	}else{
		printf("Socket create successfully!\n");
	}

	servaddr.sin_family = AF_INET;  //Famiglia di indirizzi IPv4
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  //sin_addr contiene come indirizzo,convertito in formato network long (4 bytes), la macro INADDR_ANY,che consentirà all’applicazione di accettare connessioni da qualsiasi indirizzo associato al server sulla macchina locale.
	servaddr.sin_port = htons(5080);  //Numero di porta per la comunicazione con il client

	int enable =1; //opzione socket per riutilizzare l'ip su cui e' in ascolto il server in caso di riconnessione ante il tempo di timeout del socket
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(enable))<0)
    {
        perror("Setsocktopt failed!");
    }
    int flag =1; //opzione socket per riutilizzare la porta su cui e' in ascolto il server in caso di riconnessione ante il tempo di timeout del socket
    if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,&flag,sizeof(flag))<0)
    {
        perror("Setsocktopt failed!");
    }
	if(bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)// assegna servaddr al descrittore sockfd (assegna l'indirizzo)
	{
		perror("Error!Bind returned with error!\n");
		exit(1);
	}else{
		printf("Bind returned successfully!\n");
	}

	if((listen(sockfd,10))!=0) //mette il descrittore sockfd in ascolto
	{
		perror("Error! Listen returned with error!\n");
		exit(1);
	}else{
		printf("Server on listening…\n");
	}

	while(1)
	{
		size=sizeof(clientaddr);
		connfd = accept(sockfd,(struct sockaddr *)&clientaddr,&size); //Accetta la connessione su connfd
		if(connfd<0) //se la funzione accept ritorno un intero <0 allora vi e' stato un errore
		{
			perror("Error!Accept returned with error!\n");
			exit(1);
		}else{
			printf("Connection to the user \n");
		}
		child=fork(); //crea il processo figlio con gli identici descrittori del processo chiamante. Quest’ultimo chiude il descrittore connfd, per ritornare a mettersi in ascolto di eventuali altre richieste di connessione da altri client. Il processo figlio invece dal canto suo, chiude il descrittore sockfd e gestisce il servizio sul descrittore connfd.
		if(child==0)
		{
			close(sockfd);
			menu(connfd);
		}
	}
	close(connfd);
}
//funzione menu
void menu(int connfd)
{
	int select;
	char message[1000]=" Benvenuto! scegliere uno tra i due ristoranti: \n 1-Ristorante DeInu \n 2-Ristorante JapInu \n 3-Esci dal menu: \n";
	char message2[1000]="Ristorante DeInu contattato\n";
	char message3[1000]=" Ristorante JapInu contattato\n";

	FullWrite(connfd,(char *)&message,sizeof(message)); //tramite la FullWrite invia la lista dei ristoranti al client
	printf("message sent to the client\n");
	FullRead(connfd,(char *)&select,sizeof(select));//tramite la FullRead riceve la scelta del client
	printf("The user has chosen: %d\n",select);
	while(select!=3)
	{
		switch(select)
		{
			case 1:
			FullWrite(connfd,message2,sizeof(message2));// caso 1,ha contattato DeInu
			DeInu(connfd);
			break;
			case 2:
			FullWrite(connfd,message3,sizeof(message3));//caso 2,ha contattato JapInu
			JapInu(connfd);
			break;
			case 3: //Esce dal ciclo
			printf("Grazie di averci contattato! A presto!");
			exit(0);
			break;
			default:
			break;
		}
	}
}
void DeInu(int sockfd)
{
	int clientid,riderid,orderid;
	int sock,select;
	float basket = 0.0;
	user dati;
	char delivery[1000];
	struct sockaddr_in servaddr;
	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock==-1)
	{
		perror("Error! Socket not created!\n");
		exit(1);
	}else{
		printf("Socket created successfully!\n");
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // assegniamo al server l'indirizzo IP 127.0.0.1,   ovvero,l’indirizzo dell'interfaccia di loopback
	servaddr.sin_port= htons(2079); // porta del ristorante DeInu

	if(connect(sock,(struct sockaddr *)&servaddr,sizeof(servaddr))!=0)
	{
		perror ("Connection to the restaurant failed!\n");
		exit(1);
	}else{
		printf("Connected to the restaurant!\n");
	}

	char buffer[1000];
	FullRead(sock,buffer,sizeof(buffer)); // riceve dal ristorante il menu’
	printf("Menu received\n");
	FullWrite(sockfd,buffer,sizeof(buffer)); //tramite la FullWrite invia all’ user il menù
	printf("Menu sent to client\n");
	do{ // ciclo per continuare ad aggiungere al carrello gli ordini
		FullRead(sockfd,(char *)&select,sizeof(select));
		printf("\n%d",select);
		FullWrite(sock,(char *)&select,sizeof(select));
	}while(select!=0);

    printf("Order completed!\n");
	//ID USER
    FullRead(sockfd,(char *)&clientid,sizeof(clientid)); //tramite la FullRead riceve l'ID dell’user dal client
    printf("USER ID : %d\n",clientid);
	FullWrite(sock,(char *)&clientid,sizeof(clientid));//tramite la FullWrite invia l'ID dell'user al ristorante
	//CARRELLO
	FullRead(sock,(char *)&basket,sizeof(basket)); //tramite la FullRead riceve il carrello dal ristorante
	printf("Totale ordine: %5.2f\n",basket);
	FullWrite(sockfd,(char *)&basket,sizeof(basket)); //tramite la FullWrite invia il carrello al client
	//DATI USER
	FullRead(sockfd,(char *)&dati,sizeof(dati)); //tramite la FullRead riceve dati dell' user dal client
	printf("Nome user: %s\nCognome user: %s\nIndirizzo di consegna: %s\n",dati.nome,dati.cognome,dati.indirizzo);
	FullWrite(sock,(char *)&dati,sizeof(dati));//tramite la FullWrite invia i dati dell' user al ristorante
	//ID ORDINE
	FullRead(sock,(char *)&orderid,sizeof(orderid)); //tramite la FullRead riceve l'ID dell'ordine effettuato dal ristorante
	FullWrite(sockfd,(char *)&orderid,sizeof(orderid));//tramite la FullWrite invia l'ID dell' ordine effettuato al client
	FullRead(sock,(char *)&riderid,sizeof(riderid));//tramite la FullRead riceve l'ID rider dal ristorante
	FullWrite(sockfd,(char *)&riderid,sizeof(riderid));//tramite la FullWrite invia l'ID del rider al client

	//CONFERMA DELIVERY
	FullRead(sock,delivery,sizeof(delivery)); //tramite la FullRead riceve la conferma della consegna dal ristorante
	printf("DELIVERY letta: %s \n", delivery);
    	char Dchecker[1000] = "Lo stato della consegna è OK: Consegnato";
	if( strcmp(delivery, Dchecker) == 0 )
	{
        char YDelivery[1000] = "Consegna effettuata! Trovera' l'ordine davanti alla sua abitazione: consegna senza contatto.\n";
        printf("The rider delivered the order !\n");
        FullWrite(sockfd,(char *)&YDelivery,sizeof(YDelivery));
	}
	else {
            char NoDelivery[1000] = "Si e' verificato un problema durante la consegna, riprovare in seguito.";
            printf("A problem occurred during the delivery, please try later.\n");
            FullWrite(sockfd,(char *)&NoDelivery,sizeof(NoDelivery));
            }
	close(sock);
      exit(0);
}

void JapInu(int sockfd)
{
	int clientid,riderid,orderid;
	int sock,select;
	float basket = 0.0;
	user dati;
	char delivery[1000];
	struct sockaddr_in servaddr;
	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock==-1)
	{
		perror("Error! Socket not created!\n");
		exit(1);
	}else{
		printf("Socket created successfully!\n");
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // assegniamo al server l'indirizzo IP 127.0.0.1,   ovvero,l’indirizzo dell'interfaccia di loopback
	servaddr.sin_port= htons(2090); // porta del ristorante JapInu

	if(connect(sock,(struct sockaddr *)&servaddr,sizeof(servaddr))!=0)
	{
		perror ("Connection to the restaurant failed!\n");
		exit(1);
	}else{
		printf("Connected to the restaurant!\n");
	}

	char buffer[1000];
	FullRead(sock,buffer,sizeof(buffer)); // riceve dal ristorante il menu’
	printf("Menu received\n");
	FullWrite(sockfd,buffer,sizeof(buffer)); //tramite la FullWrite invia all’ user il menù
	printf("Menu sent to client\n");
	do{ // ciclo per continuare ad aggiungere al carrello gli ordini
		FullRead(sockfd,(char *)&select,sizeof(select));
		printf("\n%d",select);
		FullWrite(sock,(char *)&select,sizeof(select));
	}while(select!=0);

    printf("Order completed!\n");
	//ID USER
    FullRead(sockfd,(char *)&clientid,sizeof(clientid)); //tramite la FullRead riceve l'ID dell’user dal client
    printf("USER ID : %d\n",clientid);
	FullWrite(sock,(char *)&clientid,sizeof(clientid));//tramite la FullWrite invia l'ID dell'user al ristorante
	//CARRELLO
	FullRead(sock,(char *)&basket,sizeof(basket)); //tramite la FullRead riceve il carrello dal ristorante
	printf("Totale ordine: %5.2f\n",basket);
	FullWrite(sockfd,(char *)&basket,sizeof(basket)); //tramite la FullWrite invia il carrello al client
	//DATI USER
	FullRead(sockfd,(char *)&dati,sizeof(dati)); //tramite la FullRead riceve dati dell' user dal client
	printf("Nome user: %s\nCognome user: %s\nIndirizzo di consegna: %s\n",dati.nome,dati.cognome,dati.indirizzo);
	FullWrite(sock,(char *)&dati,sizeof(dati));//tramite la FullWrite invia i dati dell' user al ristorante
	//ID ORDINE
	FullRead(sock,(char *)&orderid,sizeof(orderid)); //tramite la FullRead riceve l'ID dell'ordine effettuato dal ristorante
	FullWrite(sockfd,(char *)&orderid,sizeof(orderid));//tramite la FullWrite invia l'ID dell' ordine effettuato al client
	FullRead(sock,(char *)&riderid,sizeof(riderid));//tramite la FullRead riceve l'ID rider dal ristorante
	FullWrite(sockfd,(char *)&riderid,sizeof(riderid));//tramite la FullWrite invia l'ID del rider al client

	//CONFERMA DELIVERY
	FullRead(sock,delivery,sizeof(delivery)); //tramite la FullRead riceve la conferma della consegna dal ristorante
	printf("DELIVERY letta: %s \n", delivery);
    	char Dchecker[1000] = "Lo stato della consegna è OK: Consegnato";
	if( strcmp(delivery, Dchecker) == 0 )
	{
        char YDelivery[1000] = "Consegna effettuata! Trovera' l'ordine davanti alla sua abitazione: consegna senza contatto.\n";
        printf("The rider delivered the order !\n");
        FullWrite(sockfd,(char *)&YDelivery,sizeof(YDelivery));
	}
	else {
            char NoDelivery[1000] = "Si e' verificato un problema durante la consegna, riprovare in seguito.";
            printf("A problem occurred during the delivery, please try later.\n");
            FullWrite(sockfd,(char *)&NoDelivery,sizeof(NoDelivery));
            }
	close(sock);
      exit(0);
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


