 
 /*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 \\
31.01.2017 by Jan Hendrik Berlin
 
 */
#include <EEPROM.h>
#include <WiFi.h>
// use first channel of 16 channels (started from zero)
/*
#define LEDC_CHANNEL_0     5
#define LEDC_CHANNEL_1     17
#define LEDC_CHANNEL_2     16
#define LEDC_CHANNEL_3     4
//*/
//*
#define LEDC_CHANNEL_0     0
#define LEDC_CHANNEL_1     1
#define LEDC_CHANNEL_2     2
#define LEDC_CHANNEL_3     3
//*/
// use 13 bit precission for LEDC timer
#define LEDC_TIMER_13_BIT  13

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     50000

// fade LED PIN (replace with LED_BUILTIN constant for built-in LED)
#define LED_PIN1            5
#define LED_PIN2            22

#define PWM1            5
#define PWM2            17
#define PWM3            16
#define PWM4            4
//#define LED_PIN2            LED_BUILTIN

IPAddress local_IP(192, 168, 8, 5);
// Set your Gateway IP address
IPAddress gateway(192, 168, 8, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

const char* ssid     = "";
const char* password = "";
uint32_t PWMValue ; 
WiFiServer server(80);

uint32_t CurrentBrightness[4] = { 0 , 0 , 0 , 0 } ; 
int ledChannels[4]  = {LEDC_CHANNEL_0 , LEDC_CHANNEL_1, LEDC_CHANNEL_2, LEDC_CHANNEL_3} ;
//uint32_t globalBright ;
//uint32_t Ledz[4] = {0,0,0,0} ; 

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {
  //globalBright = value ;
  PWMValue= value ; //store current vlaue for reporting
  value = (8191 *value /  valueMax); //make out of 8191
  value = value * value / 8191 ;//make nonlinear
  value = 8192 - value ; // the led is inverted , so we invert the value
  
  // write duty to LEDC
  ledcWrite(channel, value);
  
  //Serial.println("Pwm value updated to " );
 // Serial.println(value );
}

void setup()
{
  WiFi.mode(WIFI_STA);
  // Setup timer and attach timer to a led pin
  
    Serial.begin(115200);
    //pinMode(5, OUTPUT);      // set the LED pin mode
    
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcSetup(LEDC_CHANNEL_1, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcSetup(LEDC_CHANNEL_2, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcSetup(LEDC_CHANNEL_3, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  
  ledcAttachPin(PWM1, LEDC_CHANNEL_0);
  ledcAttachPin(PWM2, LEDC_CHANNEL_1);
  ledcAttachPin(PWM3, LEDC_CHANNEL_2);
  ledcAttachPin(PWM4, LEDC_CHANNEL_3);


    delay(10);
   // Configures static IP address
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
      Serial.println("STA Failed to configure");
    }
    // We start by connecting to a WiFi network

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    WiFi.mode(WIFI_STA);
    server.begin();

    pinMode(32, INPUT);
    pinMode(35, INPUT);
    pinMode(34, INPUT);


    //we dont want to melt our eyes on a power restore
    EEPROM.begin(4);
    Serial.print(EEPROM.read(0));
    Serial.print(",");
    Serial.print(EEPROM.read(1));
    Serial.print(",");
    Serial.print(EEPROM.read(2));
    Serial.print(",");
    Serial.print(EEPROM.read(3));
    ledcAnalogWrite(LEDC_CHANNEL_0, EEPROM.read(0) , 100 );
    ledcAnalogWrite(LEDC_CHANNEL_1, EEPROM.read(1) , 100 );
    ledcAnalogWrite(LEDC_CHANNEL_2, EEPROM.read(2) , 100 );
    ledcAnalogWrite(LEDC_CHANNEL_3, EEPROM.read(3) , 100 );
}

int value = 0;
int disconnectedtimer = 0 ; 
int statscounter = 0 ; 
void loop(){
    statscounter++; 
    if(statscounter==100000)
    {
      statscounter= 0 ; 
      /*
      Serial.println(WiFi.status());
      Serial.println("32");
      Serial.println(digitalRead(32));
      Serial.println("35");
      Serial.println(digitalRead(35));
      Serial.println("34");
      Serial.println(digitalRead(34));
      */
    }
    //check if we still are connected to ap
//    delay(500);
//    Serial.println(WiFi.status());
    /*
    Serial.println("WL_CONNECTED");
    Serial.println(WL_CONNECTED);
    Serial.println("WL_NO_SHIELD");
    Serial.println(WL_NO_SHIELD);
    Serial.println("WL_IDLE_STATUS");
    Serial.println("WL_NO_SSID_AVAIL");
    Serial.println(WL_NO_SSID_AVAIL);
    Serial.println("WL_SCAN_COMPLETED");
    Serial.println(WL_SCAN_COMPLETED);
    Serial.println("WL_CONNECT_FAILED");
    Serial.println(WL_CONNECT_FAILED);
    Serial.println("WL_CONNECTION_LOST");
    Serial.println(WL_CONNECTION_LOST);
    Serial.println("WL_DISCONNECTED");
    Serial.println(WL_DISCONNECTED);
    */
/*
 *    WL_CONNECTED: assigned when connected to a WiFi network;
    WL_NO_SHIELD: assigned when no WiFi shield is present;
    WL_IDLE_STATUS: it is a temporary status assigned when WiFi.begin() is called and remains active until the number of attempts expires (resulting in WL_CONNECT_FAILED) or a connection is established (resulting in WL_CONNECTED);
    WL_NO_SSID_AVAIL: assigned when no SSID are available;
    WL_SCAN_COMPLETED: assigned when the scan networks is completed;
    WL_CONNECT_FAILED: assigned when the connection fails for all the attempts;
    WL_CONNECTION_LOST: assigned when the connection is lost;
    WL_DISCONNECTED: assigned when disconnected from a network;  
 *
 */
    if(WiFi.status() == WL_DISCONNECTED || WiFi.status() == WL_CONNECTION_LOST|| WiFi.status()==1  ) {
      if(disconnectedtimer < 200) 
      {
        
        Serial.print(".");
        disconnectedtimer++ ;
        delay(100);
        
      }else
      {
        WiFi.mode(WIFI_STA);
        //reconnect wifi
        disconnectedtimer = 0 ;
        WiFi.reconnect();
//        WiFi.begin(ssid, password);
//        while (WiFi.status() != WL_CONNECTED) {
//            delay(500);
//            Serial.print(".");
//        }
      }
      
    }
  WiFiClient client = server.available();   // listen for incoming clients
  bool returnStatus = false; 
  if (client) {                             // if you get a client,
//    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
//        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            if(!returnStatus)
              printPage(client);
              else
              {
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-type:text/html");
                  client.println();
//                  client.println("Current brightnessis ");
                  client.println(PWMValue);
              }
            
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(5, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(5, LOW);                // GET /L turns the LED off
        }
        if(currentLine.endsWith("HTTP/1.1"))
        {
           Serial.println(currentLine);
        }
        if (currentLine.indexOf("setch=") > 0&& currentLine.endsWith("HTTP/1.1")) //example setch=0,123 two params, channel and then duty cycle
        {
          
          int ch,duty ; 
          
          int pos = currentLine.indexOf("setch=");
          String brightness = currentLine.substring(pos+6 , currentLine.indexOf(',', pos)) ; 
          ch = brightness.toInt();
          pos = currentLine.indexOf(',', pos+1);
          brightness = currentLine.substring(pos+1 , currentLine.indexOf(',', pos+1)) ; 
          Serial.println(brightness);
          duty = brightness.toInt();

          if(ch == 0 ) 
            ledcAnalogWrite(LEDC_CHANNEL_0, duty , 100 );
          if(ch == 1 ) 
            ledcAnalogWrite(LEDC_CHANNEL_1, duty , 100 );
          if(ch == 2 ) 
            ledcAnalogWrite(LEDC_CHANNEL_2, duty , 100 );
          if(ch == 3 ) 
            ledcAnalogWrite(LEDC_CHANNEL_3, duty , 100 );
          
        }
        if (currentLine.indexOf("fadech=") > 0&& currentLine.endsWith("HTTP/1.1")) //example setch=0,123 three params, channel and then duty cycle and delay
        {
          
          int ch,bright , fade; 
          Serial.println("FADEING");
          int pos = currentLine.indexOf("fadech=");
          String brightness = currentLine.substring(pos+7 , currentLine.indexOf(',', pos)) ; 
          Serial.println(brightness);
          ch = brightness.toInt();
          pos = currentLine.indexOf(',', pos+1);
          brightness = currentLine.substring(pos+1 , currentLine.indexOf(',', pos+1)) ; 
          Serial.println(brightness);
          bright = brightness.toInt();

          pos = currentLine.indexOf(',', pos+1);
          brightness = currentLine.substring(pos+1 , currentLine.indexOf(',', pos+1)) ; 
          Serial.println(brightness);
          fade = brightness.toInt(); 
          

            
           while(CurrentBrightness[ch] !=bright)
           {
            Serial.println("STEP");
              delay(fade); 
              ledcAnalogWrite(ledChannels[ch], CurrentBrightness[ch] , 100 );
              
              if( CurrentBrightness[ch] < bright)
              {
                CurrentBrightness[ch] ++ ; 
              }
              else
              {
                CurrentBrightness[ch] -- ; 
                
              }
           }  
          
        }
        if (currentLine.indexOf("setf=") > 0&& currentLine.endsWith("HTTP/1.1")) //example setf=0,123 two params, channel and then feq 0 - 5000
        {
          Serial.println(currentLine); 
          Serial.println("setf");
          int values[5]  ; //new brightness values for each led

          int pos = currentLine.indexOf("setf=");
          String brightness = currentLine.substring(pos+5 , currentLine.indexOf(',', pos)) ; 
          values[0] =brightness.toInt();
          
            Serial.print("values[0] = ") ; 
            Serial.print(values[0]) ; 
            Serial.println() ; 
          for(int i=1 ; i<5 ; i++)
          {
            
            pos = currentLine.indexOf(',', pos+1);
            brightness = currentLine.substring(pos+1 , currentLine.indexOf(',', pos+1)) ; 
            Serial.println(brightness);
            values[i] = brightness.toInt();  
            Serial.print("values[i] = ") ; 
            Serial.print(values[i]) ; 
            Serial.println() ; 
          }
          int maxDiff = 0  ; //abs(CurrentBrightness[0] - values[0]) ;
          int diffs[5] ; 
          for(int i=0 ; i<4 ; i++)
          {
            /*
            int tempDiff = abs(CurrentBrightness[i] - values[i]) ;
            if(tempDiff >  maxDiff)
              maxDiff = tempDiff ; 
              */
              
            diffs[i] = values[i] - CurrentBrightness[i] ; //postive steps for increas in brightness , negative for decrease
            Serial.print("diffs") ;
            Serial.print( diffs[i] );
          }
          Serial.println(); 
          for(int i=1 ; i<=100 ; i++)
          {
            ledcAnalogWrite(LEDC_CHANNEL_0, CurrentBrightness[0] + ((i*diffs[0])/100) , 100 );
            ledcAnalogWrite(LEDC_CHANNEL_1, CurrentBrightness[1] + ((i*diffs[1])/100) , 100 );
            ledcAnalogWrite(LEDC_CHANNEL_2, CurrentBrightness[2] + ((i*diffs[2])/100) , 100 );
            ledcAnalogWrite(LEDC_CHANNEL_3, CurrentBrightness[3] + ((i*diffs[3])/100) , 100 );
            Serial.print("new led 0 ");
            Serial.print(  CurrentBrightness[3] + ((i*diffs[3])/100 )  ) ;
            
            Serial.println() ; 
            delay( values[4]/100); //1/100th of the delay as at most we will step 100 times :D
          }
          CurrentBrightness[0] = values[0] ;
          CurrentBrightness[1] = values[1] ;
          CurrentBrightness[2] = values[2] ;
          CurrentBrightness[3] = values[3] ;
          ledcAnalogWrite(LEDC_CHANNEL_0, values[0] , 100 );
          ledcAnalogWrite(LEDC_CHANNEL_1, values[1] , 100 );
          ledcAnalogWrite(LEDC_CHANNEL_2, values[2] , 100 );
          ledcAnalogWrite(LEDC_CHANNEL_3, values[3] , 100 );
        }
        if (currentLine.indexOf("set=") > 0&& currentLine.endsWith("HTTP/1.1")) //example set=0,123,333,444
        {
          int led0,led1,led2,led3 ; //new brightness values for each led
          
          int pos = currentLine.indexOf("set=");
          String brightness = currentLine.substring(pos+4 , currentLine.indexOf(',', pos)) ; 
          led0 = brightness.toInt();
          pos = currentLine.indexOf(',', pos+1);
          brightness = currentLine.substring(pos+1 , currentLine.indexOf(',', pos+1)) ; 
          Serial.println(brightness);
          led1 = brightness.toInt();

          pos = currentLine.indexOf(',', pos+1);
          brightness = currentLine.substring(pos+1 , currentLine.indexOf(',', pos+1)) ; 
          Serial.println(brightness);
          led2 = brightness.toInt();

          pos = currentLine.indexOf(',', pos+1);
          brightness = currentLine.substring(pos+1 , currentLine.indexOf(',', pos+1)) ; 
          Serial.println(brightness);
          led3 = brightness.toInt();

          Serial.println("Entered set function");
          Serial.print(led0);
          Serial.print(",");
          Serial.print(led1);
          Serial.print(",");
          Serial.print(led2);
          Serial.print(",");
          Serial.println(led3);
          EEPROM.write(0 , led0) ;
          EEPROM.write(1 , led1) ;
          EEPROM.write(2 , led2) ;
          EEPROM.write(3 , led3) ;
          EEPROM.commit();
          
          Serial.print(EEPROM.read(0));
          Serial.print(",");
          Serial.print(EEPROM.read(1));
          Serial.print(",");
          Serial.print(EEPROM.read(2));
          Serial.print(",");
          Serial.println(EEPROM.read(3));
          
          CurrentBrightness[0] = led0 ;
          CurrentBrightness[1] = led1 ;
          CurrentBrightness[2] = led2 ;
          CurrentBrightness[3] = led3 ;
          ledcAnalogWrite(LEDC_CHANNEL_0, led0 , 100 );
          ledcAnalogWrite(LEDC_CHANNEL_1, led1 , 100 );
          ledcAnalogWrite(LEDC_CHANNEL_2, led2 , 100 );
          ledcAnalogWrite(LEDC_CHANNEL_3, led3 , 100 );

      
          
           
        }
        /*
        if (currentLine.indexOf("brightness=") > 0  && currentLine.indexOf("speed=") > 0 && currentLine.endsWith("HTTP/1.1")) 
        {
           Serial.println("SOFT CHANGE");
          returnStatus =true; 
          int pos = currentLine.indexOf("brightness=");
          String brightness = currentLine.substring(pos+11 , currentLine.indexOf('&', pos)) ;
          int bright = brightness.toInt();
        
          pos = currentLine.indexOf("speed=");
          String speedness = currentLine.substring(pos+6 , currentLine.indexOf(' ', pos)) ;
          int speed = speedness.toInt(); 
          
           Serial.println(brightness);
           Serial.println(speedness);
           
          if(globalBright < bright )
          {
            for(int i = globalBright; i<= bright ; i=i+1)
            {
            ledcAnalogWrite(LEDC_CHANNEL_0, i , 100 );
            delay(speed*1);
            }
          }else
          {
            for(int i = globalBright; i>= bright ; i = i - 1)
            {
            ledcAnalogWrite(LEDC_CHANNEL_0, i , 100 );
            delay(speed*1);
            }
            
          }
          
          digitalWrite(5, LOW);                // GET /L turns the LED off
        }
        //*/
        if (currentLine.startsWith("GET /value?brightness=") && currentLine.endsWith("HTTP/1.1")) 
        {
          
           Serial.println("HARD CHANGE");
          returnStatus =true; 
          int pos = currentLine.indexOf('=');
          String brightness = currentLine.substring(pos+1 , currentLine.indexOf(' ', pos)) ;
          int bright = brightness.toInt();
          ledcAnalogWrite(LEDC_CHANNEL_0, bright , 100 );
          
          digitalWrite(5, LOW);                // GET /L turns the LED off
        }
        //*/
        
      }
    }
    // close the connection:
    client.stop();
//    Serial.println("Client Disconnected.");
  }
}
void printPage(WiFiClient client)
{
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/html");
      client.println();

     // the content of the HTTP response follows the header:
     client.print("<script type='text/javascript'> \n\
                  function updateled()\n\
                  {\n\
                    var url = '/value' + '?' + 'brightness=' + document.getElementById('bright').value;\n\
                    httpGetAsync(url)\n\
                  }\n\
                  function httpGetAsync(theUrl )\n\
                  {\n\
                      var xmlHttp = new XMLHttpRequest();\n\
                      xmlHttp.onreadystatechange = function() { \n\
                          if (xmlHttp.readyState == 4 && xmlHttp.status == 200)\n\
                          console.log(xmlHttp.responseText);\n\
                          document.getElementById('head').innerHTML = xmlHttp.responseText;\n\
                      }\n\
                      xmlHttp.open('GET', theUrl, true); \n\
                      xmlHttp.send(null);\n\
                  }\n</script>\n");
     client.print("<h1>Led brightness is set to : <span id='head'>");
     client.print(PWMValue);
     client.print("</span></h1>");
//     client.print("<h1>Click <a href='/L'>here</a> to turn the LED on pin 5 on.<br></h1>\n");
//     client.print("<h1>Click <a href='/H'>here</a> to turn the LED on pin 5 off.<br></h1>\n");
     client.print("Brightness <input onchange='updateled()' max='100' step='5' min='0' type='number' id='bright'> \n");
//     client.print("Brightness <input onchange='updateled()' type='number' id='bright2'> \n");
      

}
