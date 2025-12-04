#include <WiFi.h>
#include <HTTPClient.h>
// #include <Arduino_JSON.h>
#include <ArduinoJSON.h>
#include <ESPmDNS.h>
#define ADDR  "tappa" 
// WiFiServer server(80);
//valori per scheda relè esterna
// #define ON_Board_LED 2
// #define RELE_01 26 
// #define RELE_02 27 
//NB il relè ha la logica invertita

//valori per scheda relè a 220V
#define ON_Board_LED 23
#define RELE_01 16 
#define RELE_02 17 

String board="ESP32_02";
// String site="http://hp-i3/tappa/";
// http.begin("http://dannaviaggi.altervista.org/getdata.php");  //--> Specify request destination
// http.begin("http://hp-i3-ok/tappa/getdata.php");  //--> Specify request destination
const char* site = "http://hp-i3-ok/tappa/";
char destination[255];
// const char* ssid = "TIM-39751438";
const char* ssid = "TIM-39751438_EXT";
const char* password = "EFuPktKzk6utU2y5a5SEkUUQ";
String payload = "";  //--> Variable for receiving response from HTTP POST.
String postData = "";
HTTPClient http;  //--> Declare object of class HTTPClient.
int httpCode;     //--> Variables for HTTP return code.
int connecting_process_timed_out = 40; // 20 sec
const char* activity = nullptr;
const char* gssid = nullptr; 
const char* gsite = nullptr; 
JsonDocument doc; 

int tempo;
int delta;
String status;

void updatedata(){
  payload = "";
  postData = "board="; //--> Variables sent for HTTP POST request data.
  postData += board; //--> Variables sent for HTTP POST request data.
  postData +="&activity=OFF";
  Serial.println("---------------");
  Serial.println("updatedata");  
  Serial.println(postData);
  strcpy(destination ,site);
  strcat(destination ,"updatedata.php");
  Serial.println(destination);
  http.begin(destination);  //--> Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
  httpCode = http.POST(postData); //--> Send the request
  payload = http.getString();     //--> Get the response payload
  Serial.print("httpCode : ");
  Serial.println(httpCode); //--> Print HTTP return code
  Serial.print("payload  : ");
  Serial.println(payload);  //--> Print request response payload
  http.end();  //--> Close connection
}

void getdata(){
  payload = "";
  postData = "board="; //--> Variables sent for HTTP POST request data.
  postData += board; //--> Variables sent for HTTP POST request data.
  Serial.println("---------------");
  Serial.println("getdata");
  Serial.println(postData);
  strcpy(destination ,site);
  strcat(destination ,"getdata.php");
  Serial.println(destination);
  http.begin(destination);  //--> Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
  httpCode = http.POST(postData); //--> Send the request
  payload = http.getString();     //--> Get the response payload
  Serial.print("httpCode : ");
  Serial.println(httpCode); //--> Print HTTP return code
  Serial.print("payload  : ");
  Serial.println(payload);  //--> Print request response payload
  http.end();  //--> Close connection
}

void config(){
  connecting_process_timed_out = 10;
  WiFi.mode(WIFI_STA);
  WiFi.begin("Redmi14c","manu1234");
  Serial.println("\n***********************************************");
  Serial.println("Connecting to MASTER");
  Serial.println("***********************************************");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(ON_Board_LED,HIGH);
    delay(100);
    digitalWrite(ON_Board_LED,LOW);
    delay(100);
    digitalWrite(ON_Board_LED,HIGH);
    delay(100);
    digitalWrite(ON_Board_LED,LOW);
    delay(1000);
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) {
      Serial.println("\n***********************************************");
      Serial.println("NOT connected to MASTER");
      Serial.println("***********************************************");
      return;
    }
  }
  Serial.println("\nSuccessfully connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("-------------");
  Serial.println("Abilito dns");
  if (MDNS.begin(ADDR)) {
    Serial.println("Abilitato");
  }
  payload = "";
  postData = "board="; //--> Variables sent for HTTP POST request data.
  postData += board; //--> Variables sent for HTTP POST request data.
  strcat(destination ,site);
  strcat(destination ,"getdata.php");
  Serial.println("---------------");
  Serial.println(destination);

  http.begin(destination);  //--> Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
  httpCode = http.POST(postData); //--> Send the request
  payload = http.getString();     //--> Get the response payload
  Serial.print("httpCode : ");
  Serial.println(httpCode); //--> Print HTTP return code
  Serial.print("payload  : ");
  Serial.println(payload);  //--> Print request response payload
  http.end();  //--> Close connection
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print(F("Failed to parse JSON: "));
    Serial.println(error.f_str());
  } else  {
    gsite = doc["site"];
    Serial.print("gssid ");
    Serial.println(gsite);
    Serial.print("site ");
    Serial.println(site);
    if(strcmp(gsite, site) != 0 & strcmp(gsite, "") != 0){
      Serial.println("***********************************************");
      Serial.println("cambio SSID");
      Serial.println("***********************************************");
      ssid=gssid;
    } else  {
      Serial.println("***********************************************");
      Serial.println("SSID CONFERMATO");
      Serial.println("***********************************************");
    } 
    gssid = doc["SSID"];
    Serial.print("gssid ");
    Serial.println(gssid);
    Serial.print("SSID ");
    Serial.println(ssid);
    if(strcmp(gssid, ssid) != 0 & strcmp(gssid, "") != 0){
      Serial.println("***********************************************");
      Serial.println("cambio SSID");
      Serial.println("***********************************************");
      ssid=gssid;
    } else  {
      Serial.println("***********************************************");
      Serial.println("SSID CONFERMATO");
      Serial.println("***********************************************");
    } 
  }
}

void connect(){
  connecting_process_timed_out = 100;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  Serial.println("***********************************************");
  Serial.println("Connecting to ");
  Serial.println(ssid);
  Serial.println("***********************************************");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(ON_Board_LED,HIGH);
    delay(100);
    digitalWrite(ON_Board_LED,LOW);
    delay(100);
    digitalWrite(ON_Board_LED,HIGH);
    delay(100);
    digitalWrite(ON_Board_LED,LOW);
    delay(100);
    digitalWrite(ON_Board_LED,HIGH);
    delay(100);
    digitalWrite(ON_Board_LED,LOW);
    delay(1000);
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) ESP.restart();
  }
  Serial.println("\nSuccessfully connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("-------------");
  Serial.println("Abilito dns");
  if (MDNS.begin(ADDR)) {
    Serial.println("Abilitato");
  }
}

void setup() {
  Serial.begin(115200); //--> Initialize serial communications with the PC.
  Serial.println("\n-------------");
  Serial.println("Initialized serial communications with the PC.");
  pinMode(RELE_01,OUTPUT); //--> RELE_01 port Direction output.
  pinMode(RELE_02,OUTPUT); //--> RELE_02 port Direction output.
  pinMode(ON_Board_LED,OUTPUT); //--> RELE_02 port Direction output.
  digitalWrite(RELE_01, HIGH); //--> Turn off Led RELE_01.
  delay(250);
  digitalWrite(RELE_01, LOW); //--> Turn off Led RELE_01.
  delay(250);
  digitalWrite(RELE_02, HIGH); //--> Turn off Led RELE_02.
  delay(250);
  digitalWrite(RELE_02, LOW); //--> Turn off Led RELE_02.
  delay(250);

  config();
  connect();
  
  status="OFF";
  tempo=0;
  delta=0;
}

void relays(){
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print(F("Failed to parse JSON: "));
    Serial.println(error.f_str());
  } else  {
    activity = doc["activity"];
    Serial.print("activity ");
    Serial.println(activity);
    if(strcmp(activity, "OFF") == 0 && status!="OFF"){
      digitalWrite(RELE_01, LOW); 
      digitalWrite(RELE_02, LOW); 
      status="OFF";
      Serial.println("***********************************************");
      Serial.println("SPEGNIMENTO");
      Serial.println("***********************************************");
    }
    if(strcmp(activity, "UP") == 0 && status!="UP"){
      digitalWrite(RELE_02, LOW);
      delay(100);//aspetto per evitare inerzia motore in caso di inversione
      digitalWrite(RELE_01, HIGH); 
      status="UP";
      tempo=millis();
      delta=0;
      Serial.println("***********************************************");
      Serial.println("APERTURA ");
      Serial.println("***********************************************");
    }
    if(strcmp(activity, "DOWN") == 0 && status!="DOWN"){
      digitalWrite(RELE_01, LOW);
      delay(100);//aspetto per evitare inerzia motore in caso di inversione
      digitalWrite(RELE_02, HIGH); 
      status="DOWN";
      tempo=millis();
      delta=0;
      Serial.println("***********************************************");
      Serial.println("CHIUSURA ");
      Serial.println("***********************************************");
    }
    delta=millis()-tempo;
    if(delta > 30000  & status!="OFF"){
      digitalWrite(RELE_01, LOW); 
      digitalWrite(RELE_02, LOW); 
      status="OFF";
      Serial.println("***********************************************");
      Serial.println("TIMEOUT");
      Serial.println("***********************************************");
      updatedata();
    }
  }
}

void loop() {
  // if(WiFi.status()== WL_CONNECTED) {
  //sostituendo .waitForConnectResult a .status non si hanno falsi errori
  if(WiFi.waitForConnectResult()== WL_CONNECTED) {
    digitalWrite(ON_Board_LED,HIGH);
    delay(100);
    digitalWrite(ON_Board_LED,LOW);
    delay(2000);
    getdata();//mettendo un delay dopo la wait getdata fallisce con minore frequnza (?)
    relays();
    Serial.print("status ");
    Serial.println(status);
    Serial.print("tempo ");
    Serial.println(tempo);
    Serial.print("delta ");
    Serial.println(delta);
  }
  else{ 
    config();
    connect();
  }
}