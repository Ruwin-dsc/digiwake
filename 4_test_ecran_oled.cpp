// DIGIWAKE - Test unitaire de l'écran OLED SSD1306 (I2C)
// Bibliothèques : Adafruit SSD1306 + Adafruit GFX (gestionnaire de bibliothèques)
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define PIN_SDA 8
#define PIN_SCL 9
#define ADRESSE 0x3C

Adafruit_SSD1306 ecran(128, 64, &Wire, -1);

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(PIN_SDA, PIN_SCL);
  Wire.setClock(400000);

  Serial.println("Scanner I2C :");
  bool trouve = false;
  for (uint8_t a = 1; a < 127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) {          // 0 = l'esclave a répondu ACK
      Serial.printf("  peripherique trouve en 0x%02X\n", a);
      if (a == ADRESSE) trouve = true;
    }
  }
  if (!trouve) { Serial.println("ERREUR : 0x3C absente, verifier le cablage"); while (true) delay(1000); }

  if (!ecran.begin(SSD1306_SWITCHCAPVCC, ADRESSE)) { Serial.println("ERREUR init SSD1306"); while (true) delay(1000); }
  Serial.println("OLED OK");
}

void loop() {
  static uint32_t compteur = 0;
  ecran.clearDisplay();                         // efface le buffer (1 Ko en RAM de l'ESP32)
  ecran.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  ecran.setTextColor(SSD1306_WHITE);
  ecran.setTextSize(1); ecran.setCursor(8, 8);  ecran.print("DIGIWAKE - test OLED");
  ecran.setTextSize(2); ecran.setCursor(8, 30); ecran.printf("n = %lu", compteur++);
  ecran.display();                              // envoi du buffer à l'écran par I2C
  delay(1000);
}
