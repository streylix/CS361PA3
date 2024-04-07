//---------------------------------------------------------------------
// Assignment : PA-03 UDP Single-Threaded Server
// Date       :
// Author     : Ethan Pae,  Gillian Kelly
// File Name  : procurement.c
//---------------------------------------------------------------------

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/stat.h>
#include <sys/wait.h>

#include "wrappers.h"
#include "message.h"

#define MAXFACTORIES    20

typedef struct sockaddr SA ;

/*-------------------------------------------------------*/
int main( int argc , char *argv[] )
{
    int     numFactories ,      // Total Number of Factory Threads
            activeFactories ,   // How many are still alive and manufacturing parts
            iters[ MAXFACTORIES+1 ] = {0} ,  // num Iterations completed by each Factory
            partsMade[ MAXFACTORIES+1 ] = {0} , totalItems = 0;

    char  *myName = "Ethan Pae, Gillian Kelly" ; 
    printf("\nPROCUREMENT: Started. Developed by %s\n\n" , myName );    
    fflush( stdout ) ;
    
    if ( argc < 4 )
    {
        printf("PROCUREMENT Usage: %s  <order_size> <FactoryServerIP>  <port>\n" , argv[0] );
        exit( -1 ) ;  
    }

    unsigned        orderSize  = atoi( argv[1] ) ;
    char	       *serverIP   = argv[2] ;
    unsigned short  port       = (unsigned short) atoi( argv[3] ) ;
 

    /* Set up local and remote sockets */
    int sockfd;

    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));        // Gets the order size , the server's IP address
    servaddr.sin_family = AF_INET;                 // (in dotted-decimal format) , and the server's port number
    servaddr.sin_port = htons(port);               // via the command-lie arguments.
    inet_pton(AF_INET, serverIP, &servaddr.sin_addr);

    // Send the initial request to the Factory Server
    msgBuf  msg1;

    msg1.purpose = REQUEST_MSG; // Sends a REQUEST_MSG message over UDP to the 
    msg1.orderSize = orderSize; // factory server containing the order size.

    sendto(sockfd, &msg1, sizeof(msg1), 0, (SA *)&servaddr, sizeof(servaddr));

    printf("\nPROCUREMENT Sent this message to the FACTORY server: "  );
    printMsg( & msg1 );  puts("");


    /* Now, wait for oreder confirmation from the Factory server */
    msgBuf  msg2;
    printf ("\nPROCUREMENT is now waiting for order confirmation ...\n" );

    // Awaits the ORDR_CONFIRM message over UDP
    socklen_t len = sizeof(servaddr);
    recvfrom(sockfd, &msg2, sizeof(msg2), 0, (SA *)&servaddr, &len);

    if (msg2.purpose == ORDR_CONFIRM){
        numFactories = 1;
        activeFactories = 1; // it will only be one factory for this PA
    }

    printf("PROCUREMENT received this from the FACTORY server: "  );
    printMsg( & msg2 );  puts("\n");

    // learns how many sub-factories will be actively serving it (which will be 1)
    // Monitor all Active Factory Lines & Collect Production Reports
    while (activeFactories > 0){    // wait for messages from sub-factories
        recvfrom(sockfd, &msg2, sizeof(msg2), 0, (SA *)&servaddr, &len); 

        // awaits for a message over UDP
        // Inspect the incoming message
        if (msg2.purpose == PRODUCTION_MSG){
            printMsg(&msg2);                        // If it is a PRODUCTION_MSG message, print it and 
            iters[msg2.facID]++;                    // update the totals ( iterations and items made) 
            partsMade[msg2.facID] += msg2.partsMade;// for the sub-factory that sent the production message.
            totalItems += msg2.partsMade;
        } else if (msg2.purpose == COMPLETION_MSG) {// If it is a  COMPLETION_MSG message, decrement the 
            activeFactories--;                      // number of active sub-factories
        }
    } 

    // Print the summary report
    totalItems  = 0 ;
    printf("\n\n****** PROCUREMENT Summary Report ******\n");

    for (int i = 1; i <= numFactories; i++){
        printf("Factory %d: Iterations: %d, Parts Made: %d\n", i, iters[i], partsMade[i]);
    }
    printf("Total Items Made: %d\n", totalItems);

        // When there are no more active sub-factories, the client
        // prints a summary report showing the total items made, 
        // and iterations performed, by each sub-factory.

    printf("==============================\n") ;

    if (totalItems == orderSize){
        printf("Order Fulfilled.\n");
    } else {
        printf("Order Not Fulfilled.\n");
    }

        // The clients also compares the total items made by all 
        // sub-factories against the order size it 
        // originally placed.


    printf( "\n>>> Supervisor Terminated\n");

    close(sockfd);

        // Close and terminate any remaining processes

    return 0 ;
}
