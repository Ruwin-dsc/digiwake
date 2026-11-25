// DIGIWAKE - Test unitaire du buzzer passif avec ledcWriteTone()
// Écrit pour le core Arduino ESP32 version 3.x (ledcAttach). Pour la version 2.x :
//   ledcSetup(0, 2000, 8); ledcAttachPin(PIN_BUZ, 0); puis ledcWriteTone(0, f) au lieu de ledcWriteTone(PIN_BUZ, f)
#include <Arduino.h>
#define PIN_BUZ 7

struct Note { uint16_t f; uint16_t ms; };   // fréquence (Hz, 0 = silence) et durée (ms)

// Mélodies en mémoire Flash (const) - chacune dure moins de 10 s
const Note MELODIE1[] = {   // Frère Jacques (traditionnel)
  {262,400},{294,400},{330,400},{262,400},{262,400},{294,400},{330,400},{262,400},
  {330,400},{349,400},{392,800},{330,400},{349,400},{392,800}};
const Note MELODIE2[] = {   // Au clair de la lune (traditionnel)
  {262,400},{262,400},{262,400},{294,400},{330,800},{294,800},
  {262,400},{330,400},{294,400},{294,400},{262,1600}};

void jouer(const Note *m, int n) {
  uint32_t debut = millis();
  for (int i = 0; i < n; i++) {
    ledcWriteTone(PIN_BUZ, m[i].f);   // carré à f Hz, rapport cyclique 50 %
    delay(m[i].ms);
    ledcWriteTone(PIN_BUZ, 0);        // silence entre deux notes
    delay(50);
  }
  Serial.printf("  duree de la melodie : %lu ms\n", millis() - debut);
}

void setup() {
  Serial.begin(115200);
  ledcAttach(PIN_BUZ, 2000, 8);       // broche, fréquence initiale, résolution 8 bits

  Serial.println("Gamme Do4 -> Do5, 300 ms par note");
  const uint16_t gamme[] = {262, 294, 330, 349, 392, 440, 494, 523};
  for (uint16_t f : gamme) { Serial.printf("  %u Hz\n", f); ledcWriteTone(PIN_BUZ, f); delay(300); }
  ledcWriteTone(PIN_BUZ, 0); delay(1000);

  Serial.println("La4 = 440 Hz pendant 5 s (mesure de la periode a l'oscilloscope)");
  ledcWriteTone(PIN_BUZ, 440); delay(5000); ledcWriteTone(PIN_BUZ, 0); delay(1000);
}

void loop() {
  Serial.println("Melodie 1"); jouer(MELODIE1, sizeof(MELODIE1) / sizeof(Note)); delay(2000);
  Serial.println("Melodie 2"); jouer(MELODIE2, sizeof(MELODIE2) / sizeof(Note)); delay(2000);
}
