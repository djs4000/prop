#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
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

byte rowPins[COLS] = {12, 14, 27, 26}; 
byte colPins[ROWS] = {25, 33, 32, 18}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Global variables for status and timer
String flameStatus = "On";
unsigned long armedTime = 0; // To store when the 'Armed' state began
const int timerDuration = 10; // 10 seconds


constexpr uint8_t kMaxConnectionAttempts = 20;
constexpr uint16_t kRetryDelayMs = 500;

Adafruit_SSD1306 display(kScreenWidth, kScreenHeight, &Wire, kOledResetPin);

Preferences preferences;
WebServer server(80);

String endpointConfig;

String htmlEscape(const String &input)
{
    String escaped;
    escaped.reserve(input.length());
    for (uint16_t i = 0; i < input.length(); ++i)
    {
        const char c = input.charAt(i);
        switch (c)
        {
        case '&':
            escaped += F("&amp;");
            break;
        case '<':
            escaped += F("&lt;");
            break;
        case '>':
            escaped += F("&gt;");
            break;
        case 39:
            escaped += F("&#39;");
            break;
        case '"':
            escaped += F("&quot;");
            break;
        default:
            escaped += c;
            break;
        }
    }
    return escaped;
}

void sendConfigurationPage(const String &message = String())
{
    String html = F("<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>Device Configuration</title><style>body{font-family:Arial,sans-serif;margin:40px;}label{display:block;margin-bottom:8px;font-weight:bold;}input[type=text]{width:100%;max-width:480px;padding:8px;margin-bottom:12px;}button{padding:10px 16px;margin-right:10px;font-size:14px;cursor:pointer;}form{margin-bottom:20px;}p.status{color:green;}</style></head><body><h1>Device Configuration</h1>");
    if (message.length() > 0)
    {
        html += F("<p class='status'>");
        html += htmlEscape(message);
        html += F("</p>");
    }
    html += F("<form method='POST' action='/save'><label for='endpoint'>Endpoint URL</label><input type='text' id='endpoint' name='endpoint' value='");
    html += htmlEscape(endpointConfig);
    html += F("' placeholder='https://example.com/api'><button type='submit'>Save</button></form><form method='POST' action='/restart'><button type='submit'>Restart Device</button></form></body></html>");
    server.send(200, "text/html", html);
}

void handleRoot()
{
    sendConfigurationPage();
}

void handleSave()
{
    if (server.hasArg("endpoint"))
    {
        endpointConfig = server.arg("endpoint");
        preferences.putString("endpoint", endpointConfig);
        sendConfigurationPage(F("Configuration saved."));
    }
    else
    {
        server.send(400, "text/plain", "Missing endpoint parameter");
    }
}

void handleRestart()
{
    server.send(200, "text/html", F("<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Restarting</title></head><body><h1>Device Restarting</h1><p>The device will restart to apply the new settings.</p></body></html>"));
    delay(500);
    ESP.restart();
}

void setupServer()
{
    server.on("/", HTTP_GET, handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.on("/restart", HTTP_POST, handleRestart);
    server.begin();
}

/*void showStatus(const String &line1, const String &line2 = String())
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(line1);
    if (line2.length() > 0)
    {
        display.println(line2);
    }
    display.display();
}*/


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

    //display wifi status


    display.setCursor(0, 15);
    display.println(line1);
    if (line2.length() > 0)
    {
        display.println(line2);
    }




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

    preferences.begin("config", false);
    endpointConfig = preferences.getString("endpoint", String());

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
    updateDisplay("Initializing...", "Starting WiFi");

    connectToWifi();

    //display.clearDisplay();
    display.display();

    setupServer();
}

void loop()
{
    server.handleClient();

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
  delay(100);
}
