/**
 * ESP-DashboardPlus - Access Point Example
 * 
 * This example demonstrates how to create a dashboard
 * with the ESP32 running as a WiFi Access Point.
 * No external router needed!
 * 
 * Connect to the ESP32's WiFi network, then open
 * http://192.168.4.1 in your browser.
 * 
 * Hardware: ESP32 (any variant)
 * 
 * Dependencies:
 *   - ArduinoJson (v7.0.0+)
 *   - ESPAsyncWebServer
 *   - AsyncTCP
 */

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.hpp>
#include "ESPDashboardPlus.h"
#include "dashboard_html.h"

// ============================================
// ACCESS POINT CONFIGURATION
// ============================================
const char* ap_ssid = "ESP32-Dashboard";     // Network name
const char* ap_password = "dashboard123";     // Min 8 characters (or empty for open)
const int ap_channel = 1;                     // WiFi channel (1-13)
const int ap_max_clients = 4;                 // Max simultaneous connections

// ============================================
// GLOBAL OBJECTS
// ============================================
AsyncWebServer server(80);
ESPDashboardPlus dashboard("ESP32 Access Point");

// Simulated values
float temperature = 25.0;
int connectedClients = 0;
bool ledState = false;

const int LED_PIN = 2;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\nESP-DashboardPlus Access Point Example");
    Serial.println("=======================================");
    
    pinMode(LED_PIN, OUTPUT);
    
    // ========================================
    // ACCESS POINT SETUP
    // ========================================
    WiFi.mode(WIFI_AP);
    
    // Start Access Point
    bool apStarted = WiFi.softAP(ap_ssid, ap_password, ap_channel, false, ap_max_clients);
    
    if (apStarted) {
        Serial.println("Access Point started successfully!");
        Serial.printf("  SSID: %s\n", ap_ssid);
        Serial.printf("  Password: %s\n", ap_password);
        Serial.printf("  IP Address: %s\n", WiFi.softAPIP().toString().c_str());
    } else {
        Serial.println("Failed to start Access Point!");
        while (true) delay(1000);
    }
    
    // ========================================
    // DASHBOARD INITIALIZATION
    // ========================================
    dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE, true, true);
    dashboard.setTitle("ESP32 Access Point", "Standalone Dashboard");
    dashboard.setVersionInfo("1.0.0", "Never");
    
    // Console command handler
    dashboard.onCommand([](const String& command) {
        if (command == "help") {
            dashboard.logInfo("Commands: help, status, clients, reboot");
        } else if (command == "status") {
            dashboard.logInfo("AP Mode - IP: " + WiFi.softAPIP().toString());
        } else if (command == "clients") {
            dashboard.logInfo("Connected clients: " + String(WiFi.softAPgetStationNum()));
        } else if (command == "reboot") {
            dashboard.logWarning("Rebooting...");
            delay(1000);
            ESP.restart();
        } else {
            dashboard.logWarning("Unknown: " + command);
        }
    });
    
    // ========================================
    // CREATE CARDS
    // ========================================
    
    // Network info group
    dashboard.addGroup("network", "Network Status");
    
    StatCard* ipCard = dashboard.addStatCard("ip", "AP IP Address", WiFi.softAPIP().toString(), "");
    ipCard->setVariant(CardVariant::PRIMARY);
    dashboard.addCardToGroup("network", "ip");
    
    StatCard* ssidCard = dashboard.addStatCard("ssid", "Network Name", ap_ssid, "");
    ssidCard->setVariant(CardVariant::INFO);
    dashboard.addCardToGroup("network", "ssid");
    
    StatCard* clientsCard = dashboard.addStatCard("clients", "Connected Clients", "0", "");
    clientsCard->setVariant(CardVariant::SUCCESS);
    dashboard.addCardToGroup("network", "clients");
    
    // Sensor group
    dashboard.addGroup("sensors", "Sensors");
    
    GaugeCard* tempGauge = dashboard.addGaugeCard("temp", "Temperature", 0, 50, "°C");
    tempGauge->setValue(temperature);
    tempGauge->setThresholds(30, 40);
    dashboard.addCardToGroup("sensors", "temp");
    
    // Controls group
    dashboard.addGroup("controls", "Controls");
    
    ToggleCard* ledToggle = dashboard.addToggleCard("led", "LED Control", "Onboard LED", ledState);
    ledToggle->onChange = [](bool value) {
        ledState = value;
        digitalWrite(LED_PIN, value ? HIGH : LOW);
        Serial.printf("LED: %s\n", value ? "ON" : "OFF");
    };
    dashboard.addCardToGroup("controls", "led");
    
    SliderCard* brightness = dashboard.addSliderCard("brightness", "LED Brightness", 0, 100, 10, "%");
    brightness->setValue(50);
    brightness->onChange = [](int value) {
        Serial.printf("Brightness: %d%%\n", value);
        // Add PWM control here if needed
    };
    dashboard.addCardToGroup("controls", "brightness");
    
    ActionButton* restartBtn = dashboard.addActionButton(
        "restart", "System", "Restart",
        "Restart Device?", "All clients will be disconnected.",
        []() {
            dashboard.logWarning("Restarting...");
            delay(500);
            ESP.restart();
        }
    );
    restartBtn->setVariant(CardVariant::WARNING);
    dashboard.addCardToGroup("controls", "restart");
    
    // ========================================
    // START SERVER
    // ========================================
    dashboard.logInfo("Access Point ready!");
    dashboard.logInfo("Connect to WiFi: " + String(ap_ssid));
    dashboard.logInfo("Open http://" + WiFi.softAPIP().toString());
    
    server.begin();
    
    Serial.println("\n========================================");
    Serial.println("Dashboard ready!");
    Serial.printf("1. Connect to WiFi: %s\n", ap_ssid);
    Serial.printf("2. Password: %s\n", ap_password);
    Serial.printf("3. Open http://%s\n", WiFi.softAPIP().toString().c_str());
    Serial.println("========================================\n");
}

void loop() {
    dashboard.loop();
    
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 2000) {
        lastUpdate = millis();
        
        // Update connected clients count
        int clients = WiFi.softAPgetStationNum();
        if (clients != connectedClients) {
            connectedClients = clients;
            dashboard.updateStatCard("clients", String(clients));
            Serial.printf("Clients connected: %d\n", clients);
        }
        
        // Simulate temperature
        temperature += random(-5, 6) / 10.0;
        temperature = constrain(temperature, 20.0, 45.0);
        dashboard.updateGaugeCard("temp", (int)temperature);
    }
    
    delay(10);
}
