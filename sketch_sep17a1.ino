#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// PIN DEFINITIONS (Matching your latest screenshot)
#define I2C_SDA 2      // GPIO 2
#define I2C_SCL 1      // GPIO 1
#define DHTPIN 4       // GPIO 4
#define DHTTYPE DHT11
#define SOIL_1 34      // GPIO 34
#define SOIL_2 35      // GPIO 35
#define PIR_PIN 3      // GPIO 3 (PIR is connected to D3 in your image)
#define RELAY_PUMP 12  // GPIO 12

DHT dht(DHTPIN, DHTTYPE);

// Try 0x27 first. IF BLANK, change 0x27 to 0x3F below.
LiquidCrystal_I2C lcd(0x27, 20, 4); 

void setup() {
  // CRITICAL: Initialize I2C with the specific pins from your schematic
  Wire.begin(I2C_SDA, I2C_SCL); 
  
  lcd.init();
  lcd.backlight();
  dht.begin();
  
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PUMP, OUTPUT);
  
  lcd.setCursor(2, 0);
  lcd.print("SOLAR IRRIGATION");
  lcd.setCursor(4, 1);
  lcd.print("SYSTEM ONLINE");
  delay(2000);
  lcd.clear();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int rawSoil1 = analogRead(SOIL_1);
  int rawSoil2 = analogRead(SOIL_2);
  int motion = digitalRead(PIR_PIN);

  // ESP32 uses 12-bit ADC (0-4095)
  int p1 = map(rawSoil1, 0, 4095, 0, 100);
  int p2 = map(rawSoil2, 0, 4095, 0, 100);

  // Display Updates
  lcd.setCursor(0, 0);
  lcd.print("Temp: "); lcd.print(t, 1); lcd.print("C");
  
  lcd.setCursor(0, 1);
  lcd.print("S1:"); lcd.print(p1); lcd.print("% S2:"); lcd.print(p2); lcd.print("%");

  lcd.setCursor(0, 2);
  lcd.print("PIR: "); lcd.print(motion ? "MOTION DETECT" : "NO MOTION   ");

  lcd.setCursor(0, 3);
  lcd.print("PUMP: ");
  if (p1 < 30 || p2 < 30) {
    digitalWrite(RELAY_PUMP, HIGH);
    lcd.print("RUNNING... ");
  } else {
    digitalWrite(RELAY_PUMP, LOW);
    lcd.print("STANDBY    ");
  }

  delay(1000);
}
