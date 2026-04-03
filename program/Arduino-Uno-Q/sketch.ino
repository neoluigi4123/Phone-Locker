#include <Arduino_RouterBridge.h>
#include <Wire.h>

int current_pos = 1; // From 1 to 36

#define MOTOR_ADDR 0x0F
#define LED 13

int lineFinderPin = 8;
int bouton1 = 4;
int bouton2 = 3;

bool etat_porte = false;

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
  
  return "Recu: " + message;
}

void setup() {
  pinMode(LED, OUTPUT);
  delay(2000);
  Wire.begin();
  Wire.setClock(50000);
  delay(500);

  pinMode(lineFinderPin, INPUT);

  Serial.begin(115200); 
  Bridge.begin();
  Bridge.provide("traiter_commande", traiter_commande);
}

void loop() {
  Bridge.update();
  if (digitalRead(bouton1) == 1){
    if (etat_porte == 0){
      ouvrir();
    } else {
      fermer();
    }
  }
  while (digitalRead(bouton1) == 1){
    delay(50);
  }

  if (digitalRead(bouton2) == 1){
    if (current_pos == 36){
      current_pos = 1;
    } else {
      current_pos =+ 1;
    }
    Id(current_pos);
    delay(1000);
  }
}

void Move(int degrees, int percent) {
  if (current_pos == degrees) {
    stop();
    return;
  }

  // Calculate distance if we were to move clockwise
  // Adding 36 before modulo ensures we don't get negative numbers
  int dist_cw = (degrees - current_pos + 36) % 36;
  
  // If distance is 18 or less, Clockwise is optimal. Otherwise, Counter-Clockwise.
  bool move_cw = (dist_cw <= 18);

  if (move_cw) {
    setDirection(M2_CW);
  } else {
    setDirection(M2_CCW);
  }
  
  setSpeed(0, percent);
  
  while (current_pos != degrees) {
    // Wait until it detects the line
    while(digitalRead(lineFinderPin) != 0) {
      delay(2);
    }
    
    // Wait until it leaves the line completely
    while(digitalRead(lineFinderPin) == 0) {
      delay(2);
    }
        
    // Update position depending on which way we are turning
    if (move_cw) {
      current_pos += 1;
      if (current_pos > 36) {
        current_pos = 1;
      }
    } else {
      current_pos -= 1;
      if (current_pos < 1) {
        current_pos = 36;
      }
    }
  }
  
  // Target reached: Stop the motor completely
  stop();
  delay(500); 
}

void Id(int id) {
  Move(id, 100);
  delay(1000);
  ouvrir();
  delay(1000);
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
