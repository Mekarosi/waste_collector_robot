

#define IRLeft 7                    // Defining that Left Infrared sensor is connected to Digital Pin 7 and it will be called as IRLeft
#define IRRight 8                   // Defining that Right Infrared sensor is connected to Digital Pin 8 and it will be called as IRRight
#define IRBin 11                    // Defining that Bin Infrared sensor is connected to Digital Pin 11 and it will be called as IRBin
#define M1_a 3                      // Defining pin no. and name of the pin to which 1st terminal of Left Motor is connected to Intel Galileo.
#define M1_b 5                      // Defining pin no. and name of the pin to which 2nd terminal of Left Motor is connected to Intel Galileo.
#define M2_a 6                      // Defining pin no. and name of the pin to which 1st terminal of Right Motor is connected to Intel Galileo.
#define M2_b 9                      // Defining pin no. and name of the pin to which 2nd terminal of Right Motor is connected to Intel Galileo.
#include<Servo.h>                   // Including Servo.h library used to interface Servo Motor with Intel Galileo
#include <SPI.h>                    // add 'Serial Peripheral Interface' Library
#include <Ethernet.h>               // add the Ethernet Library
 


                                
                                    // all variables declared here can be accessed within any function. Also known as Global Variable
byte mac[] =                        // Enter the MAC address of your device
{  
  0x98, 0x4F, 0xEE, 0x00, 0xCE, 0x50 
};           





/*  Initialize the Ethernet server library
    with the IP address and port you want to use 
    (port 80 is default for HTTP): */

    
EthernetServer server(80);          // Create a server on port 80
String web_response;                // Declare a string variable to know the response from web server
EthernetClient client;              // client is an object of class EthernetClient to Assign incoming client to 'client'
String get_String;                  // Stores the numerical value in the web_response in string
int get_Value=0;                    // Stores the numerical value in the get_String in 'int' format
IPAddress ip(192, 168, 1, 20);      // Assign custom IP address to 'ip'
String Bin_Status="Available";      // Declaring a String type variable Bin_Status in which the status of the bin will be stored
boolean left_ir;                    // Declaring a boolean type variable in which state of left IR Sensor will be stored. If there is something in front of it, then it returns true
boolean right_ir;                   // Declaring a boolean type variable in which state of Right IR Sensor will be stored. If there is something in front of it, then it returns true
boolean IR_Status;                  // Declaring a boolean type variable in which state of Bin IR Sensor will be stored. If there is something in front of it, then it returns true
Servo Servo1;                       // Creating an object of Servo
int count;                          // Initializing Global Variable Count in which the count will be stored that how many slots it collector has reached


void setup()
{
  pinMode (M1_a, OUTPUT);           // Defining Left Motor as OUTPUT
  pinMode (M1_b, OUTPUT);           // Defining Left Motor as OUTPUT
  pinMode (M2_a, OUTPUT);           // Defining Right Motor as OUTPUT
  pinMode (M2_b, OUTPUT);           // Defining Right Motor as OUTPUT
  pinMode(IRLeft,INPUT);            // Defining that the Mode of Left IR Sensor is INPUT as we are taking some input from it
  pinMode(IRRight,INPUT);           // Defining that the Mode of Left IR Sensor is INPUT as we are taking some input from it
  pinMode(IRBin,INPUT);             // Defining that the Mode of Left IR Sensor is INPUT as we are taking some input from it
  
  Servo1.attach(10);
  Servo1.write(10);
  Serial.begin(9600);               // begin serial communication at 9600 baud rate
   
  Ethernet.begin(mac,ip);           // start the Ethernet connection
  server.begin();                   // Start the Ethernet Server  
}



void loop()
{
    client = server.available();        // try to get client              
    
    if (client)                         // Checking if the Client is connected or not. If connected, then client will return true value 
    {
      boolean currentLineIsBlank = true;  // Set 'currentLineIsBlank' as true;
      while (client.connected())        // while loop will be executed until the client is connected  
      {
        if(client.available())          // Checking if some data is available from the client
        {                               // client data available to read
            char c = client.read();     // read 1 byte (character) from client
            web_response += c;          // save the HTTP request 1 char at a time
                                        
            if (c == '\n' && currentLineIsBlank)            // last line of client request is blank and ends with \n                       
            {                                               // respond to client only after last line received
              host_server();                                //host the server;
              Serial.println(web_response);                 //display web_response
              get_String=web_response.substring(6,7);       //reads the string between 6th and 7th element
              get_Value=get_String.toInt();                 //converts the string into integer
              Serial.println(get_Value);                    //display value 
              perform_Task(get_Value);                      //performs the required task
              get_String="";                                //empty the string
              web_response = "";                            // finished with request, empty string
              break;                                        //breaks from the loop
            }
            
            if (c == '\n')                                  // every line of text received from the client ends with \r\n 
            {                                               // last character on line of received text
              currentLineIsBlank = true;                    // starting new line with next character read
            } 
            else if (c != '\r')                             // a text character was received from client 
            {
                    currentLineIsBlank = false;
            }
        }                                                   // end if (client.available())
      }                                                     // end while (client.connected())
      delay(1);                                             // give the web browser time to receive the data
      client.stop();                                        // close the connection
    }                                                       // end if (client)
}


void host_server()                                                                                          //defines host_server function
{
    client.println("HTTP/1.1 200 OK\r\n"                                                                    // send a standard http response header
    "Content-Type: text/html\r\n"                                                                           // Defines the content type as text/html
    "Connection: close\r\n"                                                                                 // Closes the connection after the response is sent
    "\r\n");                                                                                                // Prints a blank line in the HTML file
    client.println("<!DOCTYPE html>");                                                                      // Defines document as HTML type
    client.println("<html>");                                                                               // starting html tag
    
    client.println("<head>");                                                                               // HTML file's head definition
    client.println("<title>Smart Cleaning Solutions</title>");                                              // Title is defined
    client.println("<style>h1{text-align:center;}</style></head>");                                         // Style of h1 is defined
         
    client.print("<br><font size=\"6\"><font color=\"Red\">=======Smart Cleaning Solutions=======");        // Writing the Heading
    client.print("<br><font color=\"Blue\">&nbsp;Garbage Bin Status:&nbsp;&nbsp;");
    client.println(Bin_Status);                                                                             // It wil print the status of the Bin
    client.print("<br><br><font color=\"Red\">=======Call Garbage Cleaning Rover=======");                  // Defining 4 Buttons so that house owners can click on their corresponding slot
    client.println("<br>&nbsp;<font color=\"Green\"><a href=\"/H1\">House Slot 1</a><br>");
    client.println("<br>&nbsp;<font color=\"Green\"><a href=\"/H2\">House Slot 2</a><br>");
    client.println("<br>&nbsp;<font color=\"Green\"><a href=\"/H3\">House Slot 3</a><br>");
    client.println("<br>&nbsp;<font color=\"Green\"><a href=\"/H4\">House Slot 4</a><br>");
    client.println("</html>");
}


void perform_Task(int slot_no)                                // defines perform_Task function
{
  if(slot_no==1 || slot_no==2 || slot_no==3 || slot_no==4)    // If the house slot is 1 or 2 or 3 or 4, || means or
  {
  reach_home(slot_no);                                        // Call the reach_home function in which house slot is slot_no. Garbage Collector will reach the particular house slot
  Servo1.write(90);                                           // Opening the Bin so that person can pun in their Garbage
  delay(10000);                                               // Waiting for the Person to put the Garbage
  Servo1.write(0);                                            // Closing the Bin
  u_turn();                                                   // Calling the function u_turn() in which the garbage collector will take u turn based on IR Sensors
  count=0;                                                    // Resetting the Value of Count
  reach_home(slot_no);                                        // Call the reach_home function so that the garbage collector can return to its Base station
  count=0;                                                    // Again resetting the value of count
  u_turn_right();                                             // Again taking u turn by turning right.
  IR_Status=digitalRead(IRBin);                               // Checking the status of the Bin. If IR_Status is true, that means bin is full
      if(!IR_Status)                                          // If IR_Status is false i.e. there is space available for more garbage
      {
          Bin_Status="Available";                             // Update the Bin_Status as Available
      }
      else if(IR_Status)                                      // If IR_Status is tue i.e. there is no space available for more garbage
      {                                                       // Bin Status updated that no more garbage can be taken
          Bin_Status="Full!! Not Available for Service!! Sorry for the Inconvinience Caused";
      }
  }                                                           // end of if condition
  else                                                        // If house slot is something else than 1, 2,3 or 4, then Garbage Cleaner will not visit any house and check the status of bin
  {
    Servo1.write(0);                                          // Closing the Bin
    IR_Status=digitalRead(IRBin);                             // Reading the Status of the Bin. If IR_Status is true, that means bin is full
      if(!IR_Status)                                          // If IR_Status is false i.e. there is space available for more garbage
      {
          Bin_Status="Available";                             // Update the Bin_Status as Available
      }
      else if(IR_Status)                                      // If IR_Status is tue i.e. there is no space available for more garbage
      {                                                       // Bin Status updated that no more garbage can be taken
          Bin_Status="Full!! Not Available for Service!! Sorry for the Inconvinience Caused";
      }
    }                                                         // End of else condition
}

                                                            // End of Function
void reach_home(int h_no)                       // Declaring the function reach_home() which will have an input Parameter that is the slot number we want the collector to reach
{
  while(count<h_no)                             // The loop will be executed unit count is less than the desired slot number where collector has to reach
      {
      left_ir=digitalRead(IRLeft);              // Reading value from Left IR Sensor
      right_ir=digitalRead(IRRight);            // Reading value from Right IR Sensor
        
                                                /*      Conditions for Line Follower
                                                 *       Left IR Sensor   |     Right IR Sensor   |     Meaning                                         |         Movement
                                                              True                  True                Both IR are at white                                      Go Straight
                                                              True                  False               Left IR is at white and Right is at Black                 Turn Right
                                                              False                 True                Left IR is at Black and Right is at White                 Turn Left
                                                              False                 False               Both Ir are at Black                                        Stop
                                                 */
        
        if(!left_ir && !right_ir)               // If both IR are returning False value i.e. both are at black. 
            {
            stop1();                            // It both the sensors are at Black, that means Collector has reach one slot. Thus it has to stop
            count=count+1;                      // The value of count is increased by one so that it can be compared and checked that whether count is equal to desired slot or not
            forward();                          // Move little forward so that both the IR Sensors can be back on white 
            delay(500);
            }
        if(left_ir && right_ir)                 // If both the sensors are at white, that means Collector is going in the right path and has to continue straight
            {
            forward();                          // Calling the function forward() so that rover can move forward
            }
        if(!left_ir && right_ir)                // If Left IR sensor is at Black and Right IR Sensor is at White, that means collector has gone on right side and need to turn left
            {
            left();                             // Calling the function left() so that rover can move left
            }
        if(left_ir && !right_ir)                // If Left IR sensor is at White and Right IR Sensor is at Black, that means collector has gone on left side and need to turn right
            {
            right();                            // Calling the function right() so that rover can move right
            } 
      }
stop1();
}



void u_turn()                                   // Defining the function u_turn() which when called will turn the collector towards the left side.
{
  left();                                       // Collector will turn left for one second
  delay(1000);
  left_ir=digitalRead(IRLeft);                  // Reading value of Left IR sensor
    while(left_ir)                              // The Collector will keep on turning left until it is on white. As soon as it senses Black i.e. left sensor has reached the black line
      {
          left_ir=digitalRead(IRLeft);
          left();
      }
  stop1();                                      // Collector has to stop as soon as it reaches Black
  left();                                       // Collector with turn left for 100 milli seconds so that it can cross black line and both sensors can now be on the black line
  delay(100);
  stop1();                                      // The collector will Stop once both sensors and on both sides of black line
}



void u_turn_right()                             // Defining the function u_turn_right() which when called will turn the collector towards the right side.
{
  right();                                      // Collector will turn right for one second
  delay(1000);
  right_ir=digitalRead(IRRight);                // Reading value of Right IR sensor
  while(right_ir)                               // The Collector will keep on turning right until it is on white. As soon as it senses Black i.e. right sensor has reached the black line
      {
          right_ir=digitalRead(IRRight);
          right();
      }
  stop1();                                      // Collector has to stop as soon as it reaches Black
  right();                                      // Collector with turn left for 100 milli seconds so that it can cross black line and both sensors can now be on the black line
  delay(100);
  stop1();                                      // The collector will Stop once both sensors and on both sides of black line
}



void forward()                        // Defining function called forward(). Whenever this function will be called, the driving base will move forward. 
{
  digitalWrite(M1_a,LOW);             // Driving Base will move forward only if both the motors are rotating in forward direction.
  digitalWrite(M1_b,HIGH);            // On testing it came out that if LOW value is provided to M1_a and HIGH Value is provided to M1_b, then Left Motor will rotate in forward direction.
  digitalWrite(M2_a,LOW);             // On testing it came out that if LOW value is provided to M2_a and HIGH Value is provided to M2_b, then Left Motor will rotate in forward direction.
  digitalWrite(M2_b,HIGH);            // So Now both are Rotating in forward Direction, thus driving base will move in forward direction.
}



void backward()                       // Defining function called backward(). Whenever this function will be called, the driving base will move backward.
{
  digitalWrite(M1_a,HIGH);            // Driving Base will move backward only if both the motors are rotating in backward direction.
  digitalWrite(M1_b,LOW);             // On testing it came out that if HIGH value is provided to M1_a and LOW Value is provided to M1_b, then Left Motor will rotate in backward direction.
  digitalWrite(M2_a,HIGH);            // On testing it came out that if HIGH value is provided to M2_a and LOW Value is provided to M2_b, then Left Motor will rotate in backward direction.
  digitalWrite(M2_b,LOW);             // So Now both are Rotating in backward Direction, thus driving base will move in backward direction.
}



void left()                           // Defining function called left(). Whenever this function will be called, the driving base will move left.
{
  digitalWrite(M1_a,HIGH);            // Driving Base will move left if left motor is rotating in backward direction and right motor is rotating in forward direction.
  digitalWrite(M1_b,LOW);             // On testing it came out that if HIGH value is provided to M1_a and LOW Value is provided to M1_b, then Left Motor will rotate in backward direction.
  digitalWrite(M2_a,LOW);             // On testing it came out that if LOW value is provided to M2_a and HIGH Value is provided to M2_b, then Left Motor will rotate in forward direction.
  digitalWrite(M2_b,HIGH);            // So Now left motor is rotating in backward direction and right motor is rotating in forward direction, thus driving base will turn left.
}
		
void right()                          // Defining function called right(). Whenever this function will be called, the driving base will move left.
{
  digitalWrite(M1_a,LOW);             // Driving Base will move right if right motor is rotating in backward direction and left motor is rotating in forward direction.
  digitalWrite(M1_b,HIGH);            // On testing it came out that if LOW value is provided to M1_a and HIGH Value is provided to M1_b, then Left Motor will rotate in forward direction.
  digitalWrite(M2_a,HIGH);            // On testing it came out that if HIGH value is provided to M2_a and LOW Value is provided to M2_b, then Left Motor will rotate in backward direction.
  digitalWrite(M2_b,LOW);             // So Now right motor is rotating in backward direction and left motor is rotating in forward direction, thus driving base will turn right.
}


void stop1()                          // Defining function called stop1(). Whenever this function will be called, the driving base will stop moving.
{
digitalWrite(M1_a,LOW);               // Driving base will stop only if both its motors are not rotating
digitalWrite(M1_b,LOW);               // Motor will be OFF if there is no potential difference. Thus providing LOW value to both M1_a and M1_b to turn it OFF
digitalWrite(M2_a,LOW);               // Motor will be OFF if there is no potential difference. Thus providing LOW value to both M2_a and M2_b to turn it OFF
digitalWrite(M2_b,LOW);               // So Now both right motor and left motor are not rotating, thus driving base will not move.
}

