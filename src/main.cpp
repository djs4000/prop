#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_SSD1306.h>

#include "wifi_config.h"

namespace
{
constexpr uint8_t kScreenWidth = 128;
constexpr uint8_t kScreenHeight = 32;
constexpr int8_t kOledResetPin = -1;
constexpr uint8_t kOledI2cAddress = 0x3C;
constexpr int kOledSdaPin = 21;
constexpr int kOledSclPin = 22;

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

void connectToWifi()
{
    Serial.printf("Connecting to %s", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    uint8_t attempts = 0;
    showStatus("Connecting to", WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED && attempts < kMaxConnectionAttempts)
    {
        delay(kRetryDelayMs);
        Serial.print('.');
        const String attemptsMessage = String("Attempt ") + String(attempts + 1) + String("/") + String(kMaxConnectionAttempts);
        showStatus("Connecting...", attemptsMessage);
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println();
        Serial.print("WiFi connected. IP address: ");
        Serial.println(WiFi.localIP());
        showStatus("WiFi connected", WiFi.localIP().toString());
    }
    else
    {
        Serial.println();
        Serial.println("Failed to connect to WiFi. Check credentials and signal strength.");
        showStatus("WiFi failed", "Check settings");
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
}

void loop()
{
    // Placeholder loop. Future functionality (LED control, touchscreen handling, etc.)
    // will be implemented here.
    if (WiFi.status() != WL_CONNECTED)
    {
        connectToWifi();
    }

    delay(1000);
}
