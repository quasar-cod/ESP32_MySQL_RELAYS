#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <ESPmDNS.h>
#define ADDR  "esp32_relays" 
WiFiServer server(80);

#define ON_Board_LED 2 
#define LED_01 26 
#define LED_02 27 

const char* ssid = "TIM-39751438_EXT";
const char* password = "EFuPktKzk6utU2y5a5SEkUUQ";
String postData = ""; //--> Variables sent for HTTP POST request data.
String payload = "";  //--> Variable for receiving response from HTTP POST.

void setup() {
  Serial.begin(115200); //--> Initialize serial communications with the PC.
  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output.
  pinMode(LED_01,OUTPUT); //--> LED_01 port Direction output.
  pinMode(LED_02,OUTPUT); //--> LED_02 port Direction output.
  digitalWrite(LED_01, LOW); //--> Turn off Led LED_01.
  digitalWrite(LED_02, LOW); //--> Turn off Led LED_02.
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println("-------------");
  Serial.print("Connecting");
  int connecting_process_timed_out = 20; // 10 seconds.
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(ON_Board_LED, HIGH);
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
  server.begin();
}

void loop() {
  if(WiFi.status()== WL_CONNECTED) {
    HTTPClient http;  //--> Declare object of class HTTPClient.
    int httpCode;     //--> Variables for HTTP return code.
    postData = "board=ESP32_01";
    payload = "";
    digitalWrite(ON_Board_LED, HIGH);
    // http.begin("http://dannaviaggi.altervista.org/getdata.php");  //--> Specify request destination
    http.begin("http://portatile-hp-i3/esp32_relays/getdata.php");  //--> Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
    httpCode = http.POST(postData); //--> Send the request
    payload = http.getString();     //--> Get the response payload
    Serial.println("getdata");  
    // Serial.print("httpCode : ");
    // Serial.println(httpCode); //--> Print HTTP return code
    // Serial.print("payload  : ");
    // Serial.println(payload);  //--> Print request response payload
    http.end();  //--> Close connection
    JSONVar myObject = JSON.parse(payload);
  // JSON.typeof(jsonVar) can be used to get the type of the var
    if (JSON.typeof(myObject) != "undefined") {
      if(strcmp(myObject["LED_01"], "ON") == 0 && strcmp(myObject["LED_02"], "ON") == 0 )
        {digitalWrite(LED_01, LOW); 
        digitalWrite(LED_02, LOW); 
        Serial.println("XXX LED 01 ON LED 02 OFF");}
      if(strcmp(myObject["LED_01"], "ON") == 0 && strcmp(myObject["LED_02"], "OFF") == 0 )
        {digitalWrite(LED_02, LOW);//prima spengo nel caso sia nello stato opposto
        delay(10);
        digitalWrite(LED_01, HIGH); //e poi accendo
        Serial.println("111 LED 01 ON LED 02 OFF");}
      if(strcmp(myObject["LED_01"], "OFF") == 0 && strcmp(myObject["LED_02"], "ON") == 0)   
        {digitalWrite(LED_01, LOW);//prima spengo nel caso sia nello stato opposto
        delay(10);
        digitalWrite(LED_02, HIGH); //e poi accendo
        Serial.println("222 LED 01 OFF LED 02 ON");}
      if(strcmp(myObject["LED_01"], "OFF") == 0 && strcmp(myObject["LED_02"], "OFF") == 0)   
        {digitalWrite(LED_01, LOW);
        digitalWrite(LED_02, LOW); 
        Serial.println("333 LED 01 OFF LED 02 OFF");}
      // Serial.println("---------------");
    }else
      {Serial.println("Parsing input failed!");
      delay(1000);
      ESP.restart();
      }
    digitalWrite(ON_Board_LED, LOW);
    delay(200);
  }
}