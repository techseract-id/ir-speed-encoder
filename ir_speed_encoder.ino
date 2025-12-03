#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inisialisasi LCD I2C (alamat 0x27, 16 kolom, 2 baris)
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int ENCODER_PIN = 2;

const int SLOTS_PER_REVOLUTION = 20;      // Jumlah slot pada disk encoder (sesuaikan dengan FC-03 Anda)
const unsigned long RPM_INTERVAL = 1000;  // Hitung RPM setiap 1 detik
unsigned long lastTimeRPM = 0;
float currentRPM = 0;

volatile unsigned long lastInterruptTime = 0;
const unsigned long DEBOUNCE_TIME = 500;

volatile long pulseCount = 0;
long lastPulseCount = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ENCODER_PIN, INPUT_PULLUP);
  
  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  
  // Tampilan awal
  lcd.setCursor(0, 0);
  lcd.print("IR Speed Sensor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
  
  // Attach interrupt untuk mendeteksi perubahan dari HIGH ke LOW
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), countPulse, FALLING);

  Serial.println("IR Speed Sensor - Optoencoder");
  delay(500);
}

void loop() {
  // Hitung RPM setiap interval tertentu
  if (millis() - lastTimeRPM >= RPM_INTERVAL) {
    noInterrupts();
    long currentPulse = pulseCount;
    interrupts();    

    long pulseInInterval = currentPulse - lastPulseCount;
    currentRPM = (pulseInInterval * 60.0) / SLOTS_PER_REVOLUTION;

    // Tampilkan data ke Serial Monitor
    Serial.print(pulseInInterval);
    Serial.print("       | ");
    Serial.print(currentRPM, 2);
    Serial.println(" RPM");
    
    lcd.clear();
    // Baris 1: Pulse Count
    lcd.setCursor(0, 0);
    lcd.print("Pulse: ");
    lcd.print(pulseInInterval);
    // Baris 2: RPM
    lcd.setCursor(0, 1);
    lcd.print("RPM: ");
    lcd.print(currentRPM, 2);
    
    lastPulseCount = currentPulse;
    lastTimeRPM = millis();
  }
}

//COUNT PULSES
void countPulse() {
  unsigned long interruptTime = micros();
  if (interruptTime - lastInterruptTime > DEBOUNCE_TIME) {
    pulseCount++;
    lastInterruptTime = interruptTime;
  }
}