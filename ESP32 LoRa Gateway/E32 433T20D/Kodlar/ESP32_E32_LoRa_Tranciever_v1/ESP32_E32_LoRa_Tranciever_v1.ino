#include "LoRa_E32.h"
#include <HardwareSerial.h>
#define M0 32 //3in1 PCB mizde pin 7
#define M1 33 //3in1 PCB mizde pin 6
#define RX 27 //3in1 PCB mizde pin RX
#define TX 35  //3in1 PCB mizde pin TX

HardwareSerial fixajSerial(1);                            //Serial biri seçiyoruz.
LoRa_E32 e32(TX, RX, &fixajSerial, UART_BPS_RATE_9600);

struct Signal {
  char type[15] = "SCU BIL";
  byte temp[4];
} data;

void setup() {
//  pinMode(M0, OUTPUT);
//  pinMode(M1, OUTPUT);
//  digitalWrite(M0, LOW); // E22 Modülleri için Aktif edin
//  digitalWrite(M1, 0);   // E32 Modülleri için Aktif edin
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  digitalWrite(M0, LOW);
  digitalWrite(M1, 0);
  Serial.begin(9600);
  delay(500);
  e32.begin();
  delay(500);
}

void loop() {
  ResponseStatus rs = e32.sendFixedMessage(0, 44, 23, &data, sizeof(Signal));
  Serial.println(rs.getResponseDescription());
  delay(2000);
}
