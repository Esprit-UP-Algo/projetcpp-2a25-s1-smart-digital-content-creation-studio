/*
 * SYSTÈME DE POINTAGE DES EMPLOYÉS
 * Clavier 4x4 + LCD I2C 16x2 + Qt + Oracle DB
 * Affichage du rôle de l'employé + Pointage IN/OUT
 */

#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ========== CONFIGURATION LCD I2C ==========
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ========== CONFIGURATION CLAVIER 4x4 ==========
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {13, 10, A0, A1};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ========== VARIABLES GLOBALES ==========
String cinBuffer = "";
const int MAX_CIN_LENGTH = 8;
bool waitingForResponse = false;
bool waitingForPointage = false;
String currentRole = "";

void setup() {
  Serial.begin(9600);
  
  // Initialisation LCD
  Wire.begin();
  delay(50);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Message de bienvenue
  lcd.setCursor(0, 0);
  lcd.print("  POINTAGE");
  lcd.setCursor(0, 1);
  lcd.print("  EMPLOYES");
  delay(2000);
  
  afficherEcranSaisie();
  
  Serial.println("ARDUINO_READY");
}

void loop() {
  // ========== LECTURE DU CLAVIER ==========
  if (!waitingForResponse && !waitingForPointage) {
    char key = keypad.getKey();
    
    if (key) {
      // # = VALIDATION
      if (key == '#') {
        if (cinBuffer.length() > 0) {
          validerCIN();
        } else {
          afficherErreur("CIN vide!");
        }
      }
      
      // * = EFFACEMENT
      else if (key == '*') {
        if (cinBuffer.length() > 0) {
          cinBuffer.remove(cinBuffer.length() - 1);
          afficherCIN();
        } else {
          afficherEcranSaisie();
        }
      }
      
      // 0-9 = CHIFFRES
      else if (key >= '0' && key <= '9') {
        if (cinBuffer.length() < MAX_CIN_LENGTH) {
          cinBuffer += key;
          afficherCIN();
        } else {
          afficherErreur("Max 8 chiffres");
        }
      }
    }
  }
  
  // ========== SÉLECTION IN/OUT ==========
  else if (waitingForPointage) {
    char key = keypad.getKey();
    
    if (key == 'A') {
      // Pointage ENTRÉE
      envoyerPointage("IN");
    }
    else if (key == 'B') {
      // Pointage SORTIE
      envoyerPointage("OUT");
    }
    else if (key == '*') {
      // Annuler et retour à la saisie
      waitingForPointage = false;
      cinBuffer = "";
      currentRole = "";
      afficherEcranSaisie();
    }
  }
  
  // ========== LECTURE RÉPONSE DE QT ==========
  if (Serial.available() > 0) {
    String response = Serial.readStringUntil('\n');
    response.trim();
    
    if (response.startsWith("ROLE:")) {
      // Format: "ROLE:Manager"
      String role = response.substring(5);
      currentRole = role;
      afficherEmployeTrouve(role);
    }
    else if (response == "CIN_INVALIDE") {
      afficherCINInconnu();
    }
    else if (response == "DB_ERROR") {
      afficherErreur("Erreur BD");
    }
    else if (response == "POINTAGE_OK") {
      afficherPointageReussi();
    }
    else if (response == "POINTAGE_ERROR") {
      afficherErreur("Erreur pointage");
    }
    else if (response == "DOUBLE_IN") {
      afficherErreur("Deja pointe IN");
    }
    else if (response == "NO_IN_FOUND") {
      afficherErreur("Pas d'entree");
    }
  }
}

// ========== VALIDATION DU CIN ==========
void validerCIN() {
  waitingForResponse = true;
  
  // Animation
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Verification");
  
  for(int i = 0; i < 3; i++) {
    lcd.setCursor(13 + i, 0);
    lcd.print(".");
    delay(300);
  }
  
  // Envoyer le CIN à Qt
  Serial.print("CIN:");
  Serial.println(cinBuffer);
  
  // Timeout de 5 secondes
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    if (Serial.available() > 0) {
      String response = Serial.readStringUntil('\n');
      response.trim();
      
      if (response.startsWith("ROLE:")) {
        String role = response.substring(5);
        currentRole = role;
        afficherEmployeTrouve(role);
        waitingForResponse = false;
        return;
      }
      else if (response == "CIN_INVALIDE") {
        afficherCINInconnu();
        waitingForResponse = false;
        return;
      }
    }
    delay(50);
  }
  
  // Timeout
  afficherErreur("Timeout");
  waitingForResponse = false;
}

// ========== ENVOYER POINTAGE ==========
void envoyerPointage(String type) {
  waitingForResponse = true;
  
  // Animation
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Enregistrement");
  
  for(int i = 0; i < 3; i++) {
    lcd.setCursor(13 + i, 0);
    lcd.print(".");
    delay(300);
  }
  
  // Envoyer le pointage à Qt
  Serial.print("POINTAGE:");
  Serial.print(cinBuffer);
  Serial.print(":");
  Serial.println(type);
  
  // Timeout de 5 secondes
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    if (Serial.available() > 0) {
      String response = Serial.readStringUntil('\n');
      response.trim();
      
      if (response == "POINTAGE_OK") {
        afficherPointageReussi();
        waitingForResponse = false;
        waitingForPointage = false;
        return;
      }
      else if (response == "POINTAGE_ERROR") {
        afficherErreur("Erreur pointage");
        waitingForResponse = false;
        return;
      }
      else if (response == "DOUBLE_IN") {
        afficherErreur("Deja pointe IN");
        waitingForResponse = false;
        return;
      }
      else if (response == "NO_IN_FOUND") {
        afficherErreur("Pas d'entree");
        waitingForResponse = false;
        return;
      }
    }
    delay(50);
  }
  
  // Timeout
  afficherErreur("Timeout");
  waitingForResponse = false;
}

// ========== AFFICHER: EMPLOYE TROUVÉ + SÉLECTION IN/OUT ==========
void afficherEmployeTrouve(String role) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Role: ");
  lcd.print(role);
  lcd.setCursor(0, 1);
  lcd.print("A=IN B=OUT *=ANN");
  
  waitingForPointage = true;
}

// ========== AFFICHER: POINTAGE RÉUSSI ==========
void afficherPointageReussi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Pointage OK!");
  lcd.setCursor(0, 1);
  lcd.print("  Enregistre");
  
  delay(3000);
  
  // Réinitialiser
  cinBuffer = "";
  currentRole = "";
  waitingForPointage = false;
  afficherEcranSaisie();
}

// ========== AFFICHER: CIN INCONNU ==========
void afficherCINInconnu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Erreur!");
  lcd.setCursor(0, 1);
  lcd.print("  CIN inconnu");
  
  delay(2500);
  
  cinBuffer = "";
  currentRole = "";
  afficherEcranSaisie();
}

// ========== ÉCRAN DE SAISIE ==========
void afficherEcranSaisie() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" SAISIR LE CIN");
  lcd.setCursor(0, 1);
  lcd.print(" [#]=OK [*]=EFF");
}

// ========== AFFICHER CIN EN COURS ==========
void afficherCIN() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CIN: ");
  lcd.print(cinBuffer);
  lcd.print("_");
  
  lcd.setCursor(0, 1);
  lcd.print("[#]=OK [*]=EFF");
}

// ========== AFFICHER ERREUR ==========
void afficherErreur(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  ERREUR:");
  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.print(message);
  
  delay(2000);
  
  if (waitingForPointage) {
    afficherEmployeTrouve(currentRole);
  } else if (cinBuffer.length() > 0) {
    afficherCIN();
  } else {
    afficherEcranSaisie();
  }
}

/*
 * ========================================
 *      PROTOCOLE DE COMMUNICATION
 * ========================================
 * 
 * ARDUINO → QT:
 * - "CIN:12345678" (quand # est pressé)
 * - "POINTAGE:12345678:IN" (pointage entrée)
 * - "POINTAGE:12345678:OUT" (pointage sortie)
 * - "ARDUINO_READY" (au démarrage)
 * 
 * QT → ARDUINO:
 * - "ROLE:Manager" (si CIN trouvé)
 * - "CIN_INVALIDE" (si CIN non trouvé)
 * - "DB_ERROR" (si erreur base de données)
 * - "POINTAGE_OK" (pointage enregistré avec succès)
 * - "POINTAGE_ERROR" (erreur lors du pointage)
 * - "DOUBLE_IN" (si employé déjà pointé IN sans OUT)
 * - "NO_IN_FOUND" (si tentative OUT sans IN préalable)
 * 
 * ========================================
 */
