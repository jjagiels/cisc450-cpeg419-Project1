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
#include <arpa/inet.h>      /* for htonl and ntohl */

#define BUFF_SIZE 1024

struct Buffer {
	char ok;		/* Validates the struct */
	char directive;         /* informs server of action user wishes to take */
	char account1;		/* 0 or 1 for Checkings or Savings, respectively */
	char account2;		/* 0 or 1 for Checkings or Savings, respectively */
	int amount;			/* Amount to be deposited, withdrawn or transfered */
};




int main(void) {

   int sock_client;  /* Socket used by client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   struct hostent * server_hp;      /* Structure to store server's IP
                                        address */
                                        
   struct Buffer buffer = {'0','I','3','3',0};
   struct Buffer returnBuffer = {'0','I','3','3',0};
                                        
   enum state{main, checkBalance, deposit, withdraw, transfer, quit}menu;
   enum input{C,D,W,T,Q}userSelection;
   int accountID = 3;
   menu = main;
   char server_hostname[BUFF_SIZE]; /* Server's hostname */
   unsigned short server_port;  /* Port number used by server (remote port) */

   char selection = 'I';  /* sent command to server */
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
            printf("Please select an action:\nCheck the balance of an account: C\nDeposit an amount into an account: D\nWithdraw an amount from an account: W\nTransfer an amount from one account to another: T\nDisconnect from the server: Q\n:");
            scanf("%s", &selection);
        
            selection = toupper(selection);
            
            userSelection = selection;
        
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
                case 'Q':
                    menu = quit;
                default:
                    printf("Invalid selection, please try again\n");
                    menu = main;
                    break;
            }
            continue;
            break;
          case checkBalance:{
              char acct;
              //TODO: Ask for an account name to be specified, and this should prepare the sent packet to send a check balance request for the specified account name, and should do not spelling or other checks
              printf("\nPlease select:\n\nChecking account: 0\nSavings account: 1\n:");
              scanf("%c", &acct);
              buffer.directive = 'C';
              buffer.account1 = (char)acct;
              buffer.ok = '1';
              break;
          }
          case deposit:{
              //TODO: Ask for an account name to be specified, and do no checks on the name of the account as above; also ask for a number to be added to the specified account
			  char acct;
			  int amt;
			  printf("\nPlease select:\n\nChecking account: 0\n Savings account: 1\n:");
			  scanf("%c", &acct);
			  printf("\nPlease enter amount to be deposited:\n");
			  scanf("%d", &amt);
			  buffer.ok = 1;
			  buffer.directive = 'D';
			  buffer.account1 = acct;
			  buffer.amount = htonl(amt);
			  break;
          }
		  case withdraw:{
			  //TODO: Ask for an account name to be specified, and do no checks on the name of the account as above; also ask for a number to be withdrawn. *important* specify that the amount should be in $20 intervals, but do not check
			  int amt;
			  printf("\nPlease enter amount to be deposited into Checkings (use only $20 incerements):\n");
				  scanf("%d", &amt);
			  buffer.ok = 1;
			  buffer.directive = 'W';
			  buffer.account1 = 0;
			  buffer.amount = htonl(amt);
			  break;
		  }
          case transfer:{
              //TODO: Ask for an original account and an account to transfer to, then ask for an amount to be tranfered (in whole dollar amounts), do not check any value
              char acct1;
              char acct2;
              int amt;
              
              printf("\nPlease select which account you are tranferring FROM:\n\nChecking account: 0\n Savings account: 1\n:");
              scanf("%c", &acct1);
              printf("\nPlease select which account you are tranferring TO:\n\nChecking account: 0\n Savings account: 1\n:");
              scanf("%c", &acct2);
              printf("\nPlease enter amount to be tranferred\n:");
              scanf("%d", &amt);
              
              buffer.directive = 'T';
              buffer.account1 = acct1;
              buffer.account2 = acct2;
              buffer.amount = htonl(amt);
              buffer.ok = '1';
              break;
          }
          case quit:{
              char input;
              //TODO: Code Here
              close (sock_client);
              printf("\nDo you wish to reconnect? (y/n)\n:");
              scanf("%c",&input);
              if(input == 'y' || input == 'Y'){
                  //TODO: Move connection to its own function, and call that function here
                menu = main;
                continue;
              }
              else{
                  exit(0);
              }
              break;
          }
          default:
              printf("menu enum not working correctly! Fix it!");
              exit(0);
              break;
      }
   }
   
   /* send message */
   
   bytes_sent = send(sock_client, &buffer, msg_len, 0);

   /* get response from server */
  
   bytes_recd = recv(sock_client, &returnBuffer, BUFF_SIZE, 0); 

   printf("\nThe response from server is:\n");
   printf("%s\n\n", &returnBuffer.ok);

   /* close the socket */

   close (sock_client);
}
