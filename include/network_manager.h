#pragma once

#include <Arduino.h>

struct NetworkStatus {
  bool apMode;
  String ipAddress;
  String ssid;
};

// Menyiapkan koneksi jaringan. Akan memakai Wi-Fi client lalu fallback ke AP.
void initializeNetwork();

// Mengambil status jaringan aktif untuk ditampilkan atau dikirim sebagai JSON.
NetworkStatus getNetworkStatus();
