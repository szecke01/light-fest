#include <LPD8806.h>

#include <SPI.h>

#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <util.h>

#define serverCycle 50U
#define lightsCycle 5U


// Server connection fields
String command_buffer;
String command;
EthernetClient client;

// Arduino Cycle counters
unsigned long serverLastMillis = 0;
unsigned long lightsLastMillis = 0;
boolean serverState = false;
boolean lightsState = false;

// LED iterator and current pass
// current pass is which iteration of loop it is doing
int     led_iterator;
boolean led_current_pass;


/*
  Web Server Demo
  thrown together by Randy Sarafan
 
 Allows you to turn on and off an LED by entering different urls.
 
 To turn it on:
 http://your-IP-address/$1
 
 To turn it off:
 http://your-IP-address/$2
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Connect an LED to pin D2 and put it in series with a 220 ohm resistor to ground
 
 Based almost entirely upon Web Server by Tom Igoe and David Mellis
 
 Edit history: 
 created 18 Dec 2009
 by David A. Mellis
 modified 4 Sep 2010
 by Tom Igoe
 
 */
// Number of RGB LEDs in strand:
int nLEDs = 128;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 2;
int clockPin = 3;
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
IPAddress ip(192,168,1,14); //<<< ENTER YOUR IP ADDRESS HERE!!!

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup()
{
   strip.begin();
   command_buffer = "";
   command        = command_buffer;
   led_iterator   = 0;

  // Update the strip, to start they are all 'off'
  for(int i=0; i < strip.numPixels();i++)
  {
      strip.setPixelColor(i,strip.Color(0,0,0));
  }
  strip.show();

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.begin(9600);
  
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  // print your local IP address:
  Serial.println(Ethernet.localIP());
  client = server.available();
}

void loop()
{       
    
    // Command is equal to either "" or completed buffer
    if(cycleCheck(&serverLastMillis, serverCycle))
    {
      client = server.available();
      command = get_stream_value();
    }
    if(cycleCheck(&lightsLastMillis, lightsCycle))
    {
      if(command == "BLUE")
      {
        for(int i = 0; i < strip.numPixels(); i++)
         {
           strip.setPixelColor(i,strip.Color(0,0,127));
         } 
         strip.show();
      }
      else if(command == "EVIL")
      {
        evil_reigns(); 
      }
      else if(command == "TYLAR")
      {
        tyler_wants();
      }
      else if(command == "RED")
      {
         int music = analogRead(0);
         for(int i = 0; i < strip.numPixels(); i++)
         {
           float musicScaling = float(music) / 1.0f;
           strip.setPixelColor(i,strip.Color((int)(127.0f*musicScaling),0,0));
         } 
         strip.show();
      }
      else if(command == "STROBE")
      {
        for(int i = 0; i < strip.numPixels(); i++)
         {
           strip.setPixelColor(i,strip.Color(127,127,127));
         } 
         strip.show();
         delay(30);
        for(int i = 0; i < strip.numPixels(); i++)
         {
           strip.setPixelColor(i,strip.Color(0,0,0));
         }
        strip.show(); 
      }
      else if(command == "OFF")
      {
        for(int i = 0; i < strip.numPixels(); i++)
         {
           strip.setPixelColor(i,strip.Color(127,0,127));
         } 
         strip.show();
      }
      else if(command == "PG")
      {
        for(int i = 0; i < strip.numPixels(); i++)
         {
           strip.setPixelColor(i,strip.Color(0,0,0));
         } 
         strip.show();
      }
      else if(command == "RANDO")
      {
        twoFloorRandom();
      }
      else command = "RED";
      
      
      
    }
}



void evil_reigns()
{
    strip.setPixelColor(led_iterator,strip.Color(127,60,0));
    strip.show();  
    strobe(led_iterator,10,20);
    
    if(led_iterator > strip.numPixels())
      led_iterator = 0;
      
    led_iterator++;
}


void strobe(int i, int n , int n2)
{
  for(int k = 0 ; k <(n2/n); k++)
  {
    for(int j = 0 ; j < strip.numPixels() ; j++)
    {
      strip.setPixelColor(j,strip.Color(0,0,0));
    }
    strip.show();
    delay(n);
    
    for(int j = 0 ; j < i ; j++)
    {
      strip.setPixelColor(j,strip.Color(127,60,0));
    }
    strip.show();
  }
}    

void tyler_wants()
{
  
  if(led_current_pass == 0)
    {
        strip.setPixelColor(led_iterator,strip.Color(0,0,0));
     
        if(led_iterator > strip.numPixels())
          {
            strip.show();
            led_iterator = 0;
            led_current_pass = 1;
          }

        led_iterator++;
        
        return;
    }
    
  if(led_current_pass == 1)
  {
    for(int k = 0 ; k < 4; k++)
    {
       strip.setPixelColor(led_iterator + k,strip.Color(100,0,127));
    }
    strip.setPixelColor(led_iterator - 1,strip.Color(127,127,0));
    
    strip.show();
    
    if(led_iterator == strip.numPixels() - 3)
    {
      strip.show();
      led_iterator     = strip.numPixels();
      led_current_pass++;
    }
    
    led_iterator++;
    
    return;
  }
  if(led_current_pass == 2)
  {
    strip.setPixelColor(led_iterator, strip.Color(0,127,0));
    strip.show();
    
    if(led_iterator == 0)
    {
      led_current_pass = 0;
      return;
    }
    
    led_iterator--;
  }

 
}

// Ethernet character stream get character
String get_stream_value()
{ 
  boolean incoming = 0;
// listen for incoming clients
  if (client) {
    while(client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (incoming)
        {
          if(c == ' ')
          {
            String temp_buffer = command_buffer;
            command_buffer     = "";
            incoming           = 0;
            led_iterator       = 0;
            led_current_pass   = 0;
            return temp_buffer;
          }
          command_buffer += c; 
          Serial.println(command_buffer);
        }
        
        if(c == '$'){ 
          incoming = 1;
        }
      }
     else{
       client.stop();
       client = server.available();
     }
   }
  }
  return command;
}
// Determine how many cycles each process gets
boolean cycleCheck(unsigned long *lastMillis, unsigned int cycle) 
{
  unsigned long currentMillis = millis();
  if(currentMillis - *lastMillis >= cycle)
  {
    *lastMillis = currentMillis;
    return true;
  }
  else
    return false;
}



void twoFloorRandom()

{

  //states

  int state;

  int fp_state = 30;

  int total_LED = 128;

  

  //random seed

  int which;

  int which2;

  

  

  uint32_t color1 = strip.Color(127,0,0);

  uint32_t color2 = strip.Color(0,127,0);

  uint32_t color3 = strip.Color(0,0,127);

  uint32_t color4 = strip.Color(127,127,127);

  

  if(led_iterator == 0){

    //turn all color 1

    state = 0;

  }

  

  else if(led_iterator < fp_state){

    //go to color 2

    state = 1;

  }

  else if(led_iterator < 2*fp_state){

    //go to color 3

    state = 2;

  }

  else if(led_iterator < 3*fp_state){

    //go to color 4

    state = 3;

  }

  else if(led_iterator < 5*fp_state){

    //back to color 1

    state = 4;

  }

  

  

  if(state == 0)

  {

    //set all to color 1

    for(int i = 0; i < strip.numPixels() ; i++)

    {

      strip.setPixelColor(i,color1);

    }

    strip.show();

  }

  

  if(state == 1)

  {

    //set 3 random to color 2

    for(int i = 0 ; i < 3; i++)

    {

      which = random(0,strip.numPixels()/2);

      which2 = which + strip.numPixels()/2;

      

      strip.setPixelColor(which,color2);

      strip.setPixelColor(which2,color2);

      strip.show();

    }

  }

  

  if(state == 2)

  {

    //set 3 random to color 2

    for(int i = 0 ; i < 3; i++)

    {

      which = random(0,strip.numPixels()/2);

      which2 = which + strip.numPixels()/2;

      

      strip.setPixelColor(which,color3);

      strip.setPixelColor(which2,color3);

      strip.show();

    }

  }

  

  if(state == 3)

  {

    //set 3 random to color 2

    for(int i = 0 ; i < 3; i++)

    {

      which = random(0,strip.numPixels()/2);

      which2 = which + strip.numPixels()/2;

      

      strip.setPixelColor(which,color4);

      strip.setPixelColor(which2,color4);

      strip.show();

    }

  }

  

  if(state == 4)

  {

    //set 3 random to color 2

    for(int i = 0 ; i < 3; i++)

    {

      which = random(0,strip.numPixels()/2);

      which2 = which + strip.numPixels()/2;

      

      strip.setPixelColor(which,color1);

      strip.setPixelColor(which2,color1);

      strip.show();

    }

  }

  

  led_iterator++;

}

      

   



void bj_ep()

{

  //Not to be used!!!

  led_iterator = 10;

  int state;

  

  //pink

  uint32_t color1 = strip.Color(127,70,70);

  

  //green

  uint32_t color2 = strip.Color(0,127,0);

  

  

  

  if(led_iterator < 40)

  {

    state = 0;

  }

  else if(led_iterator < 80)

  {

    state = 1;

  }

  else

  {

    led_iterator = 0;

  }

  
led_iterator++;
  if(state == 0)

  {

    for(int i = 0 ; i < strip.numPixels()/6; i++)

    {

      strip.setPixelColor(i,color1);

    }  

    

    for(int i = 1*strip.numPixels()/6 ; i < 2*strip.numPixels()/6; i++)

    {

      strip.setPixelColor(i,color2);

    }

    

    for(int i = 2*strip.numPixels()/6 ; i < 3*strip.numPixels()/6; i++)

    {

      strip.setPixelColor(i,color1);

    }

    

    for(int i = 3*strip.numPixels()/6 ; i < 4*strip.numPixels()/6; i++)

    {

      strip.setPixelColor(i,color2);

    }

    

    for(int i = 4*strip.numPixels()/6 ; i < 5*strip.numPixels()/6; i++)

    {

      strip.setPixelColor(i,color1);

    }

    

    for(int i = 5*strip.numPixels()/6 ; i < 6*strip.numPixels()/6; i++)

    {

      strip.setPixelColor(i,color2);

    }

    

    strip.show();

  }

   

   

   if(state == 1)

  {

    for(int i = 0 ; i < strip.numPixels()/6; i++)

    {

      strip.setPixelColor(i,color2);

    }  

    

    for(int i = 1*strip.numPixels()/6 ; i < 2*strip.numPixels()/6; i++)

    {

      strip.setPixelColor(i,color1);

    }

    

    for(int i = 2*strip.numPixels()/6 ; i < 3*strip.numPixels()/6; i++)

    {

      strip.setPixelColor(i,color2);

    }

    

    for(int i = 3*strip.numPixels()/6 ; i < 4*strip.numPixels()/6; i++)

    {

      strip.setPixelColor(i,color1);

    }

    

    for(int i = 4*strip.numPixels()/6 ; i < 5*strip.numPixels()/6; i++)

    {

      strip.setPixelColor(i,color2);

    }

    

    for(int i = 5*strip.numPixels()/6 ; i < 6*strip.numPixels()/6; i++)

    {

      strip.setPixelColor(i,color1);

    }

    

    strip.show();

  } 

    

    

}

  

  

