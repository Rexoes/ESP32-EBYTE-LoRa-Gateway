#include "LoRa_E22.h"
#include <HardwareSerial.h>
#define M0 32 //3in1 PCB mizde pin 7
#define M1 33 //3in1 PCB mizde pin 6
#define RX 27 //3in1 PCB mizde pin RX
#define TX 35  //3in1 PCB mizde pin TX
 
HardwareSerial fixajSerial(1);                            //Serial biri seçiyoruz.
LoRa_E22 e22(TX, RX, &fixajSerial, UART_BPS_RATE_9600);
 
struct Signal {
  char type[80] ;
  byte temp[4];
} data;
 
void setup() {
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  digitalWrite(M0, LOW);
  digitalWrite(M1, 0);
 
  Serial.begin(9600);
  delay(500);
  e22.begin();
  delay(500);
 
  char* source = "ESP32 başlat";
  strcpy(data.type, source);
 
  ResponseStatus rs = e22.sendFixedMessage(0, 2, 18, &data, sizeof(Signal));
  Serial.println(rs.getResponseDescription());
 
}
 
void loop() {
  while (e22.available()  > 1) {
    // Gelen mesaj okunuyor
    ResponseStructContainer rsc = e22.receiveMessage(sizeof(Signal));
    struct Signal data = *(Signal*) rsc.data;
    Serial.print("Yer: ");
    Serial.println(data.type);
    rsc.close();
 
    char* source = "ESP32 Selam";
    strcpy(data.type, source);
 
    ResponseStatus rs = e22.sendFixedMessage(0, 2, 18, &data, sizeof(Signal));
    Serial.println(rs.getResponseDescription());
  }
}
