// DIGIWAKE - Test unitaire du module RTC DS1302 (liaison série synchrone 3 fils, écrite "à la main")
// Moniteur série 115200 bauds. Taper S puis Entrée pour écrire 12:34:50.
#include <Arduino.h>
#define PIN_CE   6   // RST du module
#define PIN_SCLK 4   // CLK du module
#define PIN_IO   5   // DAT du module (bidirectionnelle)

void ecrireOctet(uint8_t b) {            // LSB en premier, donnée lue par le DS1302 au front montant
  pinMode(PIN_IO, OUTPUT);
  for (int i = 0; i < 8; i++) {
    digitalWrite(PIN_IO, b & 1);
    delayMicroseconds(1);
    digitalWrite(PIN_SCLK, HIGH); delayMicroseconds(1);
    digitalWrite(PIN_SCLK, LOW);  delayMicroseconds(1);
    b >>= 1;
  }
}

uint8_t lireOctet() {                    // le DS1302 sort chaque bit sur le front descendant
  uint8_t b = 0;
  pinMode(PIN_IO, INPUT);
  for (int i = 0; i < 8; i++) {
    if (digitalRead(PIN_IO)) b |= (1 << i);
    digitalWrite(PIN_SCLK, HIGH); delayMicroseconds(1);
    digitalWrite(PIN_SCLK, LOW);  delayMicroseconds(1);
  }
  return b;
}

void ecrireRegistre(uint8_t cmd, uint8_t val) {
  digitalWrite(PIN_CE, HIGH); delayMicroseconds(4);
  ecrireOctet(cmd); ecrireOctet(val);
  digitalWrite(PIN_CE, LOW);  delayMicroseconds(4);
}

uint8_t lireRegistre(uint8_t cmd) {
  digitalWrite(PIN_CE, HIGH); delayMicroseconds(4);
  ecrireOctet(cmd | 0x01);               // bit 0 = 1 : lecture
  uint8_t v = lireOctet();
  digitalWrite(PIN_CE, LOW);  delayMicroseconds(4);
  return v;
}

uint8_t bcd2dec(uint8_t b) { return (b >> 4) * 10 + (b & 0x0F); }

void setup() {
  Serial.begin(115200);
  pinMode(PIN_CE, OUTPUT);   digitalWrite(PIN_CE, LOW);
  pinMode(PIN_SCLK, OUTPUT); digitalWrite(PIN_SCLK, LOW);
  delay(500);
  ecrireRegistre(0x8E, 0x00);            // WP = 0 : écriture autorisée
  uint8_t s = lireRegistre(0x80);
  if (s & 0x80) {                        // CH = 1 : oscillateur arrêté -> on le lance
    ecrireRegistre(0x80, s & 0x7F);
    Serial.println("Oscillateur demarre (CH = 0)");
  }
  Serial.println("Test DS1302 - tapez S pour ecrire 12:34:50");
}

void loop() {
  if (Serial.available() && toupper(Serial.read()) == 'S') {
    ecrireRegistre(0x80, 0x50);          // secondes 50 (BCD), CH = 0
    ecrireRegistre(0x82, 0x34);          // minutes 34
    ecrireRegistre(0x84, 0x12);          // heures 12, mode 24 h (bit 7 = 0)
    Serial.println(">> Heure ecrite : 12:34:50");
  }
  // Lecture en mode burst (commande 0xBF) : sec, min, heure, date, mois, jour, annee, WP
  uint8_t r[8];
  digitalWrite(PIN_CE, HIGH); delayMicroseconds(4);
  ecrireOctet(0xBF);
  for (int i = 0; i < 8; i++) r[i] = lireOctet();
  digitalWrite(PIN_CE, LOW);
  char buf[40];
  sprintf(buf, "%02d:%02d:%02d   (millis = %lu)", bcd2dec(r[2] & 0x3F), bcd2dec(r[1] & 0x7F), bcd2dec(r[0] & 0x7F), millis());
  Serial.println(buf);
  delay(1000);
}
