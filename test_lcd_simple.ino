#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Test avec différentes adresses I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Essayez 0x3F si 0x27 ne marche pas

void setup() {
  Serial.begin(9600);
  
  // Initialiser I2C
  Wire.begin();
  
  // Tester si l'écran est détecté
  Serial.println("Test LCD I2C...");
  
  // Initialiser LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Test d'affichage
  lcd.setCursor(0, 0);
  lcd.print("LCD OK!");
  lcd.setCursor(0, 1);
  lcd.print("Test complet");
  
  Serial.println("LCD initialise avec succes");
}

void loop() {
  // Faire clignoter le backlight
  lcd.backlight();
  delay(1000);
  lcd.noBacklight();
  delay(1000);
  lcd.backlight();
}
