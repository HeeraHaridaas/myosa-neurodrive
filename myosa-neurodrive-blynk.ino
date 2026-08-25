#define BLYNK_TEMPLATE_ID "TMPL3GB4M8Q6c"
#define BLYNK_TEMPLATE_NAME "NeuroDrive"
#define BLYNK_AUTH_TOKEN "okzbOzk1bPMiyA6biLCxkGT46ZCvN6ct"

// #define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <myosa.h>

char ssid[] = "Galaxy M13 C71B";
char pass[] = "ylkd4652";

#define BUZZER_PIN 16
#define SEA_LEVEL_PRESSURE 1013.25 // Standard baseline for altitude (hPa)[cite: 1]

MYOSA myosa;
BlynkTimer timer;
unsigned long journeyStartTime = 0;
float baseSteeringVariance = 0;

void drawHeader() {
  myosa.display.fillRect(0, 0, 128, 14, WHITE);
  myosa.display.setTextColor(BLACK, WHITE);
  myosa.display.setFont(nullptr); 
  myosa.display.setTextSize(1);
  myosa.display.setCursor(32, 3);
  myosa.display.print("NEURODRIVE");
  myosa.display.drawLine(0, 15, 128, 15, WHITE);
  myosa.display.setTextColor(WHITE);
}

// Reverted to Active Buzzer logic (Standard Voltage)
void triggerBuzzer(int durationMs) {
  digitalWrite(BUZZER_PIN, HIGH); 
  delay(durationMs);
  digitalWrite(BUZZER_PIN, LOW); 
}

void processVigilanceData() {
  float S = 0, C = 0, J = 0;
  float currentGyroX = 0, currentGyroY = 0, tempC = 0, altitudeM = 0, pressure = 0;
  float accelMag = 0;
  int ambientLight = 0;

  if(myosa.Ag.ping()) {
    currentGyroX = myosa.Ag.getGyroX();
    currentGyroY = myosa.Ag.getGyroY();
    
    accelMag = sqrt(pow(myosa.Ag.getAccelX(), 2) + pow(myosa.Ag.getAccelY(), 2) + pow(myosa.Ag.getAccelZ(), 2));
    float currentSteering = abs(currentGyroX) + abs(currentGyroY);
    
    if (currentSteering < (baseSteeringVariance * 0.5)) {
      S = map(currentSteering * 100, 0, (baseSteeringVariance * 0.5) * 100, 100, 40);
    } else if (currentSteering > (baseSteeringVariance * 2.0)) {
      S = map(constrain(currentSteering, baseSteeringVariance * 2.0, baseSteeringVariance * 4.0), 
              baseSteeringVariance * 2.0, baseSteeringVariance * 4.0, 40, 100);
    } else {
      S = 10;
    }
  }
  
  if(myosa.Pr.ping()) {
    tempC = myosa.Pr.getTempC();
    altitudeM = myosa.Pr.getAltitude(SEA_LEVEL_PRESSURE);
    pressure = myosa.Pr.getPressurePascal() / 100.0; 
    
    C = map(constrain(tempC, 24.0, 38.0), 24.0, 38.0, 0, 100);
  }
  
  if(myosa.Lpg.ping()) {
    ambientLight = myosa.Lpg.getAmbientLight();
  }
  
  J = constrain(((millis() - journeyStartTime) / 7200000.0) * 100.0, 0, 100);

  float DVS = (0.65 * S) + (0.20 * C) + (0.15 * J);
  DVS = constrain(DVS, 0, 100);

  // --- Update Blynk Cloud Dashboard ---
  Blynk.virtualWrite(V0, DVS);
  Blynk.virtualWrite(V2, tempC);
  Blynk.virtualWrite(V3, altitudeM);
  Blynk.virtualWrite(V4, pressure);
  Blynk.virtualWrite(V5, accelMag);
  Blynk.virtualWrite(V6, ambientLight);

  // --- Render OLED UI ---
  myosa.display.clearDisplay();
  drawHeader();
  
  myosa.display.setCursor(0, 19);
  myosa.display.print("T:");
  myosa.display.print(tempC, 1);
  myosa.display.print("C");
  
  myosa.display.setCursor(68, 19);
  myosa.display.print("Alt:");
  myosa.display.print(altitudeM, 0);
  myosa.display.print("m");

  myosa.display.setCursor(0, 31);
  myosa.display.print("Vigilance: ");
  myosa.display.print(DVS, 1);
  myosa.display.print("%");

  myosa.display.drawRect(0, 42, 128, 7, WHITE);
  int barWidth = map((int)DVS, 0, 100, 0, 124);
  myosa.display.fillRect(2, 44, barWidth, 3, WHITE);

  int beepDuration = 0;
  bool triggerAlert = false;

  // Escalating Durations based on severity
  if(DVS >= 85.0) {
    myosa.display.fillRect(0, 52, 128, 12, WHITE);
    myosa.display.setTextColor(BLACK, WHITE);
    myosa.display.setCursor(15, 54);
    myosa.display.print("!! DANGER LEVEL !!");
    Blynk.virtualWrite(V1, "DANGER LEVEL"); 
    triggerAlert = true;
    beepDuration = 500; // Long beep
  } else if(DVS >= 70.0) {
    myosa.display.fillRect(0, 52, 128, 12, WHITE);
    myosa.display.setTextColor(BLACK, WHITE);
    myosa.display.setCursor(20, 54);
    myosa.display.print("! CRITICAL ALERT !");
    Blynk.virtualWrite(V1, "CRITICAL ALERT"); 
    triggerAlert = true;
    beepDuration = 250; // Medium beep
  } else if (DVS >= 60.0) {
    myosa.display.setCursor(0, 54);
    myosa.display.print("Status: WARNING");
    Blynk.virtualWrite(V1, "WARNING");
    triggerAlert = true;
    beepDuration = 100; // Short warning chirp
  } else {
    myosa.display.setCursor(0, 54);
    myosa.display.print("Status: OPTIMAL");
    Blynk.virtualWrite(V1, "OPTIMAL");
  }
  myosa.display.display();

  if(triggerAlert) {
    triggerBuzzer(beepDuration);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);
  
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  myosa.begin();

  myosa.display.clearDisplay();
  drawHeader();
  myosa.display.setCursor(24, 34);
  myosa.display.print("Connecting to");
  myosa.display.setCursor(40, 48);
  myosa.display.print("WiFi...");
  myosa.display.display();
  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  unsigned long calibStart = millis();
  int samples = 0;
  int countdown = 20;
  unsigned long lastTick = millis();
  
  while(millis() - calibStart < 20000) {
    if (millis() - lastTick >= 1000) {
      myosa.display.clearDisplay();
      drawHeader();
      
      myosa.display.setCursor(10, 22);
      myosa.display.print("Calibrating Sensor");
      myosa.display.setCursor(20, 34);
      myosa.display.print("Hold Wheel Steady");
      
      myosa.display.setTextSize(2);
      myosa.display.setCursor(54, 46);
      if(countdown < 10) myosa.display.print("0");
      myosa.display.print(countdown);
      myosa.display.setTextSize(1);
      myosa.display.display();
      
      countdown--;
      lastTick = millis();
    }

    if(myosa.Ag.ping()) {
      baseSteeringVariance += abs(myosa.Ag.getGyroX()) + abs(myosa.Ag.getGyroY());
      samples++;
    }
    delay(40);
  }
  
  baseSteeringVariance /= (samples > 0 ? samples : 1);
  if (baseSteeringVariance < 1.2) baseSteeringVariance = 1.2; 
  
  myosa.display.clearDisplay();
  drawHeader();
  myosa.display.setCursor(12, 25);
  myosa.display.print("Calibration Done!");
  myosa.display.setCursor(8, 42);
  myosa.display.print("Live Monitoring ON");
  myosa.display.drawRect(4, 38, 120, 20, WHITE);
  myosa.display.display();
  delay(2000);
  
  journeyStartTime = millis();
  
  timer.setInterval(1000L, processVigilanceData);
}

void loop() {
  Blynk.run();
  timer.run();
}