// DIGIWAKE - Test unitaire des 4 matrices 1088AS + MAX7219 (trames 16 bits envoyées "à la main")
#include <Arduino.h>
#define PIN_DIN 11
#define PIN_CLK 12
#define PIN_CS  10
#define NB_MOD  4

uint8_t fb[NB_MOD][8];   // image : fb[module][ligne], 1 bit par LED
// Panneau alimenté en 3,3 V (en 5 V, les signaux 3,3 V de l ESP32 sont mal reconnus)

// Envoie le même registre/donnée aux 4 MAX7219 chaînés : 4 trames de 16 bits puis un front montant de CS
void envoyerATous(uint8_t reg, uint8_t data) {
  digitalWrite(PIN_CS, LOW);
  for (int m = 0; m < NB_MOD; m++) {
    shiftOut(PIN_DIN, PIN_CLK, MSBFIRST, reg);   // D11..D8 = adresse
    shiftOut(PIN_DIN, PIN_CLK, MSBFIRST, data);  // D7..D0  = donnée
  }
  digitalWrite(PIN_CS, HIGH);                    // validation au front montant
}

void rafraichir() {
  for (int l = 0; l < 8; l++) {
    digitalWrite(PIN_CS, LOW);
    for (int m = NB_MOD - 1; m >= 0; m--) {      // la 1re trame envoyée finit dans le module le plus loin
      shiftOut(PIN_DIN, PIN_CLK, MSBFIRST, l + 1);
      shiftOut(PIN_DIN, PIN_CLK, MSBFIRST, fb[m][l]);
    }
    digitalWrite(PIN_CS, HIGH);
  }
}

void effacer() { memset(fb, 0, sizeof(fb)); }
// x de 0 (gauche) à 31, y de 0 (haut) à 7. Si l'image est inversée : changer (NB_MOD-1-x/8) en (x/8)
void pixel(int x, int y, bool on) {
  int m = NB_MOD - 1 - x / 8;
  uint8_t bit = 0x80 >> (x % 8);
  if (on) fb[m][y] |= bit; else fb[m][y] &= ~bit;
}

const uint8_t CHIFFRES[10][7] = {   // police 5x7, bit 4 = colonne de gauche
  {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E},{0x04,0x0C,0x04,0x04,0x04,0x04,0x0E},
  {0x0E,0x11,0x01,0x02,0x04,0x08,0x1F},{0x1E,0x01,0x01,0x0E,0x01,0x01,0x1E},
  {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02},{0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E},
  {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E},{0x1F,0x01,0x02,0x04,0x08,0x08,0x08},
  {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E},{0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C}};

void chiffre(int c, int x0) {
  for (int y = 0; y < 7; y++)
    for (int x = 0; x < 5; x++)
      pixel(x0 + x, y, CHIFFRES[c][y] & (0x10 >> x));
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_DIN, OUTPUT); pinMode(PIN_CLK, OUTPUT); pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_CS, HIGH);
  envoyerATous(0x0F, 0x00);   // display test off
  envoyerATous(0x0C, 0x01);   // shutdown : marche
  envoyerATous(0x0B, 0x07);   // scan limit : 8 lignes
  envoyerATous(0x09, 0x00);   // pas de décodage 7 segments
  envoyerATous(0x0A, 0x02);   // intensité faible (0..15)

  Serial.println("Phase 1 : display test, 256 LEDs allumees 2 s");
  envoyerATous(0x0F, 0x01); delay(2000); envoyerATous(0x0F, 0x00);

  Serial.println("Phase 2 : balayage LED par LED");
  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 32; x++) { effacer(); pixel(x, y, true); rafraichir(); delay(15); }

  Serial.println("Phase 3 : affichage 12:34, ':' clignote a 1 Hz");
  effacer(); chiffre(1, 2); chiffre(2, 8); chiffre(3, 18); chiffre(4, 24); rafraichir();
}

void loop() {
  static bool deuxPoints = false;
  deuxPoints = !deuxPoints;
  pixel(15, 2, deuxPoints); pixel(15, 4, deuxPoints);
  rafraichir();
  delay(500);                 // 500 ms allumé + 500 ms éteint = période 1 s
}
