#include <Arduino_RouterBridge.h>

String traiter_commande(String message) {
  if (message == "ping") {
    return "pong";
  }
  
  return "Reçu : " + message;
}

void setup() {
  Serial.begin(9600);
  Bridge.begin();
  Bridge.provide("traiter_commande", traiter_commande);
}

void loop() {
  Bridge.update();
}
