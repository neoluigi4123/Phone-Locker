#include <Arduino_RouterBridge.h>
#include <Wire.h>

int current_pos = 1; // From 1 to 36

#define MOTOR_ADDR 0x0F
#define LED 13

int lineFinderPin = 3;

// motor commands
#define SpeedSet     0x82
#define DirectionSet 0xAA

// directions
#define M1_CW  0x01
#define M1_CCW 0x02
#define M2_CW  0x04
#define M2_CCW 0x08

void setSpeed(uint8_t m1, uint8_t m2) {
    Wire.beginTransmission(MOTOR_ADDR);
    Wire.write(SpeedSet);
    Wire.write(m1);
    Wire.write(m2);
    Wire.endTransmission();
    delay(50);
}

void setDirection(uint8_t dir) {
    Wire.beginTransmission(MOTOR_ADDR);
    Wire.write(DirectionSet);
    Wire.write(dir);
    Wire.write((uint8_t)0x00);
    Wire.endTransmission();
    delay(50);
}

// stop both motors
void stop() {
    setSpeed(0, 0);
    setDirection(0x00);
}

String traiter_commande(String message) {
  if (message == "ping") {
    return "pong";
  }

  if (message == "Fermer") {
    fermer();
    return "Door Closed";
  }

  if (message == "Ouvrir") {
    ouvrir();
    return "Door Opened";
  }

  if (message.startsWith("move:")) {
    String parameters = message.substring(5); 
    int commaIndex = parameters.indexOf(',');
    
    if (commaIndex != -1) {
      String angleStr = parameters.substring(0, commaIndex);
      String speedStr = parameters.substring(commaIndex + 1);
      
      int angle = angleStr.toInt();
      int speed = speedStr.toInt();
      
      Move(angle, speed);
      
      return "Motor moved to " + String(angle) + " with speed " + String(speed);
    } else {
      return "Error: format must be move:angle,speed";
    }
  }

  if (message.startsWith("Id:")) {
    String parameters = message.substring(3);
    int id = parameters.toInt();
      
    Id(id);
      
    return "Open on id " + String(id);
  }
  
  return "Reçu : " + message;
}

void setup() {
  pinMode(LED, OUTPUT);
  delay(2000);
  Wire.begin();
  Wire.setClock(50000);
  delay(500);

  // check connection
 // Wire.beginTransmission(MOTOR_ADDR);
 // if (Wire.endTransmission() != 0) {
 //     while (1) {
 //         digitalWrite(LED, HIGH); delay(80);
  //        digitalWrite(LED, LOW);  delay(80);
  //    }
  //}

    // stop();
    // delay(500);

  pinMode(lineFinderPin, INPUT);

  Serial.begin(115200); 
  Bridge.begin();
  Bridge.provide("traiter_commande", traiter_commande);
}

void loop() {
  Bridge.update();
}

void Move(int degrees, int percent) {
  setDirection(M2_CW);
  setSpeed(0, 255);
  
  bool prev_lineFinder = digitalRead(lineFinderPin);

  while(digitalRead(lineFinderPin) == prev_lineFinder){
    delay(50);
  }
  // Check when the lineFinderPin switches to black
  while(digitalRead(lineFinderPin) == 0){
    delay(50);
  }
      
  current_pos += 1;
  // stop
  stop();
  delay(300);
}

void Id(int id) {
  int pos = map(id, 1, 36, 0, 350);
  Move(pos, 60);
  delay(1000);
  ouvrir();
  delay(1000);
  fermer();
}

void ouvrir(){
  setDirection(M1_CW);
  setSpeed(128, 0);
  delay(300);

  // stop
  stop();
  delay(300);
}

void fermer() {
  setDirection(M1_CCW);
  setSpeed(128, 0);
  delay(300);

  // stop
  stop();
  delay(300); 
}
