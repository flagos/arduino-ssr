#include "TimerOne.h"
#include "SPI.h" // new include
#include "avr/pgmspace.h" // new include
#include "Ethernet.h"
#include "WebServer.h"
#include <string.h>
#include <stdlib.h>

int count = 0;
int current_idx = -1;
int lengths[4] = {0, 0, 0, 50}; // Default state is IDLE   
 
static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
 
#define VERSION_STRING "0.1"

P(Page_start) = "<html><head><title>Web_Parms_1 Version " VERSION_STRING "</title></head><body>\n";
P(Page_end) = "</body></html>";
P(Get_head) = "<h1>GET from ";
P(Post_head) = "<h1>POST to ";
P(Unknown_head) = "<h1>UNKNOWN request for ";
P(Default_head) = "unidentified URL requested.</h1><br>\n";
P(Raw_head) = "raw.html requested.</h1><br>\n";
P(Parsed_head) = "parsed.html requested.</h1><br>\n";
P(Good_tail_begin) = "URL tail = '";
P(Bad_tail_begin) = "INCOMPLETE URL tail = '";
P(Tail_end) = "'<br>\n";
P(Parsed_tail_begin) = "URL parameters:<br>\n";
P(Parsed_item_separator) = " = '";
P(Params_end) = "End of parameters<br>\n";
P(Post_params_begin) = "Parameters sent by POST:<br>\n";
P(Line_break) = "<br>\n"; 

/* This creates an instance of the webserver.  By specifying a prefix
 * of "", all pages will be at the root of the server. */
#define PREFIX ""
WebServer webserver(PREFIX, 80);

#define NAMELEN 32
#define VALUELEN 32

void parsedCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  URLPARAM_RESULT rc;
  char name[NAMELEN];
  char value[VALUELEN];

  int SSR0 = 0;
  int SSR1 = 0;
  int SSR2 = 0;
  int IDLE = 0;
   

  /* this line sends the standard "we're all OK" headers back to the
     browser */
  server.httpSuccess();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type == WebServer::HEAD)
    return;

  server.printP(Page_start);
  switch (type)
    {
    case WebServer::GET:
        server.printP(Get_head);
        break;
    case WebServer::POST:
        server.printP(Post_head);
        break;
    default:
        server.printP(Unknown_head);
    }

    server.printP(Parsed_head);
    server.printP(tail_complete ? Good_tail_begin : Bad_tail_begin);
    server.print(url_tail);
    server.printP(Tail_end);

  if (strlen(url_tail))
    {
    server.printP(Parsed_tail_begin);
    while (strlen(url_tail))
      {
      rc = server.nextURLparam(&url_tail, name, NAMELEN, value, VALUELEN);
      if (rc == URLPARAM_EOS)
        server.printP(Params_end);
       else
        {
        server.print(name);
        server.printP(Parsed_item_separator);
        server.print(value);
        server.printP(Tail_end);
	
	if(strcmp(name, "SSR0") == 0) {
	  SSR0 = atoi(value);
	}
	if(strcmp(name, "SSR1") == 0) {
	  SSR1 = atoi(value);
	}
	if(strcmp(name, "SSR2") == 0) {
	  SSR2 = atoi(value);
	}
	if(strcmp(name, "IDLE") == 0) {
	  IDLE = atoi(value);
	}
        }
      }
    }
  if (type == WebServer::POST)
  {
    server.printP(Post_params_begin);
    while (server.readPOSTparam(name, NAMELEN, value, VALUELEN))
    {
      server.print(name);
      server.printP(Parsed_item_separator);
      server.print(value);
      server.printP(Tail_end);

 	if(strcmp(name, "SSR0") == 0) {
	  SSR0 = atoi(value);
	}
	if(strcmp(name, "SSR1") == 0) {
	  SSR1 = atoi(value);
	}
	if(strcmp(name, "SSR2") == 0) {
	  SSR2 = atoi(value);
	}
	if(strcmp(name, "IDLE") == 0) {
	  IDLE = atoi(value);
	}     
    }
  }
  
  lengths[0]  = SSR0;
  lengths[1]  = SSR1;
  lengths[2]  = SSR2;
  lengths[3]  = IDLE;
 
  // Switch to state IDLE
  current_idx = 3;
  count       = IDLE;

  server.printP(Page_end);

} 
 
void setup() 
{
  // Initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);    
  

  digitalWrite( A0, 0 );      
  digitalWrite( A1, 0 );      
  digitalWrite( A2, 0 );
  
  Timer1.initialize(200000); // set a timer of length 20000 microseconds (or 0.02 sec - or 50Hz => frequency of electricity in europe)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
  
  /* initialize the Ethernet adapter */
  Ethernet.begin(mac);

  /* setup our default command that will be run when the user accesses
   * the root page on the server */
  webserver.setDefaultCommand(&parsedCmd);
  //webserver.setFailureCommand(&my_failCmd);
  
  webserver.addCommand("index.html", &parsedCmd);
  
  /* start the webserver */
  webserver.begin();
  
}
 
void loop()
{
  char buff[64];
  int len = 64;

  /* process incoming connections one at a time forever */
  webserver.processConnection(buff, &len);
}
 
/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timerIsr()
{
  
  while (count == 0) {
    current_idx++;
    current_idx %= (sizeof(lengths)/sizeof(lengths[0]));
    count = lengths[current_idx];
  }
   if( current_idx == 0) {
      digitalWrite( A1, 0 );      
      digitalWrite( A2, 0 );      
      digitalWrite( A0, 1 );
   }
   if( current_idx == 1) {
      digitalWrite( A0, 0 );
      digitalWrite( A2, 0 );      
      digitalWrite( A1, 1 );      
   }
   if( current_idx == 2) {
      digitalWrite( A0, 0 );      
      digitalWrite( A1, 0 );
      digitalWrite( A2, 1 );           
   }
   if( current_idx == 3) {
      digitalWrite( A0, 0 );      
      digitalWrite( A1, 0 );
      digitalWrite( A2, 0 );           
   }
   count--;
   current_idx %= (sizeof(lengths)/sizeof(lengths[0]));
}
