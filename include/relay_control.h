#pragma once

struct RelayStatus {
  bool enabled;
  int pin;
  bool autoMode;
  int autoStartPercent;
  int autoStopPercent;
};

// Menyiapkan pin relay dan menerapkan status awal pompa.
void initializeRelay();

// Mengubah status pompa menjadi aktif atau nonaktif.
void setPumpEnabled(bool enabled);

// Mengubah mode kontrol pompa antara manual dan otomatis.
void setPumpAutoMode(bool enabled);

// Mengatur batas start dan stop untuk mode otomatis.
void setPumpAutoThresholds(int startPercent, int stopPercent);

// Memproses logika otomatis berdasarkan level air terbaru.
void updatePumpControl(int levelPercent);

// Mengambil status relay pompa saat ini.
RelayStatus getRelayStatus();