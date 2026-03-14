#include <Arduino.h>
#include <WiFi.h>

#include "app_config.h"
#include "network_manager.h"

namespace {
bool currentApMode = false;

// Mengaktifkan Access Point lokal agar perangkat tetap bisa diakses tanpa router.
void startAccessPoint() {
  currentApMode = true;
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AppConfig::AP_SSID, AppConfig::AP_PASSWORD);

  Serial.println("Access point aktif.");
  Serial.print("SSID AP: ");
  Serial.println(AppConfig::AP_SSID);
  Serial.print("Password AP: ");
  Serial.println(AppConfig::AP_PASSWORD);
  Serial.print("IP AP: ");
  Serial.println(WiFi.softAPIP());
}

// Mencoba menghubungkan ESP32 ke router Wi-Fi menggunakan mode station.
bool connectToWifi() {
  if (String(AppConfig::WIFI_SSID).isEmpty() || String(AppConfig::WIFI_SSID) == "GANTI_DENGAN_SSID") {
    Serial.println("SSID belum diisi. Masuk ke mode access point.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(AppConfig::WIFI_SSID, AppConfig::WIFI_PASSWORD);

  Serial.print("Menghubungkan ke Wi-Fi");
  const unsigned long startAttempt = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < AppConfig::WIFI_TIMEOUT_MS) {
    delay(500);
    Serial.print('.');
  }

  if (WiFi.status() == WL_CONNECTED) {
    currentApMode = false;
    Serial.println();
    Serial.println("Wi-Fi terhubung.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  }

  Serial.println();
  Serial.println("Gagal terhubung ke Wi-Fi.");
  return false;
}
} // namespace

void initializeNetwork() {
  if (!connectToWifi()) {
    startAccessPoint();
  }
}

NetworkStatus getNetworkStatus() {
  NetworkStatus status;
  status.apMode = currentApMode;
  status.ipAddress = currentApMode ? WiFi.softAPIP().toString() : WiFi.localIP().toString();
  status.ssid = currentApMode ? String(AppConfig::AP_SSID) : WiFi.SSID();
  return status;
}
