#pragma once

#include <Arduino.h>

namespace AppConfig {
// Kredensial Wi-Fi mode client. Ubah sesuai jaringan yang ingin dipakai.
inline constexpr char WIFI_SSID[] = "GANTI_DENGAN_SSID";
inline constexpr char WIFI_PASSWORD[] = "GANTI_DENGAN_PASSWORD";

// Kredensial Access Point cadangan saat koneksi ke router gagal.
inline constexpr char AP_SSID[] = "ESP32-WebServer";
inline constexpr char AP_PASSWORD[] = "12345678";

// Pin ADC untuk sensor level. Ubah sesuai wiring sensor pada board.
inline constexpr uint8_t LEVEL_SENSOR_PIN = 4;

// Nilai kalibrasi default sensor level untuk titik kosong dan penuh.
inline constexpr int LEVEL_EMPTY_RAW_DEFAULT = 0;
inline constexpr int LEVEL_FULL_RAW_DEFAULT = 4095;

// Pin output untuk relay pompa. Ubah sesuai wiring modul relay.
inline constexpr uint8_t RELAY_PIN = 5;

// Tipe logika relay. true untuk relay aktif-HIGH, false untuk relay aktif-LOW.
inline constexpr bool RELAY_ACTIVE_HIGH = true;

// Nilai awal pompa saat ESP32 pertama kali menyala.
inline constexpr bool RELAY_INITIAL_ON = false;

// Mode awal kontrol pompa. true untuk otomatis, false untuk manual.
inline constexpr bool RELAY_AUTO_MODE_INITIAL = false;

// Batas level saat pompa otomatis dinyalakan kembali.
inline constexpr int RELAY_AUTO_START_PERCENT = 30;

// Batas level saat pompa otomatis dimatikan.
inline constexpr int RELAY_AUTO_STOP_PERCENT = 80;

// Jeda minimum antar perpindahan relay untuk mencegah switching terlalu cepat.
inline constexpr unsigned long RELAY_MIN_SWITCH_INTERVAL_MS = 3000;

// Batas waktu mencoba koneksi ke router sebelum beralih ke mode AP.
inline constexpr unsigned long WIFI_TIMEOUT_MS = 15000;
} // namespace AppConfig
