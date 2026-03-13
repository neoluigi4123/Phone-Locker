#include <LSS.h>
#include <Arduino_RouterBridge.h>

LSS myLSS = LSS(0); // ID: 0
void Move(int degrees, int percent);

String traiter_commande(String message) {
  if (message == "ping") {
    return "pong";
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
      delay(3000);
      
      return "Motor moved to " + String(angle) + " with speed " + String(speed);
    } else {
      return "Error: format must be move:angle,speed";
    }
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
