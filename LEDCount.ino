#include <ESP8266WiFi.h>
#include <PubSubClient.h>
int ButtonOut = 5; //PinD1
int ButtonIn = 4;  //pinD2
int Count = 0 ;
int R_Button = 0;
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 40;    // the debounce time; increase if the output flickers

const char *ssid = "your ssid";
const char *password = "your ssid password";

// MQTT
const char *mqtt_broker = "MQTT server";
const char *topic1 = "==topic==";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);
String client_id = "Arduino-UTCC";

#define SQL1 "insert into nb_data values (null,"
#define SQL2 ","
#define SQL3 ",null)"
#define SID 31001

volatile static unsigned long last_interrupt_time = 0;
/*void ICACHE_RAM_ATTR blink() { // interrupt do not use Serial.print and delay() in here (error)
  
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 70UL)  // ignores interupts for 100milliseconds
  {
      if (digitalRead(RedButton) == 0) { // detect toggle           
         R_Button = 1;
        
      }
      last_interrupt_time = interrupt_time;

  } 
 }*/


void callback(char* topic1, byte* payload, unsigned int length);
void callback(char* topic1, byte* payload, unsigned int length) {
  
if ((char)payload[12] == 'o' && (char)payload[13] == 'n') {
digitalWrite( ButtonOut , HIGH);
Count++;
delay(100);
digitalWrite( ButtonOut ,LOW);
}
else if ((char)payload[12] == 'r' && (char)payload[13] == 'e') {
digitalWrite( ButtonOut , HIGH);
Count = 0;
delay(100);
digitalWrite( ButtonOut ,LOW);
}
}
void setup() {
  Serial.begin(9600);
  pinMode(ButtonOut, OUTPUT);
  pinMode(ButtonIn, INPUT_PULLUP);
   digitalWrite(ButtonOut, HIGH);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid,password);

while(WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
 Serial.println(WiFi.localIP());

  Serial.println("Connected to the WiFi network");

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  
  while (!client.connected()) {
      
     // client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         
         // client.subscribe(topic); //old 21/12/2564
          client.subscribe(topic1); 
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }   
  }
    //attachInterrupt(digitalPinToInterrupt(RedButton), blink, FALLING);
}
void loop() {
//RButton(); 
   delay(50);
  int reading = digitalRead(ButtonIn);
  //Serial.print(reading);
   if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    
     if (reading != buttonState) {
        buttonState = reading;
        //Serial.print(reading);
        if (buttonState == LOW) {
        digitalWrite(ButtonOut, LOW);
        //Serial.print("out");
        //Serial.println(ButtonOut);
        //Serial.println("LOW");
        delay(100);
        digitalWrite(ButtonOut, HIGH);
        //Serial.print("out");
        //Serial.println(ButtonOut);
        Count++;
         client.loop();
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         
         // client.subscribe(topic); //old 21/12/2564
          client.subscribe(topic1); 
          //Serial.println("Public emqx mqtt broker connected");
      } else {
          //Serial.print("failed with state ");
          //Serial.print(client.state());
          //delay(2000);
      } 
char sql[256];                                                       
    snprintf(sql, sizeof sql, "%s%d%s%d%s%d%s%d%s%d%s", SQL1, SID, SQL2, Count, SQL2, 0, 
    SQL2,0 , SQL2,0 , SQL3);
     //Serial.print(sql);
  if (client.publish(sql, "any thing") == true) {
      //Serial.print("Successful sending: ");
      //Serial.println(sql);
    } 
    else {
      //Serial.println("Failed sending.");
    }
  delay(100);
      }
     }
  }
 lastButtonState = reading;
 

//Serial.println(digitalRead(RedButton));

  }
