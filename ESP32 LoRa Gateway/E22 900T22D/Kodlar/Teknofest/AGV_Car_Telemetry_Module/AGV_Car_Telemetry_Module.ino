#include "LoRa_E22.h"
#include <HardwareSerial.h>
#define M0 32 //3in1 PCB mizde pin 7
#define M1 33 //3in1 PCB mizde pin 6
#define RX 27 //3in1 PCB mizde pin RX
#define TX 35  //3in1 PCB mizde pin TX
 
HardwareSerial fixajSerial(1);                            //Serial biri seçiyoruz.
LoRa_E22 e22(TX, RX, &fixajSerial, UART_BPS_RATE_9600);

typedef struct{
  byte batteryLevel;
  float loadCell;
  byte leftMotorRpm;
  byte rightMotorRpm;
  float vehicleSpeed;
  float batteryCurrent;
  float leftMotorCurrent;
  float rightMotorCurrent;
  float forwardSensorDistance;
  float leftFrontCrossSensorDistance;
  float leftSensorDistance;
  float leftRearCrossSensorDistance;
  float ecuTemperature;
  boolean fanState;
  boolean carLights;
}AGV_Sensors; //Total 45 byte data.

AGV_Sensors sensor ={0,15.4,0,0,3.2,0.0,0.0,0.0,0.0,1.2,3.4,5.6,2.1,0,0};

unsigned long currentTimer = 0;
byte sampleTime = 3;

void setup() {
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  digitalWrite(M0, LOW);
  digitalWrite(M1, 0);
 
  Serial.begin(9600);
  delay(500);
  e22.begin();
  delay(500);
}
 
void loop() {
  if((millis() - currentTimer) > (sampleTime * 1000)){
    ResponseStatus rs = e22.sendFixedMessage(0, 2, 18, &sensor, sizeof(AGV_Sensors));
    Serial.println(rs.getResponseDescription());
    currentTimer = millis();
  }
}

//void LoRa_Listen(){
//  while (e22.available()  > 1) {
//    // Gelen mesaj okunuyor
//    ResponseStructContainer rsc = e22.receiveMessage(sizeof(Signal));
//    struct Signal data = *(Signal*) rsc.data;
//    Serial.print("Yer: ");
//    Serial.println(data.type);
//    rsc.close();
// 
//    char* source = "ESP32 Selam";
//    strcpy(data.type, source);
// 
//    ResponseStatus rs = e22.sendFixedMessage(0, 2, 18, &data, sizeof(Signal));
//    Serial.println(rs.getResponseDescription());
//  }
//}
