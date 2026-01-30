/**
 * ESP-DashboardPlus - Basic Example
 * 
 * This example demonstrates how to create a simple dashboard
 * with various card types for the Arduino IDE.
 * 
 * Hardware: ESP32 (any variant)
 * 
 * Dependencies:
 *   - ArduinoJson (v7.0.0+)
 *   - ESPAsyncWebServer
 *   - AsyncTCP
 * 
 * For more examples, see:
 *   https://aaronbeckmann.github.io/ESP-DashboardPlus/examples
 */

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "ESPDashboardPlus.h"
#include "dashboard_html.h"  // Pre-compressed HTML

// ============================================
// CONFIGURATION - Update these values!
// ============================================
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ============================================
// GLOBAL OBJECTS
// ============================================
AsyncWebServer server(80);
ESPDashboardPlus dashboard("My ESP32 Device");

// Simulated sensor values
float temperature = 23.5;
float humidity = 45.0;
int cpuUsage = 30;
bool ledState = false;
int ledBrightness = 75;

// Pin definitions
const int LED_PIN = 2;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\nESP-DashboardPlus Basic Example");
    Serial.println("================================");
    
    // Initialize LED
    pinMode(LED_PIN, OUTPUT);
    
    // ========================================
    // WIFI CONNECTION
    // ========================================
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
    
    // ========================================
    // DASHBOARD INITIALIZATION
    // ========================================
    // Parameters: server, htmlData, htmlSize, enableOTA, enableConsole
    dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE, true, true);
    
    // Set dashboard title and subtitle (displayed in browser)
    dashboard.setTitle("My ESP32 Device", "Home Automation Hub");
    
    // Set firmware version info (displayed in OTA tab)
    dashboard.setVersionInfo("1.0.0", "Never");
    
    // Set global command handler (for Console tab input)
    dashboard.onCommand([](const String& command) {
        Serial.printf("Console command: %s\n", command.c_str());
        
        if (command == "help") {
            dashboard.logInfo("Commands: help, status, reboot");
        } else if (command == "status") {
            dashboard.logInfo("System OK - Temp: " + String(temperature, 1) + "°C");
        } else if (command == "reboot") {
            dashboard.logWarning("Rebooting in 3 seconds...");
            delay(3000);
            ESP.restart();
        } else {
            dashboard.logWarning("Unknown command: " + command);
        }
    });
    
    // ========================================
    // CREATE CARD GROUPS
    // ========================================
    dashboard.addGroup("sensors", "Sensor Readings");
    dashboard.addGroup("controls", "Device Controls");
    
    // ========================================
    // STAT CARDS
    // ========================================
    StatCard* tempCard = dashboard.addStatCard("temp", "Temperature", String(temperature, 1), "°C");
    tempCard->setVariant(CardVariant::PRIMARY);
    tempCard->setTrend("up", "+0.5°C");
    dashboard.addCardToGroup("sensors", "temp");
    
    StatCard* humidCard = dashboard.addStatCard("humidity", "Humidity", String(humidity, 0), "%");
    humidCard->setVariant(CardVariant::INFO);
    dashboard.addCardToGroup("sensors", "humidity");
    
    // ========================================
    // GAUGE CARD
    // ========================================
    GaugeCard* cpuGauge = dashboard.addGaugeCard("cpu", "CPU Usage", 0, 100, "%");
    cpuGauge->setValue(cpuUsage);
    cpuGauge->setThresholds(60, 85);
    dashboard.addCardToGroup("sensors", "cpu");
    
    // ========================================
    // CHART CARD
    // ========================================
    ChartCard* tempChart = dashboard.addChartCard("temp-chart", "Temperature History", ChartType::LINE, 20);
    int tempSeriesIdx = tempChart->addSeries("Temperature", "primary");
    tempChart->setSize(2, 1);  // Span 2 columns
    dashboard.addCardToGroup("sensors", "temp-chart");
    
    // ========================================
    // TOGGLE CARD
    // ========================================
    ToggleCard* ledToggle = dashboard.addToggleCard("led-toggle", "LED Control", "Onboard LED", ledState);
    ledToggle->onChange = [](bool value) {
        ledState = value;
        digitalWrite(LED_PIN, value ? HIGH : LOW);
        Serial.printf("LED: %s\n", value ? "ON" : "OFF");
    };
    dashboard.addCardToGroup("controls", "led-toggle");
    
    // ========================================
    // SLIDER CARD
    // ========================================
    SliderCard* brightnessSlider = dashboard.addSliderCard("brightness", "LED Brightness", 0, 100, 5, "%");
    brightnessSlider->setValue(ledBrightness);
    brightnessSlider->onChange = [](int value) {
        ledBrightness = value;
        Serial.printf("Brightness: %d%%\n", value);
    };
    dashboard.addCardToGroup("controls", "brightness");
    
    // ========================================
    // BUTTON CARD
    // ========================================
    ButtonCardImpl* saveBtn = dashboard.addButtonCard("save", "Settings", "Save", []() {
        Serial.println("Save button clicked!");
        dashboard.logInfo("Settings saved successfully!");
    });
    saveBtn->setVariant(CardVariant::PRIMARY);
    dashboard.addCardToGroup("controls", "save");
    
    // ========================================
    // ACTION BUTTON (with confirmation)
    // ========================================
    ActionButton* restartBtn = dashboard.addActionButton(
        "restart", 
        "System", 
        "Restart Device",
        "Restart Device?",
        "The device will restart and temporarily lose connection.",
        []() {
            Serial.println("Restarting...");
            dashboard.logWarning("Device restarting...");
            delay(1000);
            ESP.restart();
        }
    );
    restartBtn->setVariant(CardVariant::WARNING);
    
    // ========================================
    // LOG INITIAL MESSAGE
    // ========================================
    dashboard.logInfo("ESP-DashboardPlus initialized successfully!");
    
    // ========================================
    // START SERVER
    // ========================================
    server.begin();
    Serial.println("Dashboard ready!");
    Serial.printf("Open http://%s in your browser\n", WiFi.localIP().toString().c_str());
}

void loop() {
    // Process WebSocket events
    dashboard.loop();
    
    // Simulate sensor readings every 2 seconds
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 2000) {
        lastUpdate = millis();
        
        // Simulate temperature fluctuation
        temperature += random(-10, 11) / 10.0;
        temperature = constrain(temperature, 15.0, 35.0);
        
        // Simulate humidity fluctuation  
        humidity += random(-5, 6);
        humidity = constrain(humidity, 30.0, 70.0);
        
        // Simulate CPU usage
        cpuUsage += random(-10, 11);
        cpuUsage = constrain(cpuUsage, 10, 90);
        
        // Update dashboard cards
        dashboard.updateStatCard("temp", String(temperature, 1));
        dashboard.updateStatCard("humidity", String(humidity, 0));
        dashboard.updateGaugeCard("cpu", cpuUsage);
        dashboard.updateChartCard("temp-chart", 0, temperature);
        
        Serial.printf("Temp: %.1f°C, Humidity: %.0f%%, CPU: %d%%\n", 
                      temperature, humidity, cpuUsage);
    }
    
    delay(10);
}
