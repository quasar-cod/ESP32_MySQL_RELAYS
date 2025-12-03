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

int tempo =0;
int delta =0;
String status="OFF";

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
  JsonDocument doc; 
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print(F("Failed to parse JSON: "));
    Serial.println(error.f_str());
  } else  {
    gssid = doc["SSID"];
    Serial.print("gssid ");
    Serial.println(gssid);
    Serial.print("SSID ");
    Serial.println(ssid);
    if(strcmp(gssid, ssid) != 0 && status=="OFF"){
      Serial.println("***********************************************");
      Serial.println("RICOLLEGAMENTO");
      Serial.println("***********************************************");
    } else  {
      Serial.println("***********************************************");
      Serial.println("COLLEGAMENTO CONFERMATO");
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

void updatedata(){
  payload = "";
  postData = "board="; //--> Variables sent for HTTP POST request data.
  postData += board; //--> Variables sent for HTTP POST request data.
  postData +="&activity=OFF";
  // http.begin("http://dannaviaggi.altervista.org/getdata.php");  //--> Specify request destination
  http.begin("http://hp-i3-ok/tappa/updatedata.php");  //--> Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
  httpCode = http.POST(postData); //--> Send the request
  payload = http.getString();     //--> Get the response payload
  Serial.println("---------------");
  Serial.println("updatedata");  
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
  // http.begin("http://dannaviaggi.altervista.org/getdata.php");  //--> Specify request destination
  http.begin("http://hp-i3-ok/tappa/getdata.php");  //--> Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
  httpCode = http.POST(postData); //--> Send the request
  payload = http.getString();     //--> Get the response payload
  Serial.println("---------------");
  Serial.println("getdata");
  Serial.print("httpCode : ");
  Serial.println(httpCode); //--> Print HTTP return code
  Serial.print("payload  : ");
  Serial.println(payload);  //--> Print request response payload
  http.end();  //--> Close connection

  JsonDocument doc; 
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
    Serial.print("status ");
    Serial.println(status);
    Serial.print("tempo ");
    Serial.println(tempo);
    Serial.print("delta ");
    Serial.println(delta);
  }
  else connect();
}