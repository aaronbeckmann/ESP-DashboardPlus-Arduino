# ESP-DashboardPlus - Arduino IDE Library

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/en/products/socs/esp32)

A real-time, on-device web dashboard library for ESP32 microcontrollers. Create beautiful, responsive dashboards with minimal code using WebSocket-based communication.

## ✨ Features

- **🔌 Real-time Updates** - WebSocket-based communication for instant UI updates
- **📊 16 Card Types** - Stats, charts, gauges, toggles, sliders, buttons, time/location pickers, and more
- **🎨 Modern Dark & Light Theme** - Industrial IoT aesthetic with theme switching
- **📱 Responsive Design** - Works on desktop and mobile browsers
- **🗂️ Tabbed Interface** - Dashboard, Console, and OTA tabs
- **🔄 OTA Updates** - Firmware update tab with drag-and-drop
- **📝 Console Logging** - Real-time console with filtering and export
- **⚡ Optimized** - Pre-compressed HTML stored in PROGMEM

## 📦 Installation

### Arduino IDE Library Manager

1. Open Arduino IDE
2. Go to **Sketch > Include Library > Manage Libraries...**
3. Search for "ESP-DashboardPlus"
4. Click **Install**

### Manual Installation

1. Download the latest release ZIP from [GitHub](https://github.com/aaronbeckmann/ESP-DashboardPlus-Arduino/releases)
2. Open Arduino IDE
3. Go to **Sketch > Include Library > Add .ZIP Library...**
4. Select the downloaded ZIP file

For OTA: a compatible partition scheme must be chosen.

### Dependencies

Install these libraries from the Library Manager:
- [**ArduinoJson**](https://docs.arduino.cc/libraries/arduinojson/) (by Benoit Blanchon)
- [**ESPAsyncWebServer**](https://docs.arduino.cc/libraries/esp-async-webserver/) (by ESP32Async)
- [**AsyncTCP**](https://docs.arduino.cc/libraries/esp-async-tcp/) (by ESP32Async)

If any JSON related errors pop up: Click **Sketch > Include Library > ArduinoJSON**

## 🚀 Quick Start

```cpp
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "ESPDashboardPlus.h"
#include "dashboard_html.h"

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

AsyncWebServer server(80);
ESPDashboardPlus dashboard("My Device");

void setup() {
    Serial.begin(115200);
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);
    Serial.println(WiFi.localIP());
    
    // Initialize dashboard with pre-compressed HTML
    dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE);
    
    // Add a stat card
    dashboard.addStatCard("temp", "Temperature", "23.5", "°C");
    
    // Add a toggle with callback
    ToggleCard* led = dashboard.addToggleCard("led", "LED", "Status", false);
    led->onChange = [](bool state) {
        digitalWrite(LED_BUILTIN, state);
    };
    
    server.begin();
}

void loop() {
    dashboard.loop();
    
    // Update values periodically
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 2000) {
        lastUpdate = millis();
        dashboard.updateStatCard("temp", String(random(20, 30)));
    }
}
```

## 📊 Available Card Types

| Card | Description |
|------|-------------|
| **StatCard** | Display numeric values with units |
| **GaugeCard** | Circular gauge with thresholds |
| **ChartCard** | Line/area/bar/scatter/step charts |
| **ToggleCard** | On/off switch |
| **SliderCard** | Range slider |
| **ButtonCard** | Simple clickable button |
| **ActionButton** | Button with confirmation popup |
| **LinkCard** | URL redirect button |
| **InputCard** | Text/number input field |
| **DropdownCard** | Select menu |
| **ColorPickerCard** | Color picker with presets |
| **DateCard** | Date/time picker |
| **TimeCard** | Time picker |
| **TimezoneCard** | Browser timezone detection |
| **LocationCard** | GPS/browser geolocation |
| **StatusCard** | Icon + status message |

## 📖 Documentation

Full documentation: **[https://aaronbeckmann.github.io/ESP-DashboardPlus](https://aaronbeckmann.github.io/ESP-DashboardPlus)**

## 📄 License

MIT License - see [LICENSE](LICENSE) file.
