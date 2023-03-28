#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "LoRa_E22.h"
#include <HardwareSerial.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "ElikRepeter";
const char* password = "057921236";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "192.168.0.108";

WiFiClient espClient;
PubSubClient client(espClient);

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
AGV_Sensors sensor;
//AGV_Sensors sensor ={0,15.4,0,0,3.2,0.0,0.0,0.0,0.0,1.2,3.4,5.6,2.1,0,0};

boolean transmit = false;

void LoRa_Listen(void);
 
void setup() {
  Serial.begin(9600);
  //WiFi_Connect();
  //client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
  //MQTT_Connect();
  
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  digitalWrite(M0, LOW);
  digitalWrite(M1, 0);
 
  delay(500);
  e22.begin();
  delay(500);
}
 
void loop() {
//  if (!client.connected()) {
//    MQTT_Connect();
//  }
  
  LoRa_Listen();
  
//  if(transmit == true){
//    MQTT_JSON_Send();
//    transmit = false;
//  }
//  client.loop();
}

void MQTT_JSON_Send(){
   StaticJsonBuffer<500> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
 
  JSONencoder["batteryLevel"] = sensor.batteryLevel;
  JSONencoder["loadCell"] = sensor.loadCell;
  JSONencoder["leftMotorRpm"] = sensor.leftMotorRpm;
  JSONencoder["rightMotorRpm"] = sensor.rightMotorRpm;
  JSONencoder["vehicleSpeed"] = sensor.vehicleSpeed;
  JSONencoder["batteryCurrent"] = sensor.batteryCurrent;
  JSONencoder["leftMotorCurrent"] = sensor.leftMotorCurrent;
  JSONencoder["rightMotorCurrent"] = sensor.rightMotorCurrent;
  //JSONencoder["forwardSensorDistance"] = sensor.forwardSensorDistance;
//  JSONencoder["leftFrontCrossSensorDistance"] = sensor.leftFrontCrossSensorDistance;
  //JSONencoder["leftSensorDistance"] = sensor.leftSensorDistance;
//  JSONencoder["leftRearCrossSensorDistance"] 
  JSONencoder["ecuTemperature"] = sensor.ecuTemperature;
  JSONencoder["fanState"] = sensor.fanState;
  JSONencoder["carLights"] = sensor.carLights;
  JsonArray& distances = JSONencoder.createNestedArray("Distances");
  distances.add(sensor.forwardSensorDistance);
  distances.add(sensor.leftFrontCrossSensorDistance);
  distances.add(sensor.leftSensorDistance);
  distances.add(sensor.leftRearCrossSensorDistance);
  Serial.println("Capacity JSONbuffer: " + String(sizeof(JSONbuffer.capacity())));
 
  char JSONmessageBuffer[400];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println("Sizeof JSONmessageBuffer: " + String(sizeof(JSONmessageBuffer)));
  Serial.println("Sizeof Struct: " + String(sizeof(sensor)));
  Serial.println("Sending message to MQTT topic..");
  Serial.println(JSONmessageBuffer);
  
  //JSONmessageBuffer = Data_Struct_to_Json();
  
 
  if (client.publish("Deneme_Json", JSONmessageBuffer) == true){
    Serial.println("Success sending message");
  } 
  else{
    Serial.println("Error sending message");
  }
}

void LoRa_Listen(){
  while (e22.available()  > 1) {
    // Gelen mesaj okunuyor
    ResponseStructContainer rsc = e22.receiveMessage(sizeof(AGV_Sensors));
    sensor = *(AGV_Sensors*) rsc.data;

    Serial.println(F("Comand Control Station received message from LoRa!"));
    Serial.println("Battery Level: " + String(sensor.batteryLevel));
    Serial.println("Load Cell: " + String(sensor.loadCell));
    Serial.println("Left Motor RPM: " + String(sensor.leftMotorRpm));
    Serial.println("Vehicle Speed: " + String(sensor.vehicleSpeed));
    Serial.println("Battery Current: " + String(sensor.batteryCurrent));
    Serial.println("Left Motor Current Used: " + String(sensor.leftMotorCurrent));
    Serial.println("Right Motor Current Used: " + String(sensor.rightMotorCurrent));
    Serial.println("Forward Distance: " + String(sensor.forwardSensorDistance));
    Serial.println("Left Front Cross Distance: " + String(sensor.leftFrontCrossSensorDistance));
    Serial.println("Left Distance: " + String(sensor.leftSensorDistance));
    Serial.println("Left Rear Cross Distance: " + String(sensor.leftRearCrossSensorDistance));
    Serial.println("ECU Temperature: " + String(sensor.ecuTemperature));
    Serial.print(F("Fan State: ")); (sensor.fanState == true) ? Serial.println(F("On")) : Serial.println("Off");
    Serial.print(F("Car Lights: ")); (sensor.carLights == true) ? Serial.println(F("On")) : Serial.println("Off");
    Serial.println(F("--------------------------------------------------------------------------\n"));
    
    rsc.close();
    transmit = true;
//    char* source = "ESP32 Aleykum Selam Fixaj.com";
//    strcpy(data.type, source);
// 
//    ResponseStatus rs = e22.sendFixedMessage(0, 1, 18, &data, sizeof(Signal));
//    Serial.println(rs.getResponseDescription());
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
    }
    else if(messageTemp == "off"){
      Serial.println("off");
    }
  }
}

void WiFi_Connect(){
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void MQTT_Connect(){
   while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("Deneme");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
