#include <Arduino.h>
#include <Preferences.h>

#include "app_config.h"
#include "relay_control.h"

namespace {
Preferences relayPreferences;

constexpr char RELAY_NAMESPACE[] = "relaycfg";
constexpr char KEY_ENABLED[] = "enabled";
constexpr char KEY_AUTO_MODE[] = "autoMode";
constexpr char KEY_AUTO_START[] = "autoStart";
constexpr char KEY_AUTO_STOP[] = "autoStop";

bool pumpEnabled = AppConfig::RELAY_INITIAL_ON;
bool pumpAutoMode = AppConfig::RELAY_AUTO_MODE_INITIAL;
int pumpAutoStartPercent = AppConfig::RELAY_AUTO_START_PERCENT;
int pumpAutoStopPercent = AppConfig::RELAY_AUTO_STOP_PERCENT;
unsigned long lastRelaySwitchMs = 0;

// Mengubah status logika pin fisik mengikuti tipe relay aktif-HIGH atau aktif-LOW.
void writeRelayPin(bool enabled) {
  const uint8_t outputLevel = (enabled == AppConfig::RELAY_ACTIVE_HIGH) ? HIGH : LOW;
  digitalWrite(AppConfig::RELAY_PIN, outputLevel);
}

// Mengecek apakah relay sudah boleh berpindah lagi berdasarkan jeda minimum.
bool canSwitchRelay() {
  return millis() - lastRelaySwitchMs >= AppConfig::RELAY_MIN_SWITCH_INTERVAL_MS;
}

// Menyimpan seluruh status relay ke flash ESP32 agar bertahan setelah restart.
void saveRelaySettings() {
  relayPreferences.putBool(KEY_ENABLED, pumpEnabled);
  relayPreferences.putBool(KEY_AUTO_MODE, pumpAutoMode);
  relayPreferences.putInt(KEY_AUTO_START, pumpAutoStartPercent);
  relayPreferences.putInt(KEY_AUTO_STOP, pumpAutoStopPercent);
}

// Menjaga nilai threshold tetap valid dan membentuk hysteresis yang benar.
void normalizeThresholds() {
  pumpAutoStartPercent = constrain(pumpAutoStartPercent, 0, 100);
  pumpAutoStopPercent = constrain(pumpAutoStopPercent, 0, 100);

  if (pumpAutoStartPercent > pumpAutoStopPercent) {
    const int temp = pumpAutoStartPercent;
    pumpAutoStartPercent = pumpAutoStopPercent;
    pumpAutoStopPercent = temp;
  }
}

// Memuat status relay terakhir dari flash jika data pernah disimpan sebelumnya.
void loadRelaySettings() {
  pumpEnabled = relayPreferences.getBool(KEY_ENABLED, AppConfig::RELAY_INITIAL_ON);
  pumpAutoMode = relayPreferences.getBool(KEY_AUTO_MODE, AppConfig::RELAY_AUTO_MODE_INITIAL);
  pumpAutoStartPercent = relayPreferences.getInt(KEY_AUTO_START, AppConfig::RELAY_AUTO_START_PERCENT);
  pumpAutoStopPercent = relayPreferences.getInt(KEY_AUTO_STOP, AppConfig::RELAY_AUTO_STOP_PERCENT);
  normalizeThresholds();
}
} // namespace

void initializeRelay() {
  relayPreferences.begin(RELAY_NAMESPACE, false);
  loadRelaySettings();
  pinMode(AppConfig::RELAY_PIN, OUTPUT);
  writeRelayPin(pumpEnabled);
  lastRelaySwitchMs = millis();
}

void setPumpEnabled(bool enabled) {
  if (pumpEnabled == enabled) {
    return;
  }

  if (!canSwitchRelay()) {
    return;
  }

  pumpEnabled = enabled;
  writeRelayPin(pumpEnabled);
  lastRelaySwitchMs = millis();
  saveRelaySettings();
}

void setPumpAutoMode(bool enabled) {
  pumpAutoMode = enabled;
  saveRelaySettings();
}

void setPumpAutoThresholds(int startPercent, int stopPercent) {
  pumpAutoStartPercent = startPercent;
  pumpAutoStopPercent = stopPercent;
  normalizeThresholds();
  saveRelaySettings();
}

void updatePumpControl(int levelPercent) {
  if (!pumpAutoMode) {
    return;
  }

  const int constrainedLevel = constrain(levelPercent, 0, 100);

  if (constrainedLevel <= pumpAutoStartPercent) {
    setPumpEnabled(true);
    return;
  }

  if (constrainedLevel >= pumpAutoStopPercent) {
    setPumpEnabled(false);
  }
}

RelayStatus getRelayStatus() {
  return {pumpEnabled, AppConfig::RELAY_PIN, pumpAutoMode, pumpAutoStartPercent, pumpAutoStopPercent};
}