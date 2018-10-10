/* tcp_ client.c */ 
/* Programmed by Adarsh Sethi */
/* Modified by Justin Jagielski and Marco Arcilla */
/* Sept. 13, 2018 */     

#include <stdio.h>          /* for standard I/O functions */
#include <ctype.h>          /* for toupper() function */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, connect, send, and recv */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define STRING_SIZE 1024

int main(void) {

   int sock_client;  /* Socket used by client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   struct hostent * server_hp;      /* Structure to store server's IP
                                        address */
                                        
   enum state{main, checkBalance, deposit, withdraw, transfer}menu;
   enum input{C,D,W,T}userSelection;
   menu = main;
   char server_hostname[STRING_SIZE]; /* Server's hostname */
   unsigned short server_port;  /* Port number used by server (remote port) */

   char selection[1];  /* sent command to server */
   
   char sentence[STRING_SIZE];
   char modifiedSentence[STRING_SIZE]; /* receive message */
   unsigned int msg_len;  /* length of message */                      
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
  
   /* open a socket */

   if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Client: can't open stream socket");
      exit(1);
   }

   /* Note: there is no need to initialize local client address information 
            unless you want to specify a specific local port
            (in which case, do it the same way as in udpclient.c).
            The local address initialization and binding is done automatically
            when the connect function is called later, if the socket has not
            already been bound. */

   /* initialize server address information */

   printf("Enter hostname of server: ");
   scanf("%s", server_hostname);
   if ((server_hp = gethostbyname(server_hostname)) == NULL) {
      perror("Client: invalid server hostname");
      close(sock_client);
      exit(1);
   }

   printf("Enter port number for server: ");
   scanf("%hu", &server_port);

   /* Clear server address structure and initialize with server address */
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
                                    server_hp->h_length);
   server_addr.sin_port = htons(server_port);

    /* connect to the server */
 		
   if (connect(sock_client, (struct sockaddr *) &server_addr, 
                                    sizeof (server_addr)) < 0) {
      perror("Client: can't connect to server");
      close(sock_client);
      exit(1);
   }
  
   /* original user interface */

   /*printf("Please input a sentence:\n");
   scanf("%s", sentence);
   msg_len = strlen(sentence) + 1; */
   
   /* our banking user interface */
   while(1){
      switch(menu){
          case main:
            printf("Please select an action:\nCheck the balance of an account: C\nDeposit an amount into an account: D\nWithdraw an amount from an account: W\nTransfer an amount from one account to another: T\n:");
            scanf("%s", selection);
        
            toupper(selection[0]); //TODO: This is not forcing the input to an uppercase letter.
            
            userSelection = selection[0];
        
            switch(userSelection){
        
                case 'C':
                    menu = checkBalance;
                    break;
                case 'D':
                    menu = deposit;
                    break;
                case 'W':
                    menu = withdraw;
                    break;
                case 'T':
                    menu = transfer;
                    break;
                default:
                    printf("Invalid selection, please try again");
                    menu = main;
                    break;
            }
            break;
          case checkBalance:
              //TODO: Code Here
              break;
          case deposit:
              //TODO: Code Here
              break;
          case withdraw:
              //TODO: Code Here
              break;
          case transfer:
              //TODO: Code Here
              break;
          default:
              printf("menu enum not working correctly! Fix it!");
              exit(0);
              break;
      }
   }
   
   /* send message */
   
   bytes_sent = send(sock_client, sentence, msg_len, 0);

   /* get response from server */
  
   bytes_recd = recv(sock_client, modifiedSentence, STRING_SIZE, 0); 

   printf("\nThe response from server is:\n");
   printf("%s\n\n", modifiedSentence);

   /* close the socket */

   close (sock_client);
}
