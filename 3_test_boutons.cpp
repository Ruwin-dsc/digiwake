// DIGIWAKE - Test unitaire des 4 boutons (pull-up interne + anti-rebond logiciel 30 ms)
const uint8_t BROCHES[4] = {15, 16, 17, 18};
const char *NOMS[4] = {"MENU", "PLUS", "MOINS", "STOP"};
const uint32_t ANTI_REBOND_MS = 30;

bool etatStable[4], derniereLecture[4];
uint32_t dernierChangement[4];
uint32_t compteur[4];

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) {
    pinMode(BROCHES[i], INPUT_PULLUP);          // repos = 1, appui = 0
    etatStable[i] = derniereLecture[i] = HIGH;
  }
  Serial.println("Appuyez 20 fois sur chaque bouton");
}

void loop() {
  for (int i = 0; i < 4; i++) {
    bool lecture = digitalRead(BROCHES[i]);
    if (lecture != derniereLecture[i]) { dernierChangement[i] = millis(); derniereLecture[i] = lecture; }
    if (millis() - dernierChangement[i] > ANTI_REBOND_MS && lecture != etatStable[i]) {
      etatStable[i] = lecture;
      if (etatStable[i] == LOW) {               // front 1 -> 0 = appui validé
        compteur[i]++;
        Serial.printf("MENU=%lu  PLUS=%lu  MOINS=%lu  STOP=%lu\n", compteur[0], compteur[1], compteur[2], compteur[3]);
      }
    }
  }
}
