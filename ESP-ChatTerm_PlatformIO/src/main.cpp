#include <Arduino.h>


#include<EEPROM.h>
#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>


const char* mqtt_server = "broker.mqtt-dashboard.com";
String ssid, pass;
long long int prev =0;
String msg_buff;
void enter();
void screensaver();
void IntCallback();

long lastMsg = 0;
char msg[50];
int value = 0;
int lenaddrSsid = 99;
int lenaddrPass = 100;
int ssidaddr = 101;
int passaddr = 150;
char pub_buff[50];
bool kill_switch;
int count = 0;





void boot_sequence(void);
String get_time();
void screensaver();





LiquidCrystal lcd(D5, D6, D3, D2, D1, D0);

WiFiClient espClient;
PubSubClient client(espClient);



void setup_wifi() {


int len = EEPROM.read(lenaddrSsid);

for (int i =0; i<len; i++)
{char n = EEPROM.read(ssidaddr+i);
  ssid+=n ;
}


int len2 = EEPROM.read(lenaddrPass);

for (int i =0; i<len2; i++)
{char n2 = EEPROM.read(passaddr+i);
  pass+=n2 ;
}


 delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  lcd.clear();
  lcd.print("Connecting to ");
  lcd.setCursor(0,1);
  lcd.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  lcd.clear();
  lcd.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("in:");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
        lcd.print((char)payload[i]);
  }
  prev = millis();
  Serial.println();
  
}

void reconnect() {

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      // ... and resubscribe
      client.subscribe("srihari/inTopic");
    } else {
      lcd.clear();
      lcd.print("Disconnected.. wait, trying again !");
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void welcome_screen()
{
  lcd.clear();

  lcd.print("Wlcm to chat 1.0");
  lcd.setCursor(0,1);
  lcd.print("By hari&ravi '19");
}



void setup() {

  pinMode(D4,OUTPUT);
  analogWrite(D4, 200);
  lcd.begin(16, 2);
   attachInterrupt(digitalPinToInterrupt(D7), IntCallback, FALLING);
    attachInterrupt(digitalPinToInterrupt(D8), enter, RISING);
   Serial.begin(115200);
  

  EEPROM.begin(512);
  kill_switch = EEPROM.read(108);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  welcome_screen();
  delay(3000);
 
}




void loop() {

if(kill_switch == false)
{ 
  screensaver();
   setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  while(1)
  {
 
    if(kill_switch == false){
  if (!client.connected()) {
        lcd.setCursor(0,1);
    lcd.print("MQTT NOT OK !");
    reconnect();
  }
  if(client.connected())
  {
    lcd.setCursor(0,1);
    lcd.print("MQTT ok!");
 
  }
  client.loop();



   screensaver();
  String got = Serial.readString();
    if(got.length() >0){
        msg_buff+= got;
        lcd.clear();
        lcd.print(msg_buff);
    
    
    }

    }
  }
}


else if(kill_switch == true)
{
  boot_sequence();
}



}



void boot_sequence()
{
   String p;
   char q;

  Serial.println(" 'Welcome to the boot-Sequence' ");
  Serial.println("This only Runs ONCE when you boot-up for the first time or you hit the kill switch !");
  Serial.println();Serial.println();
  Serial.println("----------------------------------------------------------------------------------");

Serial.println(" ENter the SSID and password one after other in serial window to setup");

 again: Serial.print("Enter the ssid: ");
 lcd.clear();
        lcd.print("Enter the ssid: ");
  while(!Serial.available());
  p = Serial.readString();
EEPROM.write(lenaddrSsid,p.length());
EEPROM.commit();
char buff[p.length()];
for (unsigned int i =0; i<p.length(); i++)
{
  buff[i] = p[i];  
}

for (unsigned int i =0; i<p.length(); i++)
{
  EEPROM.write(ssidaddr+i,buff[i]);
  EEPROM.commit();
  
}
  Serial.println(p);

  Serial.print("Password: ");
  lcd.clear();
    lcd.print("Password: ");
    while(!Serial.available());
    p = Serial.readString();

    
EEPROM.write(lenaddrPass,p.length());
EEPROM.commit();
char buff2[p.length()];
for (unsigned int i =0; i<p.length(); i++)
{
  buff2[i] = p[i];  
}
for (unsigned int i =0; i<p.length(); i++)
{
  EEPROM.write(passaddr+i,buff2[i]);
  EEPROM.commit();
  
}

Serial.println(p);

  Serial.println("Is the above entered info correct? To confirm enter 'yes' or 'no' to re-enter 'em");
    while(!Serial.available());
  q = Serial.read();
  if(q == 'y')
  {

    Serial.println(q);
      Serial.println(" Okay then, attempting to connect to Wi-Fi. Sitback and relax. ");
      
  }

 if(q == 'n')
  {
    Serial.println(q);
    Serial.println(" No probs.. mistakes happen. Please re-enter.. ");
    goto again;
  }
  lcd.clear();
  lcd.setCursor(0,0);

  lcd.print("ssid: ");
  lcd.println(ssid);
  lcd.setCursor(0,1);
  lcd.print("pass: ");
  lcd.print(pass);
       kill_switch = !kill_switch;
       EEPROM.write(108, kill_switch);
       EEPROM.commit();
       
  delay(3000);
  welcome_screen();
  

  
  
}



String get_time()
{
  String samay;
 String beg = "http://api.thingspeak.com/apps/thinghttp/send_request?api_key=18OGDYOI5VV43ED8";
HTTPClient http;  //Declare an object of class HTTPClient
http.begin(beg);  //Specify request destination
int httpCode = http.GET();                                                                  //Send the request
 
if (httpCode > 0) { //Check the returning code
 
  samay = http.getString();   //Get the request response payload

samay.trim();  String beg = "http://api.thingspeak.com/apps/thinghttp/send_request?api_key=18OGDYOI5VV43ED8";
HTTPClient http;  //Declare an object of class HTTPClient
http.begin(beg);  //Specify request destination
int httpCode = http.GET();                                                                  //Send the request
 
if (httpCode > 0) { //Check the returning code
 
  samay = http.getString();   //Get the request response payload

samay.trim();
}
 
http.end();   //Close connection

return samay;
}




void IntCallback()
{

    
  
  kill_switch = !kill_switch;
         EEPROM.write(108, kill_switch);
       EEPROM.commit();

}



void screensaver()
{
  uint32_t now = millis();
  if (now - prev > 30000)
  {
    lcd.clear();
    String t = get_time();
    lcd.print(t);
    prev = now;
  }
  
}


void enter()
{
     msg_buff.toCharArray(pub_buff,50);
    snprintf (msg, 50, pub_buff);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("srihari/outTopic", msg);
    lcd.clear();
    lcd.print("out: ");
    lcd.print(msg);
  msg_buff = "";
}