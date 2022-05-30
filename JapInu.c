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

//struct con i dati dell’utente
typedef struct user{
    char nome[30];
    char cognome[30];
    char username[30];
    char password[20];
    char indirizzo[100];
    int idUser;
} user;

//prototipi delle funzioni
int FullRead(int sockfd, void *buffer, size_t count);
int FullWrite(int sockfd,void *buffer, size_t count);
void rider(int sockfd);
void send_menu(int sockfd);

int main()
{
    int sockfd,connfd,size;
    struct sockaddr_in servaddr, clientaddr;
    pid_t child;

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
        perror("Socket creation failed!");
        exit(1);
    }else{
        printf("Socket created successfully!\n");
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(2090);// numero porta del ristorante JapInu

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

    if((bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)))!=0)
    {
        perror("Socket bind failed!\n");
        exit(1);
    }else{
        printf("Socket bind returned successfully!\n");
    }
    fflush(stdin);
        while(1)
        {
            if((listen(sockfd,10)) != 0)
            {
                perror("Listen failed!\n");
                exit(1);
                }else{
                    printf("JapInu listening..\n");
                }

            size = sizeof(clientaddr);
            connfd=accept(sockfd,(struct sockaddr *)&clientaddr,&size);
            if(connfd<0)
            {
                perror("Accept returned with error!\n");
                exit(1);
            }else{
                printf("JapInu has accepted the client!\n");
            }
            child=fork();
            if(child==0)
            {
                close(sockfd);
                send_menu(connfd);
            }
        }
        close(connfd);

}

void send_menu(int sockfd)
{
	int select_item,len,connfd;
	float basket = 0.0;
	char message[1000]="Go aisatsu! Questo è il menu' : \n1: Tempura con Soba [12.00 euro] \n2: Kitsune Udon [15.50 euro] \n3: Tempura Udon [10.50 euro] \n4: Ramen [8.00 euro] \n5: Yakitori [7.80 euro] \n6: Sukiyaki [10.20 euro] \n7: Acqua Liscia [2.00 euro] \n8: Acqua Frizzante [2.50 euro] \n9: Sake Akashi-Tai Honjozo [19.00 euro] \nDigita 0 per confermare l'ordine\nScelta 1:";

	FullWrite(sockfd,message,sizeof(message));
	printf("the user has chosen:");
	do{
		FullRead(sockfd,(char *)&select_item,sizeof(select_item));
		printf("%d\n",select_item);
		switch(select_item)
		{
			case 1:
            basket+=12.00;
            break;
            case 2:
            basket+=15.50;
            break;
            case 3:
            basket+=10.50;
            break;
            case 4:
            basket+=8.00;
            break;
            case 5:
            basket+=7.80;
            break;
            case 6:
            basket+=10.20;
            break;
            case 7:
            basket+=2.00;
            break;
            case 8:
            basket+=2.50;
            break;
            case 9:
            basket+=19.00;
            break;
			case 0:
			printf("Total Basket is : %5.2f euro\n",basket);
			FullWrite(sockfd,(char *)&basket,sizeof(basket));
			rider(sockfd);
			break;
		}
	}while(select_item!=0);
}

void rider(int sockfd)
{
    int clientid,riderid,orderid,len,sock,connfd,delivery;
    user dati;
    char deliveryok[1000];
    srand(time(NULL));
    orderid=rand();
    char message[1000]="\nSalve!E'presente una consegna da effettuare:";
    pid_t child;
    struct sockaddr_in JapInu,rider;
    sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        printf("Error!Socket creation failed!\n");
        exit(0);
    }else{
        printf("Socket created successfully!!\n");
    }
    JapInu.sin_family = AF_INET;
    JapInu.sin_addr.s_addr = inet_addr("127.0.0.1");
    JapInu.sin_port = htons(5586); // porta del rider JapRider

    int enable =1; //opzione socket per riutilizzare l'ip su cui e' in ascolto il server in caso di riconnessione ante il tempo di timeout del socket
    if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(enable))<0)
    {
        perror("Setsocktopt failed!");
    }

    int flag =1; //opzione socket per riutilizzare la porta su cui e' in ascolto il server in caso di riconnessione ante il tempo di timeout del socket
    if(setsockopt(sock,SOL_SOCKET,SO_REUSEPORT,&flag,sizeof(flag))<0)
    {
        perror("Setsocktopt failed!");
    }

    if(bind(sock,(struct sockaddr *)&JapInu,sizeof(JapInu))!=0)
    {
        perror("Error! Socket bind returned with error!\n");
        exit(1);
    }else{
        printf("Socket bind returned successfully!\n");
    }
    if((listen(sock,10))!=0)
    {
        perror("Error! Listen returned with error!\n");
        exit(1);
    }else{
        printf("JapInu listening to the rider\n");
    }
    while(1)
    {
        len=sizeof(rider);
        connfd=accept(sock,(struct sockaddr *)&rider,&len);
        if(connfd<0)
        {
            perror("Error! Accept function returned with error!\n");
            exit(1);
        }else{
            printf("JapInu has accepted the rider!\n");
        }
        child=fork();
        if(child==0)
        {
            close(sock);
            FullWrite(connfd,message,sizeof(message));
            FullRead(connfd,(char *)&delivery,sizeof(delivery));
            if(delivery==1)
            {
                //ID USER
                fflush(stdin);
                fflush(stdout);
                FullRead(sockfd,(char *)&clientid,sizeof(clientid));
                FullWrite(connfd,(char *)&clientid,sizeof(clientid));
                //DATI USER
                FullRead(sockfd,(char *)&dati,sizeof(dati));
                fflush(stdin);
                fflush(stdout);
                printf("Nome user: %s\nCognome user: %s\nIndirizzo di consegna: %s\n",dati.nome,dati.cognome,dati.indirizzo);
                FullWrite(connfd,(char *)&dati,sizeof(dati));
                //ID ORDINE
                FullWrite(connfd,(char *)&orderid,sizeof(orderid));
                FullWrite(sockfd,(char *)&orderid,sizeof(orderid));
                //ID RIDER
                FullRead(connfd,(char *)&riderid,sizeof(riderid));
                FullWrite(sockfd,(char *)&riderid,sizeof(riderid));
                //CONFERMA DELIVERY
                FullRead(connfd,deliveryok,sizeof(deliveryok));
                FullWrite(sockfd,deliveryok,sizeof(deliveryok));
                printf("DELIVERYOK: %s \n", deliveryok);
                exit(1);
            }else{
                printf("Sorry!The rider is not available!Try later!\n");
                exit(0);
            }
        }
    }
    close(connfd);
    close(sockfd);
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
    int byte_written=0;
    nleft=count;
    while(nleft>0)
    {
        if((nwritten=write(sockfd,buffer,nleft))<0)
        {
            if(errno==EINTR)
            {
                continue;
            }else{
                return(byte_written +nwritten);
            }
        }
        nleft=nleft-nwritten;
        buffer+=nwritten;
        byte_written = byte_written +nwritten;
    }
    return(byte_written);
}
