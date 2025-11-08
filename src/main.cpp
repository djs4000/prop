#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_SSD1306.h>
//keypad libraries
#include <Keypad.h>


#include "wifi_config.h"

namespace
{
constexpr uint8_t kScreenWidth = 128;
constexpr uint8_t kScreenHeight = 32;
constexpr int8_t kOledResetPin = -1;
constexpr uint8_t kOledI2cAddress = 0x3C;
constexpr int kOledSdaPin = 21;
constexpr int kOledSclPin = 22;


// Declaration for 4x4 keypad
const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {14, 27, 26, 25}; 
byte colPins[COLS] = {33, 32, 35, 34}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Global variables for status and timer
String flameStatus = "On";
unsigned long armedTime = 0; // To store when the 'Armed' state began
const int timerDuration = 10; // 10 seconds


constexpr uint8_t kMaxConnectionAttempts = 20;
constexpr uint16_t kRetryDelayMs = 500;

Adafruit_SSD1306 display(kScreenWidth, kScreenHeight, &Wire, kOledResetPin);

void showStatus(const String &line1, const String &line2 = String())
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(line1);
    if (line2.length() > 0)
    {
        display.println(line2);
    }
    display.display();
}


void updateDisplay(const String &line1=String(""), const String &line2 = String("")) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  
  // Display the status
  display.print("Digital Flame: ");
  display.println(flameStatus);

  // If status is Armed, start and display the timer
  if (flameStatus == "Armed") {
    // Calculate remaining time
    unsigned long currentTime = millis();
    long elapsedTime = (currentTime - armedTime) / 1000;
    long remainingTime = timerDuration - elapsedTime;
    
    // Check if the timer has expired
    if (remainingTime <= 0) {
      flameStatus = "Exploded"; // Automatically change status
      remainingTime = 0; // Display 0
    }
    
    // Display the timer on the screen
    display.setTextSize(2); // Use a larger font for the timer
    display.setCursor(0, 16); // Lower on the 32-pixel screen
    display.print("Timer: ");
    display.print(remainingTime);
    display.println("s");
    }
    display.clearDisplay();
    display.setCursor(12, 0);
    display.println(line1);
    if (line2.length() > 0)
    {
        display.println(line2);
    }
    display.display();



  display.display();
}



void connectToWifi()
{
    Serial.printf("Connecting to %s", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    uint8_t attempts = 0;
    updateDisplay("Connecting to", WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED && attempts < kMaxConnectionAttempts)
    {
        delay(kRetryDelayMs);
        Serial.print('.');
        const String attemptsMessage = String("Attempt ") + String(attempts + 1) + String("/") + String(kMaxConnectionAttempts);
        updateDisplay("Connecting...", attemptsMessage);
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println();
        Serial.print("WiFi connected. IP address: ");
        Serial.println(WiFi.localIP());
        updateDisplay("WiFi connected", WiFi.localIP().toString());
    }
    else
    {
        Serial.println();
        Serial.println("Failed to connect to WiFi. Check credentials and signal strength.");
        updateDisplay("WiFi failed", "Check settings");
    }
}
}

void setup()
{
    Serial.begin(115200);
    while (!Serial) {}

    Wire.begin(kOledSdaPin, kOledSclPin);
    if (!display.begin(SSD1306_SWITCHCAPVCC, kOledI2cAddress))
    {
        Serial.println(F("SSD1306 allocation failed"));
        while (true)
        {
            delay(1000);
        }
    }

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.cp437(true);
    showStatus("Initializing...", "Starting WiFi");

    connectToWifi();

    display.clearDisplay();
    display.display();
}

void loop()
{
    // Placeholder loop. Future functionality (LED control, touchscreen handling, etc.)
    // will be implemented here.
    if (WiFi.status() != WL_CONNECTED)
    {
        connectToWifi();
        delay(1500);
    }



    // --- Keypad Input Handling ---
    char customKey = customKeypad.getKey();
    if (customKey) {
    if (customKey == 'A') {
      flameStatus = "Armed";
      armedTime = millis(); // Start the timer
    } else if (customKey == 'B') {
      flameStatus = "Defused";
    } else if (customKey == 'C') {
      flameStatus = "Exploded";
    } else {
      flameStatus = "On"; // Default case for other keys
    }
    
    Serial.print("Key Pressed: ");
    Serial.println(customKey);
    // Force immediate display update after a key press
    updateDisplay();
}   
  
  // --- Timer Update in Loop ---
  // Only update the display periodically and if status is "Armed" 
  // or if the timer just expired, to avoid screen flickering
  // Use a non-blocking check for the timer to count down
  if (flameStatus == "Armed" && (millis() - armedTime) / 1000 <= timerDuration) {
    // Update display every 100 milliseconds for a smoother countdown
    // This uses a "time since last update" logic (not included here for brevity, 
    // but the following line is sufficient for basic timer update)
    updateDisplay();
  } else if (flameStatus == "Exploded") {
    // If it exploded (either by key or timer), ensure the display updates once
    updateDisplay();
  }

  // A small delay to keep the loop from running too fast if no keys are pressed
  delay(50);
}
