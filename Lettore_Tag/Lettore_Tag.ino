#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

// Definisci i pin per il modulo RFID
#define SS_PIN 10
#define RST_PIN 9

// Crea un'istanza della libreria MFRC522
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Indirizzo I2C del display LCD (potrebbe essere 0x27 o 0x3F, controlla il tuo display)
LiquidCrystal_I2C lcd(0x27, 16, 2); // Per un display 16x2

// Timeout per ripristinare la schermata iniziale (ms)
#define TIMEOUT_DISPLAY 5000
unsigned long ultimaLettura = 0;
bool tagVisualizzato = false;

// Mostra la schermata iniziale
void schermataIniziale() {
  lcd.clear();
  lcd.print("Lettore RFID");
  lcd.setCursor(0, 1);
  lcd.print("Avvicina Tag...");
}

void setup() {
  Serial.begin(9600);
  SPI.begin();      // Inizia la comunicazione SPI
  mfrc522.PCD_Init(); // Inizializza il modulo MFRC522

  // Inizializza il display LCD
  lcd.init();
  lcd.backlight();
  schermataIniziale();
}

void loop() {
  // Ripristina la schermata iniziale dopo il timeout
  if (tagVisualizzato && (millis() - ultimaLettura >= TIMEOUT_DISPLAY)) {
    schermataIniziale();
    tagVisualizzato = false;
  }

  // Cerca nuove schede
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Seleziona una delle schede
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID della scheda: ");
  String tagId = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    tagId += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") + String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println(tagId);

  // Mostra l'UID sul display LCD
  lcd.clear();
  lcd.print("Tag rilevato:");
  lcd.setCursor(0, 1);
  lcd.print(tagId);

  // Avvia il timeout per il ripristino della schermata
  ultimaLettura = millis();
  tagVisualizzato = true;

  // Ferma la lettura per un breve periodo
  delay(1000);

  // Ferma la comunicazione con la scheda corrente
  mfrc522.PICC_HaltA();

  // Ferma la crittografia sul PCD
  mfrc522.PCD_StopCrypto1();
}
