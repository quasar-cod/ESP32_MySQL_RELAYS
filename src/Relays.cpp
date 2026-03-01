#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJSON.h>
#include <ESPmDNS.h>
#define ADDR "tappa" 

//valori per scheda relè a 220V
#define LED 23
#define RELE_01 16
#define RELE_02 17

// const char* ssid = "TIM-24326654";// soggiorno
const char* ssid = "TIM-24326654_EXT";// notte
// const char* ssid = "TIM-24326654_TENDA";//tavernetta
const char* password = "T9ZDHXACUfdTUC33DcTCASsz";
const char* site = "http://myhomesmart.altervista.org/";
//const char* site = "http://hp-i3/tappa/";

int i1 = 1;//1 soggiorno 2 giardino 3 cucinino 4 notte 5 tavernetta 6 basculante
String board;
String payload;
String postData;
HTTPClient http;
int httpCode;
int connecting_process_timed_out;
const char* activity = nullptr;
const char* gssid = nullptr; 
const char* gsite = nullptr; 
JsonDocument doc; 
char destination[255];

int dutyc;
int tempo;
int delta;
String status;
bool ko;
int pt;
#include <time.h>
#define MY_NTP_SERVER "it.pool.ntp.org"           
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"   

const char* time_on;
const char* time_off;
const char* S_time_on="06:30:00";
const char* S_time_off="23:30:00";

// Compact time helpers (minimized lines)
time_t now;
tm tmn;
static void upd(){time(&now); localtime_r(&now,&tmn); }
static String p(int v){ String s = "00" + String(v); return s.substring(s.length()-2); }
// String dateYMD(){ upd(); return String(tmn.tm_year+1900) + ":" + p(tmn.tm_mon+1) + ":" + p(tmn.tm_mday); }
// String timeHMS(){ upd(); return p(tmn.tm_hour) + ":" + p(tmn.tm_min) + ":" + p(tmn.tm_sec); }
String timeHM(){ upd(); return p(tmn.tm_hour) + ":" + p(tmn.tm_min) + ":00"; }
String timeM(){ upd(); return p(tmn.tm_min); }
// String timeS(){ upd(); return p(tmn.tm_sec); }

void update_activity(String actv){
  payload = "";
  postData = "board=";
  postData += board;
  postData +="&activity="+actv;
  Serial.println("---------------");
  Serial.println("update_activity");  
  Serial.println(postData);
  strcpy(destination ,site);
  strcat(destination ,"update_activity.php");
  Serial.println(destination);
  http.begin(destination);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
  httpCode = http.POST(postData);
  payload = http.getString();
  Serial.print("httpCode : ");
  Serial.println(httpCode);
  Serial.print("payload : ");
  Serial.println(payload);
  http.end();
}

void relays(){
  payload = "";
  postData = "board=";
  postData += board;
  Serial.println("---------------");
  Serial.println("get_activity");
  Serial.println(postData);
  strcpy(destination ,site);
  strcat(destination ,"get_activity.php");
  Serial.println(destination);
  http.begin(destination);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  httpCode = http.POST(postData);
  payload = http.getString();
  Serial.print("httpCode: ");
  Serial.println(httpCode);
  Serial.print("payload: ");
  Serial.println(payload);
  http.end();
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    ko=true;
    Serial.print(F("Failed to parse JSON: "));
    Serial.println(error.f_str());
    time_on="06:30:00";
    time_off="23:30:00";
    dutyc = 10000;
  }
  else {
    ko=false;
    activity = doc["activity"];
    time_on = doc["time_on"];
    time_off = doc["time_off"];
    dutyc = doc["dutyc"];
    // Serial.print("activity ");
    // Serial.print(activity);
    // Serial.print(" time_on ");
    // Serial.print(time_on);
    // Serial.print(" time_off ");
    // Serial.print(time_off);
    // Serial.print(" duty cycle ");
    // Serial.println(dutyc);
  }
  String orario=timeHM();
  Serial.print("orario ");
  Serial.println(orario);
  Serial.println("CKP: APERTURA da schedulazione");
  if ((orario == time_on && status!="UP" && ko==false)||
    (orario == S_time_on && ko==true)) {//se non c'è connessione
    digitalWrite(RELE_02, LOW);
    delay(100);//aspetto per evitare inerzia motore in caso di inversione
    digitalWrite(RELE_01, HIGH); 
    Serial.println("***********************************************");
    Serial.println("APERTURA da schedulazione");
    Serial.println("***********************************************");
    delay (dutyc);
    digitalWrite(RELE_01, LOW); 
    delay (61000-dutyc);//aspetto un po' più di un minuto per evitare nuve attività schedulate allo stesso minuto
  }
  Serial.println("CKP: CHIUSURA da schedulazione");
  if ((orario == time_off && status!="DOWN" && ko==false)||
    (orario == S_time_off && ko==true)) {
    digitalWrite(RELE_01, LOW);
    delay(100);//aspetto per evitare inerzia motore in caso di inversione
    digitalWrite(RELE_02, HIGH); 
    Serial.println("***********************************************");
    Serial.println("CHIUSURA da schedulazione");
    Serial.println("***********************************************");
    delay (dutyc);
    digitalWrite(RELE_02, LOW); 
    delay (61000-dutyc);
  }
  Serial.println("CKP: STOP da comando");
  if(strcmp(activity, "OFF") == 0 && status!="OFF"){
    digitalWrite(RELE_01, LOW); 
    digitalWrite(RELE_02, LOW); 
    status="OFF";
    Serial.println("***********************************************");
    Serial.println("STOP da comando");
    Serial.println("***********************************************");
  }
  Serial.println("CKP: APERTURA da comando");
  if(strcmp(activity, "UP") == 0 && status!="UP"){
    digitalWrite(RELE_02, LOW);
    delay(100);//aspetto per evitare inerzia motore in caso di inversione
    digitalWrite(RELE_01, HIGH); 
    status="UP";
    tempo=millis();
    delta=0;
    Serial.println("***********************************************");
    Serial.println("APERTURA da comando");
    Serial.println("***********************************************");
  }
  Serial.println("CKP: CHIUSURA da comando");
  if(strcmp(activity, "DOWN") == 0 && status!="DOWN"){
    digitalWrite(RELE_01, LOW);
    delay(100);//aspetto per evitare inerzia motore in caso di inversione
    digitalWrite(RELE_02, HIGH); 
    status="DOWN";
    tempo=millis();
    delta=0;
    Serial.println("***********************************************");
    Serial.println("CHIUSURA da comando");
    Serial.println("***********************************************");
  }
  delta=millis()-tempo;
  Serial.println("CKP: STOP da TIMEOUT");
  if(delta > dutyc  && status!="OFF"){
    digitalWrite(RELE_01, LOW); 
    digitalWrite(RELE_02, LOW); 
    status="OFF";
    Serial.println("***********************************************");
    Serial.println("STOP da TIMEOUT");
    Serial.println("***********************************************");
    update_activity("OFF");
  }
  Serial.print("status: ");
  Serial.println(status);
  Serial.print("tempo: ");
  Serial.println(tempo);
  Serial.print("delta: ");
  Serial.println(delta);
}

void connect(){
  int r;
  for (r=1;r<10;r++){
    connecting_process_timed_out = 35;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid,password);
    Serial.println("***********************************************");
    Serial.println("Connecting to ");
    Serial.println(ssid);
    Serial.println("***********************************************");
    while (WiFi.status() != WL_CONNECTED & (connecting_process_timed_out > 0)){
      Serial.print(".");//3 flash 1.7 secondi
      digitalWrite(LED,HIGH);
      delay(100);
      digitalWrite(LED,LOW);
      delay(200);
      digitalWrite(LED,HIGH);
      delay(100);
      digitalWrite(LED,LOW);
      delay(200);
      digitalWrite(LED,HIGH);
      delay(100);
      digitalWrite(LED,LOW);
      delay(1000);
      connecting_process_timed_out--;
    }
    Serial.println("\n***********************************************");
    Serial.print("Successfully connected to ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    String MAC = WiFi.macAddress();
    Serial.print("MAC Address: ");
    Serial.println(MAC);
    Serial.println("Abilito dns");
    if (MDNS.begin(ADDR)){
      Serial.println("Abilitato");
      Serial.println("***********************************************");
      update_activity("CONN" + String(r));
      break;
    }
    delay(r*60000);//ad ogni tentativo aumento il ritardo di un minuto
  }
  if (r==10) ESP.restart();
}

void tmz(){
  // --> Here is the IMPORTANT ONE LINER needed in your sketch!
  // configTime(MY_TZ, MY_NTP_SERVER); //sulle esp8266 basta questa sola riga e le define
  configTime(0,0, MY_NTP_SERVER); //sulle ESP32 occorre separare in tre righe 
  setenv("TZ","CET-1CEST,M3.5.0/02,M10.5.0/03" ,1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
  Serial.println("***********************************************");
  Serial.println("Connecting to NTP_SERVER");
  Serial.println("***********************************************");
  time_t now = time(nullptr);
  int ntpRetry = 0;
  // 1738713600 = Thursday, February 5, 2026 00:00:00
  while (now < 1738713600 && ntpRetry < 100) { 
    Serial.print(".");//2 flash
    now = time(nullptr); 
    ntpRetry++;
    delay(1000); // Small delay to let the UDP packet arrive
    digitalWrite(LED,LOW);
    delay(100);
    digitalWrite(LED,HIGH);
    delay(200);
    digitalWrite(LED,LOW);
    delay(100);
    digitalWrite(LED,HIGH);
  }
  if (now < 1738713600) {
    ESP.restart();
  }
}

void setup() {
  Serial.begin(230400);
  board="ESP32_0" + String(i1);
  Serial.println("\n***********************************************");
  Serial.print("Initialized serial communications with board ");
  Serial.println(board);
  Serial.println("***********************************************");
  pinMode(RELE_01,OUTPUT);
  pinMode(RELE_02,OUTPUT);
  pinMode(LED,OUTPUT);
  digitalWrite(RELE_01, HIGH);
  delay(300);
  digitalWrite(RELE_01, LOW);
  delay(200);
  digitalWrite(RELE_02, HIGH);
  delay(300);
  digitalWrite(RELE_02, LOW);
  connect();
  tmz();
  // update_activity("SETUP");
  status="OFF";
  tempo=0;
  delta=0;
  pt=0;
}

void loop() {
  // if(WiFi.status()== WL_CONNECTED) {//spesso genera falsi errori
  // sostituendo .waitForConnectResult a .status non si hanno falsi errori
  if(WiFi.waitForConnectResult()== WL_CONNECTED) {
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    delay(2000);
    relays(); // mettendo un delay dopo la waitForConnectResult la funzione get_activity fallisce con minore frequnza
    if(timeM()==("00") && status=="OFF") {
      if(pt==0){
        delay(i1*10000);
        update_activity("TIME");
        pt=1;
      }
    }
    else{ 
      pt=0;
    }
  }
  else{
    connect();
  }
}