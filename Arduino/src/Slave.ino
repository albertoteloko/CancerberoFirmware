#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <stdlib.h> // for the atol() function
#include "Alarm.h"

const boolean debug = false;

byte nodeStatus = IDDLE_STATE;
long statusTime = 0;

byte mustReproduce = NULL_MELODY;
int sensors[] = {0, 0};

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 1, 70 };
EthernetServer server(5555);

void setup() {    
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Domo4Pi Slave v1.0");   

  pinMode(ALARM_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);

  for (int i=2; i < 6; i++){
    pinMode(i, INPUT);
  }

  if(digitalRead(ADDRESS_SELECT_PIN)){
    mac[5] = 0xEE;
    ip[3] = 71;
  }

  printMAC();
  printIP();
  
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.println("Domo4Pi Slave, started");
  printAlarmState();
}

void loop(){ 
  readSensors();
  checkCommand();
  checkMelody();

  statusTime += 10;
  delay(10); 
}

void readSensors() {
  sensors[0] |= getAnalogDigitalInput(0) ? 1 : 0;
  sensors[0] |= getAnalogDigitalInput(1) ? 2 : 0;
  sensors[0] |= getAnalogDigitalInput(2) ? 4 : 0;
  sensors[0] |= getAnalogDigitalInput(3) ? 8 : 0;
  sensors[0] |= getAnalogDigitalInput(4) ? 16 : 0;
  sensors[0] |= getAnalogDigitalInput(5) ? 32 : 0;
  
  sensors[1] |= digitalRead(2) ? 0 : 1;
  sensors[1] |= digitalRead(3) ? 0 : 2;
  sensors[1] |= digitalRead(4) ? 0 : 4;
  sensors[1] |= digitalRead(5) ? 0 : 8;
}

void checkCommand() {
  EthernetClient client = server.available();
  
  int i = 0;
  if (client) {
    Serial.println("Master connected");
    
    byte command = client.read();
    if (command == GET_STATUS_COMMAND){
      Serial.println("Status requested");
      sendStatus(client);
    } else if (command == SET_STATUS_COMMAND){
       byte newStatus;
      
      while(((newStatus = client.read()) == 255) && (i++ < 200)){
        delay(10);
      }
      
      setStatus(newStatus);
      sendStatus(client);
      printAlarmState();
    }
       
    delay(5);
    client.stop();
    Serial.println("Master disonnected");
  }
}

void checkMelody() {
  if(mustReproduce == ACTIVATED_STATE){
    buzzActivate();
  }else if(mustReproduce == IDDLE_STATE){
    buzzDeactivate();
  } else if (nodeStatus == ACTIVATING_STATE) {
    loopActivatingState();
  }else if(nodeStatus == SUSPICIUS_STATE) {
    loopSuspiciusState();
  }
}

void sendStatus(EthernetClient client){
  if(debug){
    Serial.print("Sensors: ");
    Serial.print(sensors[0]);
    Serial.print(", ");
    Serial.print(sensors[1]);
    Serial.println();
  }
  
  byte result[] = {nodeStatus, sensors[0], sensors[1]};
  
  client.write(result, 3);
  
  sensors[0] = 0;
  sensors[1] = 0;
}


void loopSuspiciusState() {
  int cicleTime  = statusTime % 1000;
  delay(1);
  if(cicleTime == 0){
    tone(SPEAKER_PIN, 1500);
    digitalWrite(LED_PIN, HIGH);
  } else if(cicleTime==300){
    noTone(SPEAKER_PIN);
    digitalWrite(LED_PIN, LOW);
  }
}

void loopActivatingState() {
  int cicleTime  = statusTime % 2000;
  if(cicleTime == 0){
    tone(SPEAKER_PIN, 400);
    digitalWrite(LED_PIN, HIGH);
  } else if(cicleTime==200){
    noTone(SPEAKER_PIN);
    digitalWrite(LED_PIN, LOW);
  }
}

void buzzActivate() {
  if(statusTime == 600){
    noTone(SPEAKER_PIN);
    mustReproduce = NULL_MELODY;
  } else if((statusTime % 200) == 0){
    tone(SPEAKER_PIN, ((statusTime / 100) + 1) * 400);
  }
}

void buzzDeactivate() {
  if(statusTime == 600){
    noTone(SPEAKER_PIN);
    mustReproduce = NULL_MELODY;
  } else if((statusTime % 200) == 0){
    tone(SPEAKER_PIN, (8 - (statusTime / 100)) * 400);
  }
}

void setStatus(byte state){
  if ((isValidState(state))&&(nodeStatus != state)){
    statusTime = 0;
    noTone(SPEAKER_PIN);
    nodeStatus = state;

    Serial.print("Changing alarm state: ");
    Serial.println(getStateString()); 

    digitalWrite(LED_PIN, (state == IDDLE_STATE) ? LOW : HIGH);
    digitalWrite(ALARM_PIN, ((state == ALARMED_STATE) || (state == SAFETY_STATE)) ? HIGH : LOW);

    if(state == IDDLE_STATE){
      mustReproduce = IDDLE_STATE;
    } else if(state == ACTIVATED_STATE){
      mustReproduce = ACTIVATED_STATE;
    } else if(state == ALARMED_STATE){
      tone(SPEAKER_PIN, 3000);
    } else if(state == SAFETY_STATE){
      tone(SPEAKER_PIN, 2000);
    }
  } else {
    Serial.print("Unable to change the state to unknown value: ");
    Serial.println(state); 
  }
}

boolean isValidState(byte state) {
  return (state == IDDLE_STATE) || (state == ACTIVATING_STATE) || (state == ACTIVATED_STATE) || (state == SUSPICIUS_STATE)|| (state == ALARMED_STATE) || (state == SAFETY_STATE);
}

boolean getAnalogDigitalInput(int pin){
  return (analogRead(pin) > 1000);
}

void printAlarmState(){
  Serial.print("Alarm state: ");
  Serial.println(getStateString());  
}

String getStateString(){
  return getStateString(nodeStatus);
}

String getStateString(byte state){
  String stateString = "Unknown";

  if(state == IDDLE_STATE){
    stateString = "Iddle";
  }
  else if(state == ACTIVATING_STATE){
    stateString = "Activating";
  }
  else if(state == ACTIVATED_STATE){
    stateString = "Actived";
  }
  else if(state == SUSPICIUS_STATE){
    stateString = "Suspicius";
  }
  else if(state == ALARMED_STATE){
    stateString = "Suspicius";
  }
  else if(state == SAFETY_STATE){
    stateString = "Safety";
  }

  return stateString;
}

void printMAC(){
  Serial.print("MAC: ");
  for (int i=0; i <= 5; i++){
    if(i!=0){
      Serial.print(":");
    }
    Serial.print(mac[i], HEX);
  }
  Serial.println();
}

void printIP(){
  Serial.print("IP:  ");
  for (int i=0; i <= 3; i++){
    if(i!=0){
      Serial.print(".");
    }
    Serial.print(ip[i]);
  }
  Serial.println();
}






