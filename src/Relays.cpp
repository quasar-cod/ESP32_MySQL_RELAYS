#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <ESPmDNS.h>
#define ADDR  "esp32_relays" 
WiFiServer server(80);

#define ON_Board_LED 2
//valori per scheda relè esterna
//NB ha la logica invertita
// #define RELE_01 26 
// #define RELE_02 27 
//valori per scheda relè a 220V
#define RELE_01 16 
#define RELE_02 17 

const char* ssid = "TIM-39751438_EXT";
const char* password = "EFuPktKzk6utU2y5a5SEkUUQ";
String postData = ""; //--> Variables sent for HTTP POST request data.
String payload = "";  //--> Variable for receiving response from HTTP POST.

void setup() {
  Serial.begin(115200); //--> Initialize serial communications with the PC.
  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output.
  pinMode(RELE_01,OUTPUT); //--> RELE_01 port Direction output.
  pinMode(RELE_02,OUTPUT); //--> RELE_02 port Direction output.
  digitalWrite(RELE_01, LOW); //--> Turn off Led RELE_01.
  digitalWrite(RELE_02, LOW); //--> Turn off Led RELE_02.
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println("-------------");
  Serial.println("Connecting");
  int connecting_process_timed_out = 20; // 10 seconds.
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(ON_Board_LED, HIGH);
    Serial.println("Connecting");
    delay(250);
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    //Countdown "connecting_process_timed_out".
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
  }
  // If successfully connected to the wifi router, the IP Address that will be visited is displayed in the serial monitor
  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("-------------");
  Serial.println("Abilito dns");
  if (MDNS.begin(ADDR)) {
    Serial.println("Abilitato");
  }
  delay(1000);
  if(WiFi.status()== WL_CONNECTED) {
    Serial.println("Connesso");
  }
  server.begin();
  while(WiFi.status()== WL_CONNECTED) {
    delay(1000);
    HTTPClient http;  //--> Declare object of class HTTPClient.
    int httpCode;     //--> Variables for HTTP return code.
    postData = "board=ESP32_01";
    payload = "";
    http.begin("http://hp-i3/tappa/getdata.php");  //--> Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
    httpCode = http.POST(postData); //--> Send the request
    payload = http.getString();     //--> Get the response payload
    http.end();  //--> Close connection
    Serial.println("---------------");
    Serial.println("getdata");  
    Serial.print("httpCode : ");
    Serial.println(httpCode); //--> Print HTTP return code
    Serial.print("payload  : ");
    Serial.println(payload);  //--> Print request response payload
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
}

void loop() {
  if(WiFi.status()== WL_CONNECTED) {
    HTTPClient http;  //--> Declare object of class HTTPClient.
    int httpCode;     //--> Variables for HTTP return code.
    postData = "board=ESP32_01";
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
    }else
      {Serial.println("Parsing input failed!");
      delay(1000);
      // ESP.restart();
      }
    // digitalWrite(ON_Board_LED, LOW);
    delay(1000);
  }
}