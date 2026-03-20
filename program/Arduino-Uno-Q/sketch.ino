#include <LSS.h>
#include <Arduino_RouterBridge.h>
#include <Grove_I2C_Motor_Driver.h>

LSS myLSS = LSS(0); // ID: 0
void Move(int degrees, int percent);
void Id(int id);

String traiter_commande(String message) {
  if (message == "ping") {
    return "pong";
  }

  if (message == "Fermer") {
    // fermer();
    return "Door Closed";
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
  LSS::initBus(Serial, LSS_DefaultBaud);

  myLSS.setAngularStiffness(10);
  myLSS.setAngularHoldingStiffness(40);
  myLSS.setAngularDeceleration(60);

  myLSS.move(0);
  delay(2000);

  Motor.begin(0x0F);
  Motor.speed(MOTOR1, 0);
  
  Serial.begin(115200); 
  Bridge.begin();
  Bridge.provide("traiter_commande", traiter_commande);
}

void loop() {
  Bridge.update();
}

void Move(int degrees, int percent) {
  myLSS.setAngularAcceleration(percent);
  
  myLSS.move(degrees*10);
}

void Id(int id) {
  int pos = map(id, 1, 36, 0, 350);
  Move(pos, 60);
  delay(1000);
  ouvrir();
}

void ouvrir(){
  Motor.speed(MOTOR1, 50 );
  delay(1000);
  Motor.speed(MOTOR1, 0 );
}

void fermer () {
 Motor.speed(MOTOR1, -50 );
 delay(1000);
 Motor.speed(MOTOR1, 0 );
}
