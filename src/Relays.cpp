#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <ESPmDNS.h>
#define ADDR  "esp32_relays" 
WiFiServer server(80);

//valori per scheda relè esterna
// #define ON_Board_LED 2
//NB il relè ha la logica invertita
// #define RELE_01 26 
// #define RELE_02 27 

//valori per scheda relè a 220V
#define ON_Board_LED 23
#define RELE_01 16 
#define RELE_02 17 

const char* ssid1 = "TIM-39751438";
const char* ssid2 = "TIM-39751438_EXT";
const char* password = "EFuPktKzk6utU2y5a5SEkUUQ";
String payload = "";  //--> Variable for receiving response from HTTP POST.
String postData = "board=ESP32_02"; //--> Variables sent for HTTP POST request data.

void setup() {
  Serial.begin(115200); //--> Initialize serial communications with the PC.
  Serial.println("\n-------------");
  Serial.println("Initialized serial communications with the PC.");
  // pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output.
  pinMode(RELE_01,OUTPUT); //--> RELE_01 port Direction output.
  pinMode(RELE_02,OUTPUT); //--> RELE_02 port Direction output.
  pinMode(ON_Board_LED,OUTPUT); //--> RELE_02 port Direction output.
  digitalWrite(RELE_01, HIGH); //--> Turn off Led RELE_01.
  digitalWrite(RELE_02, HIGH); //--> Turn off Led RELE_02.
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off Led RELE_02.
  delay(250);
  digitalWrite(RELE_01, LOW); //--> Turn off Led RELE_01.
  digitalWrite(RELE_02, LOW); //--> Turn off Led RELE_02.
  digitalWrite(ON_Board_LED, LOW); //--> Turn off Led RELE_02.
  delay(250);
  digitalWrite(RELE_01, HIGH); //--> Turn off Led RELE_01.
  digitalWrite(RELE_02, HIGH); //--> Turn off Led RELE_02.
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off Led RELE_02.
  delay(250);
  digitalWrite(RELE_01, LOW); //--> Turn off Led RELE_01.
  digitalWrite(RELE_02, LOW); //--> Turn off Led RELE_02.
  digitalWrite(ON_Board_LED, LOW); //--> Turn off Led RELE_02.
  delay(250);
  
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid1, password);
  int connecting_process_timed_out = 20; // 10 seconds.
  Serial.print("Connecting to ");
  Serial.println(ssid1);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) {
      WiFi.begin(ssid2, password);
      connecting_process_timed_out = 20; // 10 seconds.
      Serial.print("Connecting to ");
      Serial.println(ssid2);
      while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
        if(connecting_process_timed_out > 0) connecting_process_timed_out--;
        if(connecting_process_timed_out == 0) {
          delay(1000);
          ESP.restart();
        }
      }
      break;
    }
  }
  Serial.print("Successfully connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("-------------");
  Serial.println("Abilito dns");
  if (MDNS.begin(ADDR)) {
    Serial.println("Abilitato");
  }
//////////////////////////////////////////////////////////////////////
  while(WiFi.status()== WL_CONNECTED) {
    HTTPClient http;  //--> Declare object of class HTTPClient.
    int httpCode;     //--> Variables for HTTP return code.
    payload = "";
    // digitalWrite(ON_Board_LED, HIGH);
    // http.begin("http://dannaviaggi.altervista.org/getdata.php");  //--> Specify request destination
    http.begin("http://hp-i3/tappa/getdata.php");  //--> Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
    httpCode = http.POST(postData); //--> Send the request
    payload = http.getString();     //--> Get the response payload
    // Serial.print("---------------");
    Serial.println("getdata");  
    // Serial.print("httpCode : ");
    // Serial.println(httpCode); //--> Print HTTP return code
    // Serial.print("payload  : ");
    // Serial.println(payload);  //--> Print request response payload
    http.end();  //--> Close connection
    JSONVar myObject = JSON.parse(payload);
    if (JSON.typeof(myObject) != "undefined") {
      if(strcmp(myObject["activity"], "OFF") == 0 )
        {digitalWrite(RELE_01, LOW); 
        digitalWrite(RELE_02, LOW); 
        Serial.println("SPEGNIMENTO");
      }
      if(strcmp(myObject["activity"], "UP") == 0 )
        {
        digitalWrite(RELE_02, LOW);
        delay(100);//aspetto per evitare inerzia motore in caso di inversione
        digitalWrite(RELE_01, HIGH); 
        Serial.println("APERTURA");
      }
      if(strcmp(myObject["activity"], "DOWN") == 0 )
        {
        digitalWrite(RELE_01, LOW);
        delay(100);//aspetto per evitare inerzia motore in caso di inversione
        digitalWrite(RELE_02, HIGH); 
        Serial.println("CHIUSURA");
        }
    }
  }
  Serial.println("Parsing input failed!");
  delay(1000);
  ESP.restart();
////////////////////////////////////////////////////////////////////////
}

void loop() {
  if(WiFi.status()== WL_CONNECTED) {
    HTTPClient http;  //--> Declare object of class HTTPClient.
    int httpCode;     //--> Variables for HTTP return code.

    payload = "";
    // digitalWrite(ON_Board_LED, HIGH);
    // http.begin("http://dannaviaggi.altervista.org/getdata.php");  //--> Specify request destination
    http.begin("http://hp-i3/tappa/getdata.php");  //--> Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
    httpCode = http.POST(postData); //--> Send the request
    payload = http.getString();     //--> Get the response payload
    // Serial.println("---------------");
    // Serial.println("getdata");  
    // Serial.print("httpCode : ");
    // Serial.println(httpCode); //--> Print HTTP return code
    // Serial.print("payload  : ");
    // Serial.println(payload);  //--> Print request response payload
    http.end();  //--> Close connection
    JSONVar myObject = JSON.parse(payload);
    if (JSON.typeof(myObject) != "undefined") {
      if(strcmp(myObject["activity"], "OFF") == 0 )
        {digitalWrite(RELE_01, LOW); 
        digitalWrite(RELE_02, LOW); 
        Serial.println("SPEGNIMENTO");
      }
      if(strcmp(myObject["activity"], "UP") == 0 )
        {
        digitalWrite(RELE_02, LOW);
        delay(100);//aspetto per evitare inerzia motore in caso di inversione
        digitalWrite(RELE_01, HIGH); 
        Serial.println("APERTURA");
      }
      if(strcmp(myObject["activity"], "DOWN") == 0 )
        {
        digitalWrite(RELE_01, LOW);
        delay(100);//aspetto per evitare inerzia motore in caso di inversione
        digitalWrite(RELE_02, HIGH); 
        Serial.println("CHIUSURA");
        }
      }
    else {
      Serial.println("Parsing input failed!");
      delay(1000);
      ESP.restart();
      }
    // digitalWrite(ON_Board_LED, LOW);
    delay(1000);
  }
}