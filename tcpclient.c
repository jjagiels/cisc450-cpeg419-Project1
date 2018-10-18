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
#define BLUE "\x1b[36m"
#define RESET "\x1b[0m"

int sock_client;  /* Socket used by client */

struct sockaddr_in server_addr;  /* Internet address structure that
stores server address */
struct hostent * server_hp;      /* Structure to store server's IP
address */

char server_hostname[BUFF_SIZE]; /* Server's hostname */
unsigned short server_port;  /* Port number used by server (remote port) */

typedef enum{
    
    mainMenu = 0,
    checkBalance,
    deposit,
    withdraw,
    transfer,
    quit,
} state;

typedef enum{
    
    C = 0,
    D,
    W,
    T,
    Q,
} input;

struct Buffer {
    /* Values from the client */
    char ok;		/* Validates the struct */
    char directive;         /* informs server of action user wishes to take */
    char account1;		/* 0 or 1 for Checkings or Savings, respectively, used to dictate the account used for checking balance, depositing, withdrawing, or the origin account for transfers*/
    //char account2;		/* 0 or 1 for Checkings or Savings, respectively, used to dictate the target account for transfers */
    int amount;			/* Amount to be deposited, withdrawn or transfered */
    
    
    /* Values from the server */
    char message;   /* Error message from the server */
    int beforeAmount;	/* Value used to transmit the amount of money in an account before a transaction */
    int afterAmount;	/* Value used to transmit the amount of money in an account after a transaction */
};

void SendFunc(struct Buffer msg, int sock_client, input userSelection){
    int bytes_sent;
    /* send message */
    
    bytes_sent = send(sock_client, &msg, sizeof(msg), 0);
    printf(BLUE "\nMessage validation (0 for NOT VALID, 1 for VALID) for SEND: %c\n"RESET, msg.ok);
    switch(userSelection){
        
        case 'C':
            printf(BLUE "Transaction type: Check Balance\n" RESET);
            break;
        case 'D':
            printf(BLUE "Transaction type: Deposit\n" RESET);
            break;
        case 'W':
            printf(BLUE "Transaction type: Withdraw\n" RESET);
            break;
        case 'T':
            printf(BLUE "Transaction type: Transaction\n" RESET);
            break;
        case 'Q':
            printf(BLUE "Transaction type: Disconnect\n" RESET);
            break;
        default:
            printf(BLUE "Invalid selection, please try again\n" RESET);
            break;
    }
    printf(BLUE "Account used (0 for Checkings, 1 for Savings): %c\n" RESET, msg.account1);
    //printf("Account 2 used (0 for Checkings, 1 for Savings): %c\n", msg.account2);
    printf(BLUE "Amount (in USD) expected: $%d\n" RESET, ntohl(msg.amount)); 
    printf(BLUE "MESSAGE LENGTH: %d bytes\n\n" RESET, bytes_sent);
}

void RecvFunc(struct Buffer msg, int sock_client, state menu) {
    int bytes_recd; /* number of bytes sent or received */
    /* get response from server */
    
    bytes_recd = recv(sock_client, &msg, sizeof(msg), 0);
    if(bytes_recd == 0){ //connection was interrupted, print an error and return
        
        printf(BLUE "connection closed abruptly, returning...\n" RESET);
        return;
    }
    printf(BLUE "Message validation (0 for NOT VALID, 1 for VALID) for RECEIVE: %c\n" RESET, msg.ok);
    //printf("Transaction type: %c\n", menu);
    //printf("Account used (0 for Checkings, 1 for Savings): %c\n", msg.account1);
    printf(BLUE "Error message from server: %c\n" RESET, msg.message);
    //printf("Account 2 used (0 for Checkings, 1 for Savings): %c\n", msg.account2);
    printf(BLUE "Amount (in USD) in account before Transaction: $%d\n" RESET, ntohl(msg.beforeAmount));
    printf(BLUE "Amount (in USD) in account after Transaction: $%d\n" RESET, ntohl(msg.afterAmount));
    printf(BLUE "MESSAGE LENGTH: %d bytes\n\n" RESET, bytes_recd);
}

void clientConnect(){
    
    /* open a socket */
    
    if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("Client: can't open stream socket");
        exit(1);
    }
    
    /* Note: there is no need to initialize local client address information 
     *                            unless you want to specify a specific local port
     *                            (in which case, do it the same way as in udpclient.c).
     *                            The local address initialization and binding is done automatically
     *                            when the connect function is called later, if the socket has not
     *                            already been bound. */
    
    /* initialize server address information */
    
    printf(BLUE "Enter hostname of server: " RESET);
    scanf("%s", server_hostname);
    if ((server_hp = gethostbyname(server_hostname)) == NULL) {
        perror("Client: invalid server hostname");
        close(sock_client);
        exit(1);
    }
    
    printf(BLUE "Enter port number for server: " RESET);
    scanf("%hu", &server_port);
    
    /* Clear server address structure and initialize with server address */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
           server_hp->h_length);
    server_addr.sin_port = htons(server_port);
    
    /* connect to the server */
    if (connect(sock_client, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0) {
        perror("Client: can't connect to server");
        close(sock_client);
        exit(1);
    }
}

/*
 * 
 * DESCRIPTION OF ERROR CODES:
 * 
 * ------------------------------------------
 * || CODE ||            MEANING           ||
 * ------------------------------------------
 * ||  '0' || No Error                     ||
 * ------------------------------------------
 * ||  'A' || Invalid message              ||
 * ------------------------------------------
 * ||  'B' || Withdraw from Savings        ||
 * ------------------------------------------
 * ||  'C' || Overdraw from account        ||
 * ------------------------------------------
 * ||  'D' || Withdraw not divisible by 20 ||
 * ------------------------------------------
 * ||  'E' || Invalid account selected     ||
 * ------------------------------------------
 * 
 */

int main(void) {
    
    
    
    struct Buffer buffer;
    /* Initialize the values in buffer */
    buffer.ok = '0';
    buffer.directive = 'R';
    buffer.account1 = '3';
    buffer.amount = 0;
    buffer.message = '0';
    buffer.beforeAmount = 0;
    buffer.afterAmount = 0;
    
    state menu = mainMenu;
    input userSelection = C;
    int accountID = 3;
    menu = mainMenu;
    
    
    char selection = 'I';  /* sent command to server */
    unsigned int msg_len;  /* length of message */                      
    
    clientConnect();
    
    /* our banking user interface */
    while(1){
        /* Reset the values in buffer */
        buffer.ok = '0';
        buffer.directive = 'R';
        buffer.account1 = '3';
        buffer.amount = 0;
        buffer.message = '0';
        buffer.beforeAmount = 0;
        buffer.afterAmount = 0;
        
        switch(menu){
            case mainMenu:
                printf( BLUE"Please select an action:\nCheck the balance of an account: C\nDeposit an amount into an account: D\nWithdraw an amount from an account: W\nTransfer an amount from one account to another: T\nDisconnect from the server: Q\n:" RESET);
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
                        break;
                    default:
                        printf(BLUE "Invalid selection, please try again\n" RESET);
                        menu = mainMenu;
                        break;
                }
                continue;
                break;
                    case checkBalance:{
                        char acct;
                        //Ask for an account name to be specified, and this should prepare the sent packet to send a check balance request for the specified account name, and should do not spelling or other checks
                        printf(BLUE "\nPlease select:\n\nChecking account: 0\nSavings account: 1\n:" RESET);
                        scanf("%s", &acct);
                        buffer.directive = 'C';
                        buffer.account1 = (char)acct;
                        buffer.ok = '1';
                        menu = mainMenu;
                        break;
                    }
                    case deposit:{
                        //Ask for an account name to be specified, and do no checks on the name of the account as above; also ask for a number to be added to the specified account
                        char acct;
                        int amt;
                        printf(BLUE "\nPlease select:\n\nChecking account: 0\nSavings account: 1\n:" RESET);
                        scanf("%s", &acct);
                        printf(BLUE "\nPlease enter amount to be deposited\n:" RESET);
                        scanf("%d", &amt);
                        buffer.ok = '1';
                        buffer.directive = 'D';
                        buffer.account1 = acct;
                        buffer.amount = htonl(amt);
                        menu = mainMenu;
                        break;
                    }
                    case withdraw:{
                        //Ask for an account name to be specified, and do no checks on the name of the account as above; also ask for a number to be withdrawn. *important* specify that the amount should be in $20 intervals, but do not check
                        char acct;
                        int amt;
                        printf(BLUE "\nPlease select:\n\nChecking account: 0\nSavings account: 1\n:" RESET);
                        scanf("%s", &acct);
                        printf(BLUE "\nPlease enter amount to be withdrawn (use only $20 incerements):\n"RESET);
                        scanf("%d", &amt);
                        buffer.ok = '1';
                        buffer.directive = 'W';
                        buffer.account1 = acct;
                        buffer.amount = htonl(amt);
                        menu = mainMenu;
                        break;
                    }
                    case transfer:{
                        //Ask for an original account and an account to transfer to, then ask for an amount to be tranfered (in whole dollar amounts), do not check any value
                        char acct1;
                        char acct2;
                        int amt;
                        
                        printf(BLUE "\nPlease select which account you are tranferring FROM:\n\nChecking account: 0\n Savings account: 1\n:" RESET);
                        scanf("%s", &acct1);
                        printf(BLUE "\nPlease enter amount to be tranferred\n:" RESET);
                        scanf("%d", &amt);
                        
                        buffer.directive = 'T';
                        buffer.account1 = acct1;
                        buffer.amount = htonl(amt);
                        buffer.ok = '1';
                        menu = mainMenu;
                        break;
                    }
                    case quit:{
                        char input;
                        /* send one last message so the server closes its socket */
                        buffer.directive = 'Q';
                        buffer.ok = '1';
                        SendFunc(buffer, sock_client, menu);
                        close (sock_client);
                        printf(BLUE "\nDo you wish to reconnect? (y/n)\n:" RESET);
                        scanf("%s",&input);
                        if(input == 'y' || input == 'Y'){
                            
                            clientConnect(); /* reconnect to the server */
                            
                            menu = mainMenu;
                            continue;
                        }
                        else{
                            exit(0);
                        }
                        break;
                    }
                    default:
                        printf(BLUE "menu enum not working correctly! Fix it!" RESET);
                        exit(0);
                        break;
        }
        SendFunc(buffer, sock_client, userSelection);
        
        RecvFunc(buffer, sock_client, userSelection);
        
        
    }
    
    
}
