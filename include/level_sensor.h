#pragma once

struct LevelReading {
  int raw;
  int percent;
  int emptyRaw;
  int fullRaw;
};

struct LevelCalibration {
  int emptyRaw;
  int fullRaw;
};

// Menyiapkan pin dan resolusi ADC untuk sensor level.
void initializeLevelSensor();

// Membaca nilai sensor level dalam bentuk raw ADC dan persentase.
LevelReading readLevel();

// Mengambil konfigurasi kalibrasi sensor level yang sedang aktif.
LevelCalibration getLevelCalibration();

// Mengubah nilai kalibrasi raw untuk kondisi kosong dan penuh.
void setLevelCalibration(int emptyRaw, int fullRaw);
