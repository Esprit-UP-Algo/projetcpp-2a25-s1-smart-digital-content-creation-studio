/*
 * SYSTÈME DE POINTAGE DES EMPLOYÉS - VERSION TEST
 * Clavier 4x4 + LCD I2C 16x2 + Qt + Oracle DB
 * Affichage du rôle de l'employé + Pointage IN/OUT
 * 
 * TEST: Ce code simule les réponses Qt pour tester sans application Qt
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

// ========== MODE TEST ==========
bool testMode = true; // Mettre à false pour utiliser avec Qt

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
  Serial.println("MODE TEST ACTIF - Simulation Qt");
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
  
  // ========== MODE TEST - SIMULATION QT ==========
  if (testMode) {
    simulerReponseQt();
  }
  
  // ========== LECTURE RÉPONSE DE QT (mode normal) ==========
  else if (Serial.available() > 0) {
    traiterReponseQt();
  }
}

// ========== MODE TEST - SIMULATION DES RÉPONSES QT ==========
void simulerReponseQt() {
  static unsigned long lastSimTime = 0;
  static bool simulationEnCours = false;
  
  if (waitingForResponse && !simulationEnCours) {
    simulationEnCours = true;
    lastSimTime = millis();
    
    // Simuler un délai de traitement
    delay(1500);
    
    // Simuler la réponse Qt selon le CIN
    if (cinBuffer == "12345678") {
      // CIN trouvé - Manager
      currentRole = "Manager";
      afficherEmployeTrouve("Manager");
    }
    else if (cinBuffer == "87654321") {
      // CIN trouvé - Employe
      currentRole = "Employe";
      afficherEmployeTrouve("Employe");
    }
    else if (cinBuffer == "11111111") {
      // CIN trouvé - Technicien
      currentRole = "Technicien";
      afficherEmployeTrouve("Technicien");
    }
    else {
      // CIN non trouvé
      afficherCINInconnu();
    }
    
    waitingForResponse = false;
    simulationEnCours = false;
  }
  else if (waitingForPointage && !simulationEnCours) {
    simulationEnCours = true;
    delay(1000);
    
    // Simuler le pointage réussi
    afficherPointageReussi();
    waitingForPointage = false;
    simulationEnCours = false;
  }
}

// ========== TRAITEMENT RÉPONSE QT (mode normal) ==========
void traiterReponseQt() {
  String response = Serial.readStringUntil('\n');
  response.trim();
  
  if (response.startsWith("ROLE:")) {
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
    waitingForPointage = false;
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
  
  if (!testMode) {
    // Envoyer le CIN à Qt
    Serial.print("CIN:");
    Serial.println(cinBuffer);
  }
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
  
  if (!testMode) {
    // Envoyer le pointage à Qt
    Serial.print("POINTAGE:");
    Serial.print(cinBuffer);
    Serial.print(":");
    Serial.println(type);
  }
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
  waitingForResponse = false;
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
  waitingForResponse = false;
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
  waitingForResponse = false;
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
  
  waitingForResponse = false;
  
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
 *      MODE TEST - CINS PRÉDÉFINIS
 * ========================================
 * 
 * Testez avec ces CINS en mode test:
 * 
 * 12345678 -> Manager
 * 87654321 -> Employe  
 * 11111111 -> Technicien
 * Autre   -> CIN inconnu
 * 
 * ========================================
 * 
 * MODE NORMAL:
 * Mettez testMode = false pour utiliser avec Qt
 * 
 * ========================================
 */
