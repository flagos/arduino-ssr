#include "TimerOne.h"
 
int count = 0;
int current_idx = 0;
int lengths[3] = {20, 15, 15};   
 
void setup() 
{
  // Initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards
  pinMode(13, OUTPUT);    
  pinMode(A0, OUTPUT);
  
  Timer1.initialize(200000); // set a timer of length 20000 microseconds (or 0.02 sec - or 50Hz => frequency of electricity in europe)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
}
 
void loop()
{
  // Main code loop
  // TODO: Put your regular (non-ISR) logic here
}
 
/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timerIsr()
{
  
  while (count == 0) {
    count = lengths[current_idx++%(sizeof(lengths)/sizeof(lengths[0]))];
  }
   if( current_idx == 0) {
      digitalWrite( A0, 0 );      
      digitalWrite( 13, 1 );
   }
   if( current_idx == 1) {
      digitalWrite( A0, 0 );      
      digitalWrite( 13, 1 );
   }
   if( current_idx == 2) {
      digitalWrite( A0, 0 );      
      digitalWrite( 13, 1 );
   }
   count--;
   current_idx %= 3;
}
