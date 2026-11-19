
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==============================================
// AQUASENCE SMART IRRIGATION SYSTEM
// Automatic irrigation + flow potentiometers
// ==============================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

// Potentiometer inputs
const int SOIL1_PIN = 34;
const int SOIL2_PIN = 35;
const int PRESSURE_PIN = 14;
const int FLOW1_PIN = 32;
const int FLOW2_PIN = 33;

// LED outputs
const int PUMP_LED = 25;
const int VALVE1_LED = 26;
const int VALVE2_LED = 27;
const int FLOW1_LED = 16;
const int FLOW2_LED = 17;
const int PRESSURE_LED = 4;
const int LOW_PRESSURE_LED = 5;

// Thresholds
const int DRY_THRESHOLD = 35;
const int WET_TARGET = 100;
const int PRESSURE_LIMIT = 75;
const int FLOW_THRESHOLD = 10;

Adafruit_SSD1306 display(
  SCREEN_WIDTH, SCREEN_HEIGHT,
  &Wire, OLED_RESET
);

enum IrrigationState {
  STANDBY,
  ZONE1_WATERING,
  ZONE2_WATERING,
  COMPLETE
};

IrrigationState state = STANDBY;

int soil1 = 0;
int soil2 = 0;
int pressure = 0;
int flow1Level = 0;
int flow2Level = 0;

bool pumpOn = false;
bool valve1Open = false;
bool valve2Open = false;
bool flow1Detected = false;
bool flow2Detected = false;
bool lowPressure = false;

unsigned long lastUpdate = 0;

// Convert analog input to percentage
int readPercent(int pin) {
  int raw = analogRead(pin);
  return constrain((raw * 100L) / 4095, 0, 100);
}

// Read all potentiometers
void readSensors() {
  soil1 = readPercent(SOIL1_PIN);
  soil2 = readPercent(SOIL2_PIN);

  flow1Level = readPercent(FLOW1_PIN);
  flow2Level = readPercent(FLOW2_PIN);

  flow1Detected = flow1Level > FLOW_THRESHOLD;
  flow2Detected = flow2Level > FLOW_THRESHOLD;
}

// Start irrigation, choosing the drier zone first
void startIrrigation() {
  if (soil1 < DRY_THRESHOLD &&
      soil2 < DRY_THRESHOLD) {
    state = (soil1 <= soil2)
              ? ZONE1_WATERING
              : ZONE2_WATERING;
  } else if (soil1 < DRY_THRESHOLD) {
    state = ZONE1_WATERING;
  } else if (soil2 < DRY_THRESHOLD) {
    state = ZONE2_WATERING;
  }
}

// Automatic sequential irrigation
void controlIrrigation() {
  if (state == STANDBY || state == COMPLETE) {
    if (soil1 < DRY_THRESHOLD ||
        soil2 < DRY_THRESHOLD) {
      startIrrigation();
    }
  }

  pumpOn = false;
  valve1Open = false;
  valve2Open = false;

  if (state == ZONE1_WATERING) {
    pumpOn = true;
    valve1Open = true;

    if (soil1 >= WET_TARGET) {
      if (soil2 < WET_TARGET) {
        state = ZONE2_WATERING;
      } else {
        state = COMPLETE;
      }
    }
  } else if (state == ZONE2_WATERING) {
    pumpOn = true;
    valve2Open = true;

    if (soil2 >= WET_TARGET) {
      if (soil1 < WET_TARGET) {
        state = ZONE1_WATERING;
      } else {
        state = COMPLETE;
      }
    }
  }

  // Recalculate outputs after state changes
  if (state == ZONE1_WATERING) {
    pumpOn = true;
    valve1Open = true;
    valve2Open = false;
  } else if (state == ZONE2_WATERING) {
    pumpOn = true;
    valve1Open = false;
    valve2Open = true;
  } else {
    pumpOn = false;
    valve1Open = false;
    valve2Open = false;
  }

  // Pressure monitoring only while pump is ON
  if (pumpOn) {
    pressure = readPercent(PRESSURE_PIN);
    lowPressure = pressure < PRESSURE_LIMIT;
  } else {
    pressure = 0;
    lowPressure = false;
  }

  // Main indicators
  digitalWrite(PUMP_LED, pumpOn);
  digitalWrite(VALVE1_LED, valve1Open);
  digitalWrite(VALVE2_LED, valve2Open);

  // Flow LEDs follow their potentiometers
  digitalWrite(FLOW1_LED, flow1Detected);
  digitalWrite(FLOW2_LED, flow2Detected);

  digitalWrite(PRESSURE_LED, pumpOn);
  digitalWrite(LOW_PRESSURE_LED,
               pumpOn && lowPressure);
}

// OLED display
void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  display.println("AQUASENCE SMART");
  display.println("----------------");

  display.print("S1:");
  display.print(soil1);
  display.print("% S2:");
  display.print(soil2);
  display.println("%");

  display.print("Pump:");
  display.print(pumpOn ? "ON" : "OFF");

  display.print(" P:");
  if (pumpOn) {
    display.print(pressure);
    display.println("%");
  } else {
    display.println("OFF");
  }

  display.print("Flow1:");
  display.print(flow1Detected ? "YES" : "NO");
  display.print(" Flow2:");
  display.println(flow2Detected ? "YES" : "NO");

  if (lowPressure) {
    display.println("LOW PRESSURE!");
  } else if (state == ZONE1_WATERING) {
    display.println("ZONE 1 WATERING");
  } else if (state == ZONE2_WATERING) {
    display.println("ZONE 2 WATERING");
  } else {
    display.println("IRRIGATION STOPPED");
  }

  display.display();
}

// Serial diagnostics and leakage warnings
void printStatus() {
  Serial.println("\n--- AQUASENCE ---");

  Serial.print("Soil 1: ");
  Serial.print(soil1);
  Serial.println("%");

  Serial.print("Soil 2: ");
  Serial.print(soil2);
  Serial.println("%");

  Serial.print("Pump: ");
  Serial.println(pumpOn ? "ON" : "OFF");

  Serial.print("Pressure: ");
  if (pumpOn) {
    Serial.print(pressure);
    Serial.println("%");
  } else {
    Serial.println("INACTIVE");
  }

  Serial.print("Valve 1: ");
  Serial.println(valve1Open ? "OPEN" : "CLOSED");

  Serial.print("Valve 2: ");
  Serial.println(valve2Open ? "OPEN" : "CLOSED");

  Serial.print("Flow 1: ");
  Serial.println(flow1Level);

  Serial.print("Flow 2: ");
  Serial.println(flow2Level);

  if (pumpOn && lowPressure) {
    Serial.println("WARNING: LOW PRESSURE");
  }

  if (flow1Detected && !valve1Open) {
    Serial.println("POSSIBLE LEAK: CLOSED VALVE 1");
  }

  if (flow2Detected && !valve2Open) {
    Serial.println("POSSIBLE LEAK: CLOSED VALVE 2");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PUMP_LED, OUTPUT);
  pinMode(VALVE1_LED, OUTPUT);
  pinMode(VALVE2_LED, OUTPUT);
  pinMode(FLOW1_LED, OUTPUT);
  pinMode(FLOW2_LED, OUTPUT);
  pinMode(PRESSURE_LED, OUTPUT);
  pinMode(LOW_PRESSURE_LED, OUTPUT);

  analogReadResolution(12);
  Wire.begin(21, 22);

  if (!display.begin(
        SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED initialization failed!");
    while (true) {
      delay(100);
    }
  }

  digitalWrite(PUMP_LED, LOW);
  digitalWrite(VALVE1_LED, LOW);
  digitalWrite(VALVE2_LED, LOW);
  digitalWrite(FLOW1_LED, LOW);
  digitalWrite(FLOW2_LED, LOW);
  digitalWrite(PRESSURE_LED, LOW);
  digitalWrite(LOW_PRESSURE_LED, LOW);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  display.println("AQUASENCE");
  display.println("Automatic Irrigation");
  display.println("System Ready");
  display.display();

  delay(1000);
}

void loop() {
  readSensors();
  controlIrrigation();

  if (millis() - lastUpdate >= 500) {
    lastUpdate = millis();
    updateDisplay();
    printStatus();
  }

  delay(50);
}
