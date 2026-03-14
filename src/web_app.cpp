#include <Arduino.h>
#include <WebServer.h>

#include "app_config.h"
#include "level_sensor.h"
#include "network_manager.h"
#include "relay_control.h"
#include "web_app.h"

namespace {
WebServer server(80);

// Membangun halaman HTML dashboard yang menampilkan status perangkat secara live.
String buildHtmlPage() {
  const NetworkStatus status = getNetworkStatus();
  const LevelReading level = readLevel();
  const RelayStatus relay = getRelayStatus();

  String html;
  html.reserve(9800);
  html += F("<!DOCTYPE html><html lang='id'><head><meta charset='UTF-8'>");
  html += F("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  html += F("<title>ESP32 Web Server</title>");
  html += F("<style>");
  html += F(":root{--bg:#eef6f7;--card:#ffffff;--ink:#1d2733;--accent:#0f5c7a;--soft:#dff3ff;}");
  html += F("body{margin:0;font-family:Arial,sans-serif;background:radial-gradient(circle at top,#ffffff 0,#eef6f7 55%,#ddebed 100%);color:#1d2733;}");
  html += F("main{max-width:720px;margin:40px auto;padding:24px;}");
  html += F(".card{background:#fff;border-radius:16px;padding:24px;box-shadow:0 12px 30px rgba(0,0,0,.08);} ");
  html += F("h1{margin-top:0;color:#0f5c7a;}p,li{line-height:1.6;}code{background:#eef3f8;padding:2px 6px;border-radius:6px;}");
  html += F(".badge{display:inline-block;padding:6px 12px;border-radius:999px;background:#dff3ff;color:#0f5c7a;font-weight:700;}");
  html += F(".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(220px,1fr));gap:16px;margin:20px 0;} .panel{padding:18px;border-radius:14px;background:#f8fbfc;border:1px solid #e3edf0;}");
  html += F(".value{font-size:2rem;font-weight:700;color:#0f5c7a;} .muted{color:#52606d;font-size:.95rem;} .bar{height:14px;background:#dce8eb;border-radius:999px;overflow:hidden;margin-top:10px;}");
  html += F(".actions{display:flex;gap:10px;flex-wrap:wrap;margin-top:14px;} button{border:none;border-radius:12px;padding:12px 16px;font-weight:700;cursor:pointer;transition:transform .2s ease,opacity .2s ease;} button:hover{transform:translateY(-1px);} .btnOn{background:#0f7a5c;color:#fff;} .btnOff{background:#c44f31;color:#fff;} .btnSoft{background:#0f5c7a;color:#fff;} .stateOn{color:#0f7a5c;} .stateOff{color:#c44f31;} .stateAuto{color:#0f5c7a;}");
  html += F("label{display:block;margin-top:12px;font-size:.92rem;color:#52606d;} input{margin-top:6px;width:100%;box-sizing:border-box;padding:10px 12px;border-radius:10px;border:1px solid #cfdde2;background:#fff;}");
  html += F(".fill{height:100%;width:0;background:linear-gradient(90deg,#0f5c7a,#38a3c7);transition:width .3s ease;} @media (max-width:640px){main{margin:16px auto;padding:16px;}} ");
  html += F("</style></head><body><main><section class='card'>");
  html += F("<span class='badge'>ESP32 Web Server Aktif</span>");
  html += F("<h1>Status Perangkat</h1>");
  html += F("<div class='grid'>");
  html += F("<div class='panel'><div class='muted'>Level Air</div><div class='value'><span id='levelPercent'>");
  html += String(level.percent);
  html += F("</span>%</div><div class='muted'>ADC raw: <span id='levelRaw'>");
  html += String(level.raw);
  html += F("</span></div><div class='muted'>Kalibrasi kosong: <span id='levelEmptyRaw'>");
  html += String(level.emptyRaw);
  html += F("</span> | penuh: <span id='levelFullRaw'>");
  html += String(level.fullRaw);
  html += F("</span></div><div class='bar'><div id='levelBar' class='fill' style='width:");
  html += String(level.percent);
  html += F("%'></div></div><label>Raw saat kosong<input id='levelEmptyInput' type='number' min='0' max='4095' value='");
  html += String(level.emptyRaw);
  html += F("'></label><label>Raw saat penuh<input id='levelFullInput' type='number' min='0' max='4095' value='");
  html += String(level.fullRaw);
  html += F("'></label><div class='actions'><button class='btnSoft' type='button' onclick='saveLevelCalibration()'>Simpan Kalibrasi</button><button class='btnSoft' type='button' onclick='useCurrentForEmpty()'>Set Kosong = Raw Sekarang</button><button class='btnSoft' type='button' onclick='useCurrentForFull()'>Set Penuh = Raw Sekarang</button></div></div>");
  html += F("<div class='panel'><div class='muted'>Status Server</div><div class='value' id='networkMode'>");
  html += (status.apMode ? F("Access Point") : F("Wi-Fi"));
  html += F("</div><div class='muted'>IP: <span id='ipAddress'>");
  html += status.ipAddress;
  html += F("</span></div><div class='muted'>SSID: <span id='activeSsid'>");
  html += status.ssid;
  html += F("</span></div></div>");
  html += F("<div class='panel'><div class='muted'>Kontrol Pompa</div><div id='relayState' class='value ");
  html += (relay.enabled ? F("stateOn'>HIDUP") : F("stateOff'>MATI"));
  html += F("</div><div class='muted'>Relay pin: <span id='relayPin'>");
  html += String(relay.pin);
  html += F("</span></div><div class='muted'>Mode: <span id='relayMode' class='");
  html += (relay.autoMode ? F("stateAuto'>AUTO") : F("stateOff'>MANUAL"));
  html += F("</span></div><div class='muted'>Default boot: ");
  html += (AppConfig::RELAY_INITIAL_ON ? F("ON") : F("OFF"));
  html += F("</div><div class='muted'>Auto start: <span id='autoStartValue'>");
  html += String(relay.autoStartPercent);
  html += F("</span>% | Auto stop: <span id='autoStopValue'>");
  html += String(relay.autoStopPercent);
  html += F("</span>%</div><div class='actions'><button class='btnSoft' type='button' onclick='setPumpMode(false)'>Mode Manual</button><button class='btnSoft' type='button' onclick='setPumpMode(true)'>Mode Auto</button><button class='btnOn' type='button' onclick='setPump(true)'>Pompa ON</button><button class='btnOff' type='button' onclick='setPump(false)'>Pompa OFF</button></div><label>Start pompa saat level <= (%)<input id='autoStartInput' type='number' min='0' max='100' value='");
  html += String(relay.autoStartPercent);
  html += F("'></label><label>Stop pompa saat level >= (%)<input id='autoStopInput' type='number' min='0' max='100' value='");
  html += String(relay.autoStopPercent);
  html += F("'></label><div class='actions'><button class='btnSoft' type='button' onclick='saveAutoThresholds()'>Simpan Batas Auto</button></div></div>");
  html += F("</div>");
  html += F("<ul>");
  html += F("<li>Mode jaringan: <strong>");
  html += (status.apMode ? F("Access Point") : F("Wi-Fi Client"));
  html += F("</strong></li>");
  html += F("<li>IP address: <code>");
  html += status.ipAddress;
  html += F("</code></li>");
  html += F("<li>SSID aktif: <code>");
  html += status.ssid;
  html += F("</code></li>");
  html += F("<li>Pin sensor level: <code>");
  html += String(AppConfig::LEVEL_SENSOR_PIN);
  html += F("</code></li>");
  html += F("<li>Pin relay pompa: <code>");
  html += String(AppConfig::RELAY_PIN);
  html += F("</code></li>");
  html += F("<li>Status awal pompa: <code>");
  html += (AppConfig::RELAY_INITIAL_ON ? F("ON") : F("OFF"));
  html += F("</code></li>");
  html += F("<li>Mode awal pompa: <code>");
  html += (AppConfig::RELAY_AUTO_MODE_INITIAL ? F("AUTO") : F("MANUAL"));
  html += F("</code></li>");
  html += F("<li>Uptime: <code>");
  html += String(millis() / 1000);
  html += F(" detik</code></li>");
  html += F("</ul>");
  html += F("<p>Endpoint JSON tersedia di <code>/status</code>, <code>/level</code>, <code>/level/config</code>, <code>/relay</code>, dan <code>/relay/config</code>.</p>");
  html += F("<script>");
  html += F("async function parseJsonResponse(response){const data=await response.json();if(!response.ok){throw new Error(data.error||'request gagal');}return data;}function updateRelayView(relay){const relayPin=document.getElementById('relayPin');const relayState=document.getElementById('relayState');const relayMode=document.getElementById('relayMode');document.getElementById('autoStartValue').textContent=relay.auto_start_percent;document.getElementById('autoStopValue').textContent=relay.auto_stop_percent;document.getElementById('autoStartInput').value=relay.auto_start_percent;document.getElementById('autoStopInput').value=relay.auto_stop_percent;relayPin.textContent=relay.pin;relayState.textContent=relay.enabled?'HIDUP':'MATI';relayState.className='value '+(relay.enabled?'stateOn':'stateOff');relayMode.textContent=relay.auto_mode?'AUTO':'MANUAL';relayMode.className=relay.auto_mode?'stateAuto':'stateOff';}function updateLevelView(level){document.getElementById('levelRaw').textContent=level.raw;document.getElementById('levelPercent').textContent=level.percent;document.getElementById('levelBar').style.width=level.percent+'%';document.getElementById('levelEmptyRaw').textContent=level.empty_raw;document.getElementById('levelFullRaw').textContent=level.full_raw;document.getElementById('levelEmptyInput').value=level.empty_raw;document.getElementById('levelFullInput').value=level.full_raw;}async function refreshData(){try{const [statusResponse,levelResponse,relayResponse]=await Promise.all([fetch('/status'),fetch('/level'),fetch('/relay')]);const status=await statusResponse.json();const level=await parseJsonResponse(levelResponse);const relay=await parseJsonResponse(relayResponse);document.getElementById('networkMode').textContent=status.mode==='ap'?'Access Point':'Wi-Fi';document.getElementById('ipAddress').textContent=status.ip;document.getElementById('activeSsid').textContent=status.ssid;updateLevelView(level);updateRelayView(relay);}catch(error){console.log(error);}}async function setPump(enabled){try{const response=await fetch('/relay/set',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'enabled='+(enabled?'1':'0')});const relay=await parseJsonResponse(response);updateRelayView(relay);}catch(error){console.log(error);}}async function setPumpMode(autoMode){try{const response=await fetch('/relay/config',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'auto_mode='+(autoMode?'1':'0')});const relay=await parseJsonResponse(response);updateRelayView(relay);}catch(error){console.log(error);}}async function saveAutoThresholds(){try{const start=document.getElementById('autoStartInput').value;const stop=document.getElementById('autoStopInput').value;const response=await fetch('/relay/config',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'auto_start_percent='+encodeURIComponent(start)+'&auto_stop_percent='+encodeURIComponent(stop)});const relay=await parseJsonResponse(response);updateRelayView(relay);}catch(error){console.log(error);}}function useCurrentForEmpty(){document.getElementById('levelEmptyInput').value=document.getElementById('levelRaw').textContent;}function useCurrentForFull(){document.getElementById('levelFullInput').value=document.getElementById('levelRaw').textContent;}async function saveLevelCalibration(){try{const emptyRaw=document.getElementById('levelEmptyInput').value;const fullRaw=document.getElementById('levelFullInput').value;const response=await fetch('/level/config',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'empty_raw='+encodeURIComponent(emptyRaw)+'&full_raw='+encodeURIComponent(fullRaw)});const level=await parseJsonResponse(response);updateLevelView(level);}catch(error){console.log(error);}}refreshData();setInterval(refreshData,2000);");
  html += F("</script>");
  html += F("</section></main></body></html>");
  return html;
}

// Handler untuk endpoint root (/). Mengirim halaman dashboard utama.
void handleRoot() {
  server.send(200, "text/html", buildHtmlPage());
}

// Handler untuk endpoint /status. Mengirim status jaringan dan uptime dalam JSON.
void handleStatus() {
  const NetworkStatus status = getNetworkStatus();

  String json = "{";
  json += "\"mode\":\"";
  json += (status.apMode ? "ap" : "station");
  json += "\",";
  json += "\"ip\":\"" + status.ipAddress + "\",";
  json += "\"ssid\":\"" + status.ssid + "\",";
  json += "\"uptime_seconds\":" + String(millis() / 1000);
  json += "}";
  server.send(200, "application/json", json);
}

// Handler untuk endpoint /level. Mengirim data sensor level dalam JSON.
void handleLevel() {
  const LevelReading level = readLevel();

  String json = "{";
  json += "\"raw\":" + String(level.raw) + ",";
  json += "\"percent\":" + String(level.percent) + ",";
  json += "\"pin\":" + String(AppConfig::LEVEL_SENSOR_PIN) + ",";
  json += "\"empty_raw\":" + String(level.emptyRaw) + ",";
  json += "\"full_raw\":" + String(level.fullRaw);
  json += "}";
  server.send(200, "application/json", json);
}

// Handler untuk endpoint /level/config. Mengubah kalibrasi min dan max sensor level.
void handleLevelConfig() {
  if (!server.hasArg("empty_raw") || !server.hasArg("full_raw")) {
    server.send(400, "application/json", "{\"error\":\"parameter empty_raw dan full_raw wajib diisi\"}");
    return;
  }

  setLevelCalibration(server.arg("empty_raw").toInt(), server.arg("full_raw").toInt());
  updatePumpControl(readLevel().percent);
  handleLevel();
}

// Handler untuk endpoint /relay. Mengirim status relay pompa dalam JSON.
void handleRelayStatus() {
  const RelayStatus relay = getRelayStatus();

  String json = "{";
  json += "\"enabled\":" + String(relay.enabled ? "true" : "false") + ",";
  json += "\"label\":\"" + String(relay.enabled ? "on" : "off") + "\",";
  json += "\"pin\":" + String(relay.pin) + ",";
  json += "\"auto_mode\":" + String(relay.autoMode ? "true" : "false") + ",";
  json += "\"auto_start_percent\":" + String(relay.autoStartPercent) + ",";
  json += "\"auto_stop_percent\":" + String(relay.autoStopPercent);
  json += "}";
  server.send(200, "application/json", json);
}

// Handler untuk endpoint /relay/set. Mengubah status relay berdasarkan input web.
void handleRelaySet() {
  if (!server.hasArg("enabled")) {
    server.send(400, "application/json", "{\"error\":\"parameter enabled wajib diisi\"}");
    return;
  }

  const RelayStatus relay = getRelayStatus();
  if (relay.autoMode) {
    server.send(409, "application/json", "{\"error\":\"nonaktifkan mode auto untuk kontrol manual\"}");
    return;
  }

  const String enabledArg = server.arg("enabled");
  const bool enabled = enabledArg == "1" || enabledArg.equalsIgnoreCase("true") || enabledArg.equalsIgnoreCase("on");
  setPumpEnabled(enabled);
  handleRelayStatus();
}

// Handler untuk endpoint /relay/config. Mengubah mode auto dan threshold pompa.
void handleRelayConfig() {
  if (server.hasArg("auto_mode")) {
    const String autoModeArg = server.arg("auto_mode");
    const bool autoMode = autoModeArg == "1" || autoModeArg.equalsIgnoreCase("true") || autoModeArg.equalsIgnoreCase("on");
    setPumpAutoMode(autoMode);
  }

  if (server.hasArg("auto_start_percent") || server.hasArg("auto_stop_percent")) {
    const RelayStatus relay = getRelayStatus();
    const int startPercent = server.hasArg("auto_start_percent") ? server.arg("auto_start_percent").toInt() : relay.autoStartPercent;
    const int stopPercent = server.hasArg("auto_stop_percent") ? server.arg("auto_stop_percent").toInt() : relay.autoStopPercent;
    setPumpAutoThresholds(startPercent, stopPercent);
  }

  updatePumpControl(readLevel().percent);

  handleRelayStatus();
}

// Handler default jika path yang diminta client tidak tersedia.
void handleNotFound() {
  server.send(404, "text/plain", "Endpoint tidak ditemukan");
}
} // namespace

void initializeWebApp() {
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/level", handleLevel);
  server.on("/level/config", HTTP_POST, handleLevelConfig);
  server.on("/relay", HTTP_GET, handleRelayStatus);
  server.on("/relay/set", HTTP_POST, handleRelaySet);
  server.on("/relay/config", HTTP_POST, handleRelayConfig);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server berjalan di port 80.");
}

void handleWebAppClient() {
  server.handleClient();
}
