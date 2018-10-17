/* tcpserver.c */
/* Programmed by Adarsh Sethi */
/* Sept. 13, 2018 */    

#include <ctype.h>          /* for toupper */
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset */
#include <sys/socket.h>     /* for socket, bind, listen, accept */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define BUFF_SIZE 1024   

struct Buffer {
	/* Values from the client */
	char ok;		/* Validates the struct */
	char directive;         /* informs server of action user wishes to take */
	char account1;		/* 0 or 1 for Checkings or Savings, respectively, used to dictate the account used for checking balance, depositing, withdrawing, or the origin account for transfers*/
	char account2;		/* 0 or 1 for Checkings or Savings, respectively, used to dictate the target account for transfers */
	int amount;			/* Amount to be deposited, withdrawn or transfered */
	
	
	/* Values from the server */
	char message[15];   /* Error message from the server */
	int beforeAmount;	/* Value used to transmit the amount of money in an account before a transaction */
	int afterAmount;	/* Value used to transmit the amount of money in an account after a transaction */
}recvBuffer;

/*

DESCRIPTION OF ERROR CODES:

------------------------------------------
|| CODE ||            MEANING           ||
------------------------------------------
||  '0' || No Error                     ||
------------------------------------------
||  'A' || Invalid message              ||
------------------------------------------
||  'B' || Withdraw from Savings        ||
------------------------------------------
||  'C' || Overdraw from account        ||
------------------------------------------
||  'D' || Withdraw not divisible by 20 ||
------------------------------------------
||  'E' || Invalid account selected     ||
------------------------------------------

*/

/* SERV_TCP_PORT is the port number on which the server listens for
   incoming requests from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

#define SERV_TCP_PORT 20987

int main(void) {

   int sock_server;  /* Socket on which server listens to clients */
   int sock_connection;  /* Socket on which server exchanges data with client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   unsigned int server_addr_len;  /* Length of server address structure */
   unsigned short server_port;  /* Port number used by server (local port) */

   struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */
   unsigned int client_addr_len;  /* Length of client address structure */

   char sentence[BUFF_SIZE];  /* receive message */
   char modifiedSentence[BUFF_SIZE]; /* send message */
   unsigned int msg_len;  /* length of message */
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
   unsigned int i;  /* temporary loop variable */
   
   unsigned int checkingBalance = 0; /* amount of money in the Checking account */
   unsigned int savingsBalance = 0; /* amount of money in the savings account */

   /* open a socket */

   if ((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Server: can't open stream socket");
      exit(1);                                                
   }

   /* initialize server address information */
    
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */ 
   server_port = SERV_TCP_PORT; /* Server will listen on this port */
   server_addr.sin_port = htons(server_port);

   /* bind the socket to the local server port */

   if (bind(sock_server, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      perror("Server: can't bind to local address");
      close(sock_server);
      exit(1);
   }                     

   /* listen for incoming requests from clients */

   if (listen(sock_server, 50) < 0) {    /* 50 is the max number of pending */
      perror("Server: error on listen"); /* requests that will be queued */
      close(sock_server);
      exit(1);
   }
   printf("I am here to listen ... on port %hu\n\n", server_port);
  
   client_addr_len = sizeof (client_addr);

   /* wait for incoming connection requests in an indefinite loop */

   for (;;) {

      sock_connection = accept(sock_server, (struct sockaddr *) &client_addr, 
                                         &client_addr_len);
                     /* The accept function blocks the server until a
                        connection request comes from a client */
      if (sock_connection < 0) {
         perror("Server: accept() error\n"); 
         close(sock_server);
         exit(1);
      }
 
      /* receive the message */

      bytes_recd = recv(sock_connection, &recvBuffer, BUFF_SIZE, 0);

      if (bytes_recd > 0){
          /* Old recieve function */
         //printf("Received Sentence is:\n");
         //printf("%s", sentence);
         //printf("\nwith length %d\n\n", bytes_recd);
         
         if(recvBuffer.ok == 0){
         
             //TODO: Return an "incorrect message" error back to the client
        }
        
        //convert the amount int from the network to host long value
        ntohl(recvBuffer.amount);
		
		/* print relevant information from the client's message */
		
		switch(recvBuffer.directive){
        
                case 'C':
                    printf("User has selected the option: Check Balance\n");
                    break;
                case 'D':
                    printf("User has selected the option: Deposit\n");
                    break;
                case 'W':
                    printf("User has selected the option: Withdraw\n");
                    break;
                case 'T':
                    printf("User has selected the option: Transfer\n");
                    break;
                case 'Q':
                    printf("User has disconnected\n");
                default:
                    printf("User has made an invalid selection of directive\n");
                    break;
        }
		
		switch(recvBuffer.account1){
        
                case '0':
                    printf("User has selected the account: Checking\n");
                    break;
                case '1':
                    printf("User has selected the account: Savings\n");
                    break;
                default:
                    printf("User has made an invalid selection of account\n");
                    break;
        }
		
		printf("The user has specified %d as the amount to use for this transaction\n\n", recvBuffer.amount);
		
		/* add some newlines for readability*/
		
		printf("\n\n\n");
		
        
        switch(recvBuffer.directive){
        
            case 'C':{
            
                //TODO: The server must check the amount of money stored in the account requested, and return that amount to the client
				if(recvBuffer.account1 == '0'){ //The user has selected the Checking account
				
					recvBuffer.beforeAmount = checkingBalance;
				}
				else if(recvBuffer.account1 == '1'){ //The user has selected the Savings account
				
					recvBuffer.beforeAmount = savingsBalance;
				}
				else{ //Neither the checking or savings account was selected, and an error must be returned
				
					recvBuffer.message[0] = 'E';
				}
                break;
            }
            
            case 'D':{
            
                //TODO: The server must add the number from recvBuffer.amount to the amount stored in the requested account's balance
				if(recvBuffer.account1 == '0'){ //The user has selected the Checking account
				
					recvBuffer.beforeAmount = checkingBalance; //Store the amount in checking before the transaction
					checkingBalance += recvBuffer.amount; 
					recvBuffer.afterAmount = checkingBalance;
				}
				else if(recvBuffer.account1 == '1'){ //The user has selected the Savings account
				
					recvBuffer.beforeAmount = savingsBalance;
					savingsBalance += recvBuffer.amount;
					recvBuffer.afterAmount = savingsBalance;
				}
				else{ //Neither the checking or savings account was selected, and an error must be returned
				
					recvBuffer.message[0] = 'E';
				}
                break;
            }
            case 'W':{
            
                //TODO: The server must remove the number from recvBuffer.amount from the amount stored in the requested account's balance. This will return an error if recvBuffer.amount > balance
				if(recvBuffer.account1 == '0'){ //The user has selected the Checking account
				
					if((checkingBalance - recvBuffer.amount) < 0 ){ //User will overdraft the checking account, server must return an error
					
						recvBuffer.message[0] = 'C';
						break;
					}
					if((recvBuffer.amount % 20) != 0){ //The requested withdraw amount is not a multiple of 20, and server must return an error
					
						recvBuffer.message[0] = 'D';
						break;
					}
					
					recvBuffer.beforeAmount = checkingBalance; //Store the amount in checking before the transaction
					checkingBalance -= recvBuffer.amount; 
					recvBuffer.afterAmount = checkingBalance;
				}
				else if(recvBuffer.account1 == '1'){ //The user has selected the Savings account, however this is disallowed for withdrawl, so server must return an error
				
					recvBuffer.message[0] = 'B';
					break;
				}
				else{ //Neither the checking or savings account was selected, and an error must be returned
				
					recvBuffer.message[0] = 'E';
				}
                break;
            }
            case 'T':{
            
                //TODO: The server must first remove the number from recvBuffer.amount from the amount stored in the first requested account's balance and then add that amount to the amount stored in the second account's balance. This will return an error if recvBuffer.amount > account1Balance
				
				if(recvBuffer.account1 == '0'){ //The user has selected the Checking account to transfer from, thus we need to transfer to savings
				
					if((checkingBalance - recvBuffer.amount) < 0 ){ //User will overdraft the checking account, server must return an error
					
						recvBuffer.message[0] = 'C';
						break;
					}
				
					recvBuffer.beforeAmount = savingsBalance;
					checkingBalance -= recvBuffer.amount;
					savingsBalance += recvBuffer.amount;
					recvBuffer.afterAmount = savingsBalance;
				}
				else if(recvBuffer.account1 == '1'){ //The user has selected the Savings account to transfer from, thus we need to transfer to checking
				
					if((savingsBalance - recvBuffer.amount) < 0 ){ //User will overdraft the savings account, server must return an error
					
						recvBuffer.message[0] = 'C';
						break;
					}
				
					recvBuffer.beforeAmount = checkingBalance;
					savingsBalance -= recvBuffer.amount;
					checkingBalance += recvBuffer.amount;
					recvBuffer.afterAmount = checkingBalance;
				}
				else{ //Neither the checking or savings account was selected, and an error must be returned
				
					recvBuffer.message[0] = 'E';
				}
                break;
            }
            case 'Q':{
				printf("Server will not respond to client, as client has disconnected\n");
                //TODO: The client has disconnected
				break;
            }
            default:
                //TODO: return an error to the client
				break;
        }
		
		/*Print out the information from the response */
		
		printf("Server will respond with:\n");
		printf("Amount stored in selected account before transaction: %d\n", recvBuffer.beforeAmount);
		printf("Amount stored in selected account after transaction: %d\n", recvBuffer.afterAmount);
		printf("Error Code: %c\n", recvBuffer.message[0]);

        /* prepare the message to send */

         msg_len = bytes_recd;

         /* send message */
 
         bytes_sent = send(sock_connection, &recvBuffer, msg_len, 0);
      }

      /* close the socket */

      close(sock_connection);
   } 
}
