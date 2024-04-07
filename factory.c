//---------------------------------------------------------------------
// Assignment : PA-03 UDP Single-Threaded Server
// Date       :
// Author     : Ethan Pae,  Gillian Kelly
// File Name  : factory.c
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

#define MAXSTR     200
#define IPSTRLEN    50

typedef struct sockaddr SA ;

int minimum( int a , int b)
{
    return ( a <= b ? a : b ) ; 
}

void subFactory( int factoryID , int myCapacity , int myDuration ) ;

void factLog( char *str )
{
    printf( "%s" , str );
    fflush( stdout ) ;
}

/*-------------------------------------------------------*/

// Global Variable for Future Thread to Shared
int   remainsToMake , // Must be protected by a Mutex
      actuallyMade ;  // Actually manufactured items

int   sd ;      // Server socket descriptor
struct sockaddr_in  
             srvrSkt,       /* the address of this server   */
             clntSkt;       /* remote client's socket       */

//------------------------------------------------------------
//  Handle Ctrl-C or KILL 
//------------------------------------------------------------
void goodbye(int sig) 
{
    /* Mission Accomplished */
    printf( "\n### I (%d) have been nicely asked to TERMINATE. "
           "goodbye\n\n" , getpid() );  
    close(sd);
    exit(0);
}

/*-------------------------------------------------------*/
int main( int argc , char *argv[] )
{
    char  *myName = "Ethan Pae, Gillian Kelly" ; 
    unsigned short port = 50015 ;      /* service port number  */
    int    N = 1 ;                     /* Num threads serving the client */

    printf("\nThis is the FACTORY server developed by %s\n\n" , myName ) ;

    signal(SIGINT, goodbye);
    signal(SIGTERM, goodbye);

	switch (argc) 
	{
      case 1:
        break ;     // use default port with a single factory thread
      
      case 2:
        N = atoi( argv[1] ); // get from command line
        port = 50015;            // use this port by default
        break;

      case 3:
        N    = atoi( argv[1] ) ; // get from command line
        port = atoi( argv[2] ) ; // use port from command line
        break;

      default:
        printf( "FACTORY Usage: %s [numThreads] [port]\n" , argv[0] );
        exit( 1 ) ;
    }


    // missing code goes here
    // ------------------------
    // awaits for a REQUEST_MSG  message over UDP from a 
    // client (implemented by the procurement.c program) 
    // containing the orderSize.

    sd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&srvrSkt, 0, sizeof(srvrSkt));
    srvrSkt.sin_family = AF_INET;
    srvrSkt.sin_addr.s_addr = htonl(INADDR_ANY);
    srvrSkt.sin_port = htons(port);

    bind(sd, (SA *)&srvrSkt, sizeof(srvrSkt));

    socklen_t clntLen = sizeof(clntSkt);
    msgBuf msg1;

    int forever = 1;
    while ( forever )
    {
        printf( "\nFACTORY server waiting for Order Requests\n" ) ; 

        recvfrom(sd, &msg1, sizeof(msg1), 0, (SA *)&clntSkt, &clntLen);

        if (msg1.purpose == REQUEST_MSG){
            remainsToMake = msg1.orderSize;

            msg1.purpose = ORDR_CONFIRM;
            msg1.numFac = N;
            sendto(sd, &msg1, sizeof(msg1), 0, (SA *)&clntSkt, clntLen);

            // immediately responds to that client with a ORDR_CONFIRM
            // message over UDPcontaining the number N of sub-factories
            // that will manufacture the requested order. For this project,
            // N is always 1. The server initializes the remainsToMake to be
            // the same as the orderSize.


            printf("\n\nFACTORY server received: " ) ;
            printMsg( & msg1 );  puts("");

            // missing code goes here

            // Activates a single sub-factory, implemented as a regular 
            // function() call in factory.c , providing it with its 
            // FactoryID ( 1 for this PA ), its capacity (say 50 items), 
            // and the duration in milliseconds to make that capacity 
            // (or part of) of items.

            // The factory server should capture any signals meant to INTerrupt
            // or TERMinate the server in the proper way.

            printf("\n\nFACTORY sent this Order Confirmation to the client " );
            printMsg(  & msg1 );  puts("");
            
            subFactory( 1 , 50 , 350 ) ;  // Single factory, ID=1 , capacity=50, duration=350 msg
        }
    }


    return 0 ;
}

void subFactory( int factoryID , int myCapacity , int myDuration )
{
    char    strBuff[ MAXSTR ] ;   // print buffer
    int     partsImade = 0 , myIterations = 0 ;
    msgBuf  msg;
    int toMake;

    // The sub-factory,  as long as ( remainsToMake  > 0  )
    while ( 1 )
    {
        // See if there are still any parts to manufacture
        if ( remainsToMake <= 0 )
            break ;   // Not anymore, exit the loop
        
        toMake = minimum(myCapacity, remainsToMake); // -Decides how many to make as the lesser of capacity and remainsToMake
        remainsToMake -= toMake;                     // -Decrements remainsToMake  by the number of items this sub-factory will make,
        usleep(myDuration * 1000);                   // -Sleeps for duration milliseconds, to emulate the actual 
                                                     // manufacturing of the items,
        partsImade += toMake;
        myIterations++;

        // Send a Production Message to Supervisor

        msg.purpose = PRODUCTION_MSG;    // -After waking up, the sub-factory sends a  PRODUCTION_MSG 
        msg.facID = factoryID;           // message over UDP containing its FactoryID
        msg.capacity = myCapacity;       // its capacity, 
        msg.partsMade = toMake;          // the actual number of items it has just made in this 
        msg.duration = myDuration;       // iteration, and the duration in milliseconds it took to make that many items.
    
        sendmsg(sd, &msg, sizeof(msg));

        printf("Factory #%d made %d parts in this iteration.\n", factoryID, toMake);
    }

    // Send a Completion Message to Supervisor
    msg.purpose = COMPLETION_MSG;    // Once there are no more items to make, the sub-factory sends 
    msg.facID = factoryID;           // a COMPLETION_MSG message over UDP to the client containing
                                     // its FactoryID. 

    sendmsg(sd, &msg, sizeof(msg));

    snprintf( strBuff , MAXSTR , ">>> Factory # %-3d: Terminating after making total of %-5d parts in %-4d iterations\n" 
          , factoryID, partsImade, myIterations);
    factLog( strBuff ) ;
    
}