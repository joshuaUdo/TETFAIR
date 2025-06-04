//MY OWN CODE NEVER GREE DEY WORK SIR.

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#define IRSensor 2
#define BUTTON_PIN 3 
#define LED_PIN 7
#define SIGNAL_IN_PIN 5

#define FRONTLIGHT 6
#define NUMPIXELS 200

#define BACKLIGHT 4
#define NUMPIXELS_2 200 

#define MODE_ADDRESS 0

Adafruit_NeoPixel strip1(NUMPIXELS, FRONTLIGHT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(NUMPIXELS_2, BACKLIGHT, NEO_GRB + NEO_KHZ800);

enum Mode {
  AUTOMATIC_MODE,
  MANUAL_MODE
};

Mode currentMode;
bool lastButtonState = HIGH; 
bool lastSignalState = LOW; 

unsigned long motionStartTime = 0;
const unsigned long motionLightDuration = 5000; 
bool frontLightOn = false; 

void AutomaticMode();
void ManualMode();
void saveModeToEEPROM(Mode mode);
Mode loadModeFromEEPROM();

void turnOnBacklight() { 
  for (int i = 0; i < NUMPIXELS_2; i++) { 
    strip2.setPixelColor(i, strip2.Color(255, 255, 255)); // White
  }
  strip2.show();
}

void turnOffBacklight() { 
  strip2.clear();
  strip2.show();
}

void turnOnLED() {
  digitalWrite(LED_PIN, HIGH);
}

void turnOffLED() {
  digitalWrite(LED_PIN, LOW);
}

void enterAutomaticMode() {
  currentMode = AUTOMATIC_MODE;
  turnOnBacklight();
  turnOffLED();
  strip1.show();
  frontLightOn = false;
  saveModeToEEPROM(currentMode);
  Serial.println("Switched to AUTOMATIC MODE");
}

void enterManualMode() { 
  currentMode = MANUAL_MODE;
  turnOffBacklight();
  turnOnLED();
  strip1.show();
  frontLightOn = false;
  saveModeToEEPROM(currentMode);
  Serial.println("Switched to MANUAL MODE");
}

// FRONTLIGHT ANIMATION ~ PURPLE + PINK
void fadeToColor(Adafruit_NeoPixel& strip, byte r1, byte g1, byte b1, byte r2, byte g2, byte b2) {
  const int steps = 30;
  for (int i = 0; i <= steps; i++) {
    float progress = i / (float)steps;

    byte r = r1 + (r2 - r1) * progress;
    byte g = g1 + (g2 - g1) * progress;
    byte b = b1 + (b2 - b1) * progress;

    for (int j = 0; j < NUMPIXELS; j++) {
      strip.setPixelColor(j, strip1.Color(r, g, b));
    }
    strip.show();
  }
}

void saveModeToEEPROM(Mode mode) {
  EEPROM.update(MODE_ADDRESS, (byte)mode);
}

Mode loadModeFromEEPROM() {
  byte value = EEPROM.read(MODE_ADDRESS);
  if (value == AUTOMATIC_MODE || value == MANUAL_MODE) {
    return (Mode)value;
  } else {
    return AUTOMATIC_MODE; 
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(IRSensor, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  pinMode(SIGNAL_IN_PIN, INPUT_PULLUP);

  // FRONT LIGHT
  strip1.begin();
  strip1.setBrightness(200);
  strip1.show();

  // BACK LIGHT
  strip2.begin();
  strip2.setBrightness(200);

  currentMode = loadModeFromEEPROM();
  if (currentMode == AUTOMATIC_MODE) {
    enterAutomaticMode(); 
  } else {
    enterManualMode(); 
  }
}

void loop() {
  // --- Button Logic ---
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    static unsigned long lastButtonPressTime = 0;
    if (millis() - lastButtonPressTime > 50) { 
      if (currentMode == AUTOMATIC_MODE) {
        enterManualMode();
      } else {
        enterAutomaticMode();
      }
      lastButtonPressTime = millis();
    }
  }
  lastButtonState = currentButtonState;

  // --- IR Sensor Logic (Non-blocking) ---
  if (currentMode == AUTOMATIC_MODE) {
    int sensorStatus = digitalRead(IRSensor);
    if (sensorStatus == LOW) { 
      if (!frontLightOn) { 
        Serial.println("Motion Detected! Front light ON.");
        motionStartTime = millis();
        frontLightOn = true;
      }
    }

    if (frontLightOn) {
      fadeToColor(strip1, 255, 50, 200, 128, 0, 128);
      if (millis() - motionStartTime >= motionLightDuration) {
        strip1.clear();
        strip1.show();
        frontLightOn = false;
        Serial.println("Front light OFF (motion timeout).");
      }
    } else {
      strip1.clear();
      strip1.show();
    }
  } else { 
    if (frontLightOn) {
      strip1.clear();
      strip1.show();
      frontLightOn = false;
    }
  }

  // --- Signal Input Logic (Manual Mode) ---
  if (currentMode == MANUAL_MODE) {
    bool currentSignalState = digitalRead(SIGNAL_IN_PIN);
    static unsigned long lastSignalChangeTime = 0;

    if (currentSignalState == HIGH && lastSignalState == LOW) { 
      if (millis() - lastSignalChangeTime > 50) { 
        turnOnBacklight();
        Serial.println("Signal HIGH - Backlight ON");
        lastSignalChangeTime = millis();
      }
    } else if (currentSignalState == LOW && lastSignalState == HIGH) { 
      if (millis() - lastSignalChangeTime > 50) { 
        turnOffBacklight();
        Serial.println("Signal LOW - Backlight OFF");
        lastSignalChangeTime = millis();
      }
    }
    lastSignalState = currentSignalState;
  }
}
