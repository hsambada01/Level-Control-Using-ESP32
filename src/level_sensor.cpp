#include <Arduino.h>
#include <Preferences.h>

#include "app_config.h"
#include "level_sensor.h"

namespace {
Preferences levelPreferences;

constexpr char LEVEL_NAMESPACE[] = "levelcfg";
constexpr char KEY_EMPTY_RAW[] = "emptyRaw";
constexpr char KEY_FULL_RAW[] = "fullRaw";

int levelEmptyRaw = AppConfig::LEVEL_EMPTY_RAW_DEFAULT;
int levelFullRaw = AppConfig::LEVEL_FULL_RAW_DEFAULT;

// Membaca nilai mentah ADC dari pin sensor level.
int readLevelRaw() {
  return analogRead(AppConfig::LEVEL_SENSOR_PIN);
}

// Menjaga nilai kalibrasi tetap valid tanpa menghilangkan kemungkinan sensor terbalik.
void normalizeCalibration() {
  levelEmptyRaw = constrain(levelEmptyRaw, 0, 4095);
  levelFullRaw = constrain(levelFullRaw, 0, 4095);

  if (levelEmptyRaw == levelFullRaw) {
    if (levelFullRaw < 4095) {
      levelFullRaw += 1;
    } else {
      levelEmptyRaw -= 1;
    }
  }
}

// Menyimpan nilai kalibrasi sensor ke flash ESP32.
void saveLevelCalibration() {
  levelPreferences.putInt(KEY_EMPTY_RAW, levelEmptyRaw);
  levelPreferences.putInt(KEY_FULL_RAW, levelFullRaw);
}

// Memuat nilai kalibrasi sensor dari flash ESP32.
void loadLevelCalibration() {
  levelEmptyRaw = levelPreferences.getInt(KEY_EMPTY_RAW, AppConfig::LEVEL_EMPTY_RAW_DEFAULT);
  levelFullRaw = levelPreferences.getInt(KEY_FULL_RAW, AppConfig::LEVEL_FULL_RAW_DEFAULT);
  normalizeCalibration();
}
} // namespace

void initializeLevelSensor() {
  levelPreferences.begin(LEVEL_NAMESPACE, false);
  loadLevelCalibration();
  analogReadResolution(12);
  pinMode(AppConfig::LEVEL_SENSOR_PIN, INPUT);
}

LevelReading readLevel() {
  const int raw = readLevelRaw();
  const int percent = constrain(map(raw, levelEmptyRaw, levelFullRaw, 0, 100), 0, 100);
  return {raw, percent, levelEmptyRaw, levelFullRaw};
}

LevelCalibration getLevelCalibration() {
  return {levelEmptyRaw, levelFullRaw};
}

void setLevelCalibration(int emptyRaw, int fullRaw) {
  levelEmptyRaw = emptyRaw;
  levelFullRaw = fullRaw;
  normalizeCalibration();
  saveLevelCalibration();
}
