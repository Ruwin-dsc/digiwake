// DIGIWAKE - Test unitaire du capteur ultrason HC-SR04
// Capteur alimenté en 3,3 V : ECHO branché directement sur GPIO 14 (si alimenté en 5 V : pont 1 kOhm / 2 kOhm obligatoire)
#include <Arduino.h>
#define PIN_TRIG 13
#define PIN_ECHO 14
#define SEUIL_CM 15.0

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TRIG, OUTPUT); digitalWrite(PIN_TRIG, LOW);
  pinMode(PIN_ECHO, INPUT);
}

void loop() {
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);   // impulsion de 10 us
  digitalWrite(PIN_TRIG, LOW);
  unsigned long t = pulseIn(PIN_ECHO, HIGH, 30000);      // durée d'ECHO, timeout 30 ms
  if (t == 0) {
    Serial.println("pas d'echo (trop loin ou rien devant)");
  } else {
    float d = t / 58.0;                                  // cm
    Serial.printf("t = %5lu us   d = %6.1f cm", t, d);
    if (d < SEUIL_CM) Serial.print("   >>> MAIN DETECTEE");
    Serial.println();
  }
  delay(100);
}
