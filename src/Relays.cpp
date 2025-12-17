#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJSON.h>
#include <ESPmDNS.h>
#define ADDR "tappa" 
//valori led blu per scheda dev
// #define ON_Board_LED 2
// valori per scheda relè esterna
// #define RELE_01 26
// #define RELE_02 27
//NB il relè ha la logica invertita

//valori per scheda relè a 220V
#define ON_Board_LED 23
#define RELE_01 16
#define RELE_02 17

int i1 = 3;//1 soggiorno 2 tavernetta 3 notte
// const char* ssid = "TIM-39751438";//1 soggiorno
// const char* ssid = "TIM-39751438_TENDA";//2 tavernetta
const char* ssid = "TIM-39751438_EXT";// 3 notte

const char* password = "EFuPktKzk6utU2y5a5SEkUUQ";
const char* site = "http://myhomesmart.altervista.org/";
//const char* site = "http://hp-i3/tappa/";

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

int tempo;
int delta;
String status;
bool ko;
int pt;
#include <time.h>
#define MY_NTP_SERVER "it.pool.ntp.org"           
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"   
time_t now;
tm tmn;
const char* time_on;
const char* time_off;
const char* S_time_on="06:30:00";
const char* S_time_off="23:30:00";

String dateYMD(){
  time(&now);
  localtime_r(&now, &tmn);
  String a = String(tmn.tm_year + 1900);
  String m = "00" + String(tmn.tm_mon);
  String mm = m.substring(m.length()-2);
  String g = "00" + String(tmn.tm_mday);
  String gg = g.substring(g.length()-2);
  String amg = a + ":" + mm + ":" + gg;
  return(amg);
}
String timeHMS(){
  time(&now);
  localtime_r(&now, &tmn);
  String h = "00" + String(tmn.tm_hour);
  String hh = h.substring(h.length()-2);
  String m = "00" + String(tmn.tm_min);
  String mm = m.substring(m.length()-2);
  String s = "00" + String(tmn.tm_sec);
  String ss = s.substring(s.length()-2);
  String hms = hh + ":" + mm + ":" + ss;
  return(hms);
}
String timeHM(){
  time(&now);
  localtime_r(&now, &tmn);
  String h = "00" + String(tmn.tm_hour);
  String hh = h.substring(h.length()-2);
  String m = "00" + String(tmn.tm_min);
  String mm = m.substring(m.length()-2);
  String hms = hh + ":" + mm + ":00";
  return(hms);
}
String timeM(){
  time(&now);
  localtime_r(&now, &tmn);
  String m = "00" + String(tmn.tm_min);
  String mm = m.substring(m.length()-2);
  return(mm);
}
String timeS(){
  time(&now);
  localtime_r(&now, &tmn);
  String s = "00" + String(tmn.tm_sec);
  String ss = s.substring(s.length()-2);
  return(ss);
}

void updatedata(String actv){
  payload = "";
  postData = "board=";
  postData += board;
  postData +="&activity="+actv;
  Serial.println("---------------");
  Serial.println("updatedata");  
  Serial.println(postData);
  strcpy(destination ,site);
  strcat(destination ,"updatedata.php");
  Serial.println(destination);
  http.begin(destination);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
  httpCode = http.POST(postData);
  payload = http.getString();
  Serial.print("httpCode : ");
  Serial.println(httpCode);
  Serial.print("payload  : ");
  Serial.println(payload);
  http.end();
}

void getdata(){
  payload = "";
  postData = "board=";
  postData += board;
  Serial.println("---------------");
  Serial.println("getdata");
  Serial.println(postData);
  strcpy(destination ,site);
  strcat(destination ,"getdata.php");
  Serial.println(destination);
  http.begin(destination);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  httpCode = http.POST(postData);
  payload = http.getString();
  Serial.print("httpCode : ");
  Serial.println(httpCode);
  Serial.print("payload  : ");
  Serial.println(payload);
  http.end();
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    ko=true;
    Serial.print(F("Failed to parse JSON: "));
    Serial.println(error.f_str());
  }
  else {
    ko=false;
    activity = doc["activity"];
    time_on = doc["time_on"];
    time_off = doc["time_off"];
    Serial.print("activity ");
    Serial.print(activity);
    Serial.print("on ");
    Serial.print(time_on);
    Serial.print(" off ");
    Serial.println(time_off);
  }
}

void relays(){
  String orario=timeHM();
  Serial.print("orario ");
  Serial.println(orario);
  if ((orario == time_on && status!="UP" && ko==false)||
    (orario == S_time_on && ko==true)) {//&& è booleano mentre & è bitwise
    digitalWrite(RELE_02, LOW);
    delay(100);//aspetto per evitare inerzia motore in caso di inversione
    digitalWrite(RELE_01, HIGH); 
    Serial.println("***********************************************");
    Serial.println("APERTURA ");
    Serial.println("***********************************************");
    delay (30000);
    digitalWrite(RELE_01, LOW); 
    delay (40000);
  }
  if ((orario == time_off && status!="DOWN" && ko==false)||
    (orario == S_time_off && ko==true)) {
    digitalWrite(RELE_01, LOW);
    delay(100);//aspetto per evitare inerzia motore in caso di inversione
    digitalWrite(RELE_02, HIGH); 
    Serial.println("***********************************************");
    Serial.println("CHIUSURA ");
    Serial.println("***********************************************");
    delay (30000);
    digitalWrite(RELE_02, LOW); 
    delay (40000);
  }

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
  if(delta > 30000  && status!="OFF"){
    digitalWrite(RELE_01, LOW); 
    digitalWrite(RELE_02, LOW); 
    status="OFF";
    Serial.println("***********************************************");
    Serial.println("TIMEOUT");
    Serial.println("***********************************************");
    updatedata("OFF");
  }
  Serial.print("status ");
  Serial.println(status);
  Serial.print("tempo ");
  Serial.println(tempo);
  Serial.print("delta ");
  Serial.println(delta);
}

void config(){
  connecting_process_timed_out = 10;
  WiFi.mode(WIFI_STA);
  WiFi.begin("Redmi14c","manu1234");
//NB se uso il cellulare non posso partire da una pagina nella rete locale perchè non è esposta su internet 
  Serial.println("\n***********************************************");
  Serial.println("Connecting to MASTER");
  Serial.println("***********************************************");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");//4 flash 2 secondi
    digitalWrite(ON_Board_LED,HIGH);
    delay(100);
    digitalWrite(ON_Board_LED,LOW);
    delay(200);
    digitalWrite(ON_Board_LED,HIGH);
    delay(100);
    digitalWrite(ON_Board_LED,LOW);
    delay(200);
    digitalWrite(ON_Board_LED,HIGH);
    delay(100);
    digitalWrite(ON_Board_LED,LOW);
    delay(200);
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
    payload = "";
    postData = "board=";
    postData += board; 
    strcat(destination ,site);
    strcat(destination ,"getdata.php");
    Serial.println("---------------");
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
      Serial.print(F("Failed to parse JSON"));
      Serial.println(error.f_str());
    } else {
      gsite = doc["site"];
      Serial.print("gssid ");
      Serial.println(gsite);
      Serial.print("site ");
      Serial.println(site);
      if(strcmp(gsite, site) != 0 & strcmp(gsite, "") != 0){
        Serial.println("***********************************************");
        Serial.println("cambio sito");
        Serial.println("***********************************************");
        ssid=gssid;
      } else {
        Serial.println("***********************************************");
        Serial.println("sito CONFERMATO");
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
      } else {
        Serial.println("***********************************************");
        Serial.println("SSID CONFERMATO");
        Serial.println("***********************************************");
      }
    }
  }
  else{
    Serial.print(F("Failed to open DNS"));
  }
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
      digitalWrite(ON_Board_LED,HIGH);
      delay(100);
      digitalWrite(ON_Board_LED,LOW);
      delay(200);
      digitalWrite(ON_Board_LED,HIGH);
      delay(100);
      digitalWrite(ON_Board_LED,LOW);
      delay(200);
      digitalWrite(ON_Board_LED,HIGH);
      delay(100);
      digitalWrite(ON_Board_LED,LOW);
      delay(1000);
      connecting_process_timed_out--;
    }
    Serial.println("\n***********************************************");
    Serial.print("Successfully connected to ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("-------------");
    Serial.println("Abilito dns");
    if (MDNS.begin(ADDR)){
      Serial.println("-------------");
      Serial.println("Abilitato");
      Serial.println("***********************************************");
      updatedata("CONNECT " + String(r));
      break;
    }
    delay(r*60000);//ad ogni tentavio aumento il ritardo di un minuto
  }
  if (r==10) ESP.restart();
}

void tmz(){
  // --> Here is the IMPORTANT ONE LINER needed in your sketch!
  // configTime(MY_TZ, MY_NTP_SERVER); //sulle esp8266 basta questa sola riga e le define
  configTime(0,0, MY_NTP_SERVER); //sulle ESP32 occorre separare in tre righe 
  setenv("TZ","CET-1CEST,M3.5.0/02,M10.5.0/03" ,1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
  Serial.println("\n***********************************************");
  Serial.println("NTP TZ DST - wait 1 minute");
  Serial.println("***********************************************");
  for (int i=0;i<44;i++){
    Serial.print(".");//2 flash 1.4 secondi
    digitalWrite(ON_Board_LED,HIGH);
    delay(100);
    digitalWrite(ON_Board_LED,LOW);
    delay(200);
    digitalWrite(ON_Board_LED,HIGH);
    delay(100);
    digitalWrite(ON_Board_LED,LOW);
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200);
  board="ESP32_0" + String(i1);
  Serial.println("\n***********************************************");
  Serial.print("Initialized serial communications with board ");
  Serial.println(board);
  Serial.println("***********************************************");
  pinMode(RELE_01,OUTPUT);
  pinMode(RELE_02,OUTPUT);
  pinMode(ON_Board_LED,OUTPUT);
  digitalWrite(RELE_01, HIGH);
  delay(250);
  digitalWrite(RELE_01, LOW);
  delay(250);
  digitalWrite(RELE_02, HIGH);
  delay(250);
  digitalWrite(RELE_02, LOW);
  // config();
  connect();
  tmz();
  updatedata(board);
  status="OFF";
  tempo=0;
  delta=0;
  pt=0;
}

void loop() {
  // if(WiFi.status()== WL_CONNECTED) {//spesso genera falsi errori
  // sostituendo .waitForConnectResult a .status non si hanno falsi errori
  if(WiFi.waitForConnectResult()== WL_CONNECTED) {
    digitalWrite(ON_Board_LED,HIGH);
    delay(100);
    digitalWrite(ON_Board_LED,LOW);
    delay(2000);
    getdata(); // mettendo un delay dopo la waitForConnectResult la funzione getdata fallisce con minore frequnza
    relays();
    if(timeM()==("00") && status=="OFF") {
      if(pt==0){
        delay(i1*10000);
        updatedata("TIME");
        pt=1;
      }
    }
    else{ 
      pt=0;
    }
  }
  else{
    config();
    connect();
  }
}