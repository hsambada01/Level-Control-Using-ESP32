#include <Arduino.h>

#include "level_sensor.h"
#include "network_manager.h"
#include "relay_control.h"
#include "web_app.h"

/*
  Program: ESP32 Level Control Web Server

  Arsitektur program:
  - main.cpp          : bootstrap aplikasi
  - level_sensor.cpp  : inisialisasi dan pembacaan sensor level
  - network_manager.cpp : koneksi Wi-Fi dan fallback Access Point
  - relay_control.cpp : kontrol output relay untuk pompa
  - web_app.cpp       : route HTTP, halaman dashboard, dan respons JSON

  Tujuan pemisahan ini adalah membuat setiap modul memiliki tanggung jawab yang jelas
  sehingga program lebih mudah dibaca, diuji, dan dikembangkan.
*/

// Inisialisasi perangkat: serial, sensor level, jaringan, lalu web server.
void setup() {
  Serial.begin(115200);
  delay(1000);

  initializeLevelSensor();
  initializeRelay();
  initializeNetwork();
  initializeWebApp();
}

// Loop utama hanya memproses request HTTP dari client.
void loop() {
  const LevelReading level = readLevel();
  updatePumpControl(level.percent);
  handleWebAppClient();
}