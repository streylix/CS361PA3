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

    char  *myName = "Replace with your Names" ; 
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


    // missing code goes here

        // Gets the order size , the server's IP address 
        // (in dotted-decimal format) , and the server's port number
        // via the command-lie arguments.


    // Prepare the server's socket address structure
    
    // missing code goes here


    // Send the initial request to the Factory Server
    msgBuf  msg1;

    // missing code goes here

        // Sends a REQUEST_MSG message over UDP to the 
        // factory server containing the order size.


    printf("\nPROCUREMENT Sent this message to the FACTORY server: "  );
    printMsg( & msg1 );  puts("");


    /* Now, wait for oreder confirmation from the Factory server */
    msgBuf  msg2;
    printf ("\nPROCUREMENT is now waiting for order confirmation ...\n" );


    // missing code goes here

        // Awaits the ORDR_CONFIRM message over UDP


    printf("PROCUREMENT received this from the FACTORY server: "  );
    printMsg( & msg2 );  puts("\n");


    // missing code goes here

        // learns how many sub-factories will be actively serving
        // it (in this PA, it's always 1 sub-factory)

    // Monitor all Active Factory Lines & Collect Production Reports
    while ( activeFactories > 0 ) // wait for messages from sub-factories
    {


        // missing code goes here
            // awaits for a message over UDP        

       // Inspect the incoming message


       // missing code goes here
            // If it is a PRODUCTION_MSG message, print it and 
            // update the totals ( iterations and items made) 
            // for the sub-factory that sent the production message.

            // If it is a  COMPLETION_MSG message, decrement the 
            // number of active sub-factories

    } 

    // Print the summary report
    totalItems  = 0 ;
    printf("\n\n****** PROCUREMENT Summary Report ******\n");


    // missing code goes here
        // When there are no more active sub-factories, the client
        // prints a summary report showing the total items made, 
        // and iterations performed, by each sub-factory.


    printf("==============================\n") ;


    // missing code goes here
        // The clients also compares the total items made by all 
        // sub-factories against the order size it 
        // originally placed.


    printf( "\n>>> Supervisor Terminated\n");

    // missing code goes here
        // Close and terminate any remaining processes

    return 0 ;
}
