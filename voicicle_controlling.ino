#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


//#define echo_in D5
//#define trig_out D7
//#define obstaclePin_back D6

#define motorPin1 D1
#define motorPin2 D2
#define motorPin3 D3
#define motorPin4 D4
#define ignite_on D0

//float s = 0.034;
//float t,d;

#define WLAN_SSID       "AndroidAP"
#define WLAN_PASS       "erhb1548"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Voicicle"
#define AIO_KEY         "b27dd192ef204e28baf304e35e5aecd1"

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Controller");
Adafruit_MQTT_Subscribe light2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Ignition");

void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);

//  pinMode(trig_out,OUTPUT);
//  pinMode(echo_in,INPUT);

  pinMode(motorPin1,OUTPUT);
  pinMode(motorPin2,OUTPUT);
  pinMode(motorPin3,OUTPUT);
  pinMode(motorPin4,OUTPUT);

  pinMode(ignite_on,OUTPUT);
  

  
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  mqtt.subscribe(&light1);
  mqtt.subscribe(&light2);
}

uint32_t x=0;
void forward()
{
    //forward
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
}
void backward()
{   
    //backward
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
}   

void left()
{    
    //left
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
    delay(3000);
    forward();
}
void right()
{     
    //right
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
    delay(3000);
    forward();
}      

void halt()
{
    //stop
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);  
}


void loop() {

  //int hasObstacle_front = digitalRead(obstaclePin_front);
//####  int hasObstacle_back = digitalRead(obstaclePin_back);
  /*if(hasObstacle_front==0)
  {
    Serial.println("Stop something is ahead!!");
    halt();
  }*/
  
/*####  if(hasObstacle_back==0)
  {
    Serial.println("Stop something is at back!!");
    halt();
  }

  digitalWrite(trig_out,LOW);
  delay(2);
  digitalWrite(trig_out,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_out,LOW);
  t = pulseIn(echo_in,1);
  d = s*t/2;
  Serial.println(d);
  if(d<80)
  {
    Serial.println("Stop something is ahead!!");
    halt();
  }


  else
  {
####*/  

  
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    //if (subscription == &light1) 
      Serial.print(F("Got: "));
      Serial.println((char *)light1.lastread);
      int command_number=atoi((char *)light1.lastread);
    
    //if (subscription == &light2){
       Serial.print(F("Got: "));
      Serial.println((char *)light2.lastread);
      int ignite=atoi((char *)light2.lastread);
    
      Serial.println(command_number);
      Serial.println(ignite);
      if (ignite == 0)
      {
        Serial.println("Turn on Ignition");
        
        digitalWrite(ignite_on,LOW);
        halt();
      }
      else
      {
      
      digitalWrite(ignite_on,HIGH);
      
      
      if (command_number == 20)
      {
        forward();
      }
      if (command_number == 40)
      {
        backward();
      }
      if (command_number == 60)
      {
        left();
      }
      if (command_number == 80)
      {
        right();
      }
      if (command_number == 100 )
      {
        halt();
      }
      
      }
      //digitalWrite(LED1,light1_state);               #####
    //}
    
  }

  // Now we can publish stuff!
  /*Serial.print(F("\nSending photocell val "));
  Serial.print(x);
  Serial.print("...");
  if (! photocell.publish(x++)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
}
//###}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
