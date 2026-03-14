# LevelControl ESP32 Web Server

## Ringkasan
Project ini mengubah ESP32 menjadi web server untuk monitoring level air dan kontrol pompa melalui relay. Sistem dapat berjalan dalam mode Wi-Fi client atau fallback ke Access Point, menyediakan dashboard web, endpoint JSON, kontrol manual pompa, kontrol otomatis berbasis level air, proteksi anti rapid switching, dan penyimpanan setting relay ke flash ESP32.

README ini berfungsi sebagai:
- ringkasan seluruh pekerjaan yang sudah dilakukan pada pemrograman
- dokumentasi teknis untuk pengembangan berikutnya
- user manual untuk penggunaan aplikasi/web server oleh operator

## Fitur Yang Sudah Dibuat
Fitur yang saat ini sudah tersedia di program:
- ESP32 berjalan sebagai HTTP web server pada port 80
- mencoba koneksi ke Wi-Fi router saat boot
- otomatis membuat Access Point jika koneksi router gagal atau SSID belum diisi
- dashboard web untuk menampilkan status perangkat
- pembacaan sensor level berbasis ADC dan ditampilkan dalam persen
- kalibrasi sensor level kosong dan penuh dari web
- endpoint JSON untuk status jaringan, level sensor, dan status relay
- kontrol pompa manual dari web
- mode otomatis pompa berdasarkan level air
- hysteresis antara batas start dan stop pompa agar tidak hunting
- proteksi anti rapid switching relay dengan jeda minimum antar perpindahan
- penyimpanan status relay, mode auto, dan threshold ke flash ESP32
- dokumentasi source code per modul dan fungsi-fungsi utama

## Pekerjaan Yang Sudah Dilakukan Di Pemrograman Ini
Perubahan dan pengembangan yang sudah dibuat selama implementasi:

### 1. Pembuatan Web Server Dasar
- program default PlatformIO diganti menjadi aplikasi ESP32 web server
- server HTTP berjalan di port 80
- dibuat halaman HTML utama pada endpoint `/`
- dibuat endpoint JSON dasar untuk monitoring

### 2. Penambahan Dukungan Jaringan
- ESP32 mencoba terkoneksi ke Wi-Fi menggunakan SSID dan password yang dikonfigurasi
- jika gagal, ESP32 otomatis berpindah ke mode Access Point
- status jaringan aktif ditampilkan di dashboard dan endpoint `/status`

### 3. Penambahan Sensor Level
- sensor level dibaca melalui ADC 12-bit
- nilai ADC diubah ke persentase 0 sampai 100
- data level ditampilkan di dashboard dan endpoint `/level`

### 3a. Penambahan Kalibrasi Sensor Level
- ditambahkan nilai kalibrasi `kosong` dan `penuh` untuk sensor level
- kalibrasi bisa diubah dari dashboard web
- kalibrasi disimpan ke flash ESP32
- hasil persentase level sekarang dihitung berdasarkan kalibrasi aktif

### 4. Refactor Menjadi Modular
- kode dipecah menjadi beberapa file agar lebih mudah dirawat
- `main.cpp` dijadikan bootstrap aplikasi
- modul sensor, jaringan, relay, dan web dipisahkan sesuai tanggung jawabnya

### 5. Penambahan Kontrol Relay Pompa
- dibuat modul relay untuk kontrol output pompa
- status pompa dapat diubah dari dashboard web
- tersedia default status pompa saat ESP32 pertama kali boot

### 6. Penambahan Mode Otomatis Pompa
- pompa bisa berjalan dalam mode manual atau auto
- mode auto memakai dua threshold
  - start saat level rendah
  - stop saat level tinggi
- ini mencegah relay sering berubah status saat nilai level berada di area ambang

### 7. Penambahan Proteksi Relay
- ditambahkan jeda minimum antar switching relay
- proteksi ini membantu mencegah relay terlalu sering ON/OFF dalam waktu singkat

### 8. Penambahan Persistence Ke Flash
- setting relay disimpan ke flash menggunakan `Preferences`
- nilai yang disimpan:
  - status pompa
  - mode manual/auto
  - batas auto start
  - batas auto stop
- saat ESP32 restart, nilai terakhir dimuat kembali

### 9. Penambahan Dokumentasi
- komentar pada fungsi dan prosedur ditambahkan di source
- README diperbarui untuk dokumentasi teknis dan user manual

## Struktur Project
Struktur project saat ini:

```text
LevelControl/
|-- include/
|   |-- app_config.h
|   |-- level_sensor.h
|   |-- network_manager.h
|   |-- relay_control.h
|   `-- web_app.h
|-- src/
|   |-- level_sensor.cpp
|   |-- main.cpp
|   |-- network_manager.cpp
|   |-- relay_control.cpp
|   `-- web_app.cpp
|-- platformio.ini
`-- README.md
```

## Tanggung Jawab Tiap File
- `src/main.cpp`
  Titik masuk aplikasi. Menginisialisasi sensor, relay, jaringan, dan web server.
- `include/app_config.h`
  Menyimpan konfigurasi utama seperti Wi-Fi, pin sensor, pin relay, threshold, dan proteksi switching.
- `include/level_sensor.h` dan `src/level_sensor.cpp`
  Menangani inisialisasi ADC, pembacaan level sensor, kalibrasi sensor, dan persistence kalibrasi.
- `include/network_manager.h` dan `src/network_manager.cpp`
  Menangani koneksi Wi-Fi client dan fallback ke Access Point.
- `include/relay_control.h` dan `src/relay_control.cpp`
  Menangani status pompa, mode auto/manual, hysteresis, proteksi switch relay, dan persistence ke flash.
- `include/web_app.h` dan `src/web_app.cpp`
  Menangani route HTTP, dashboard HTML, dan endpoint JSON/API.

## Alur Kerja Program
Urutan kerja program saat board dinyalakan:
1. Serial diinisialisasi pada baud rate `115200`
2. sensor level ADC diinisialisasi
3. modul relay diinisialisasi dan setting terakhir dimuat dari flash
4. ESP32 mencoba koneksi ke Wi-Fi router
5. jika gagal, ESP32 membuat Access Point lokal
6. web server dijalankan pada port 80
7. loop utama membaca level dan mengevaluasi logika pompa otomatis
8. web server terus memproses request dari browser atau client lain

## Konfigurasi Utama
Semua konfigurasi utama berada di `include/app_config.h`.

### Konfigurasi Wi-Fi
- `WIFI_SSID`
  Nama Wi-Fi router yang ingin digunakan.
- `WIFI_PASSWORD`
  Password Wi-Fi router.
- `AP_SSID`
  Nama Access Point cadangan jika ESP32 gagal terkoneksi ke router.
- `AP_PASSWORD`
  Password Access Point cadangan.
- `WIFI_TIMEOUT_MS`
  Waktu tunggu koneksi router sebelum masuk ke mode Access Point.

### Konfigurasi Sensor
- `LEVEL_SENSOR_PIN`
  Pin ADC tempat sensor level terhubung.
- `LEVEL_EMPTY_RAW_DEFAULT`
  Nilai raw default saat sensor dianggap kosong.
- `LEVEL_FULL_RAW_DEFAULT`
  Nilai raw default saat sensor dianggap penuh.

### Konfigurasi Relay Pompa
- `RELAY_PIN`
  Pin output untuk relay pompa.
- `RELAY_ACTIVE_HIGH`
  `true` jika relay aktif pada logika HIGH.
  `false` jika relay aktif pada logika LOW.
- `RELAY_INITIAL_ON`
  Status awal pompa saat board menyala jika belum ada data tersimpan di flash.
- `RELAY_AUTO_MODE_INITIAL`
  Mode awal pompa saat board menyala jika belum ada data tersimpan di flash.
- `RELAY_AUTO_START_PERCENT`
  Level saat pompa otomatis dinyalakan.
- `RELAY_AUTO_STOP_PERCENT`
  Level saat pompa otomatis dimatikan.
- `RELAY_MIN_SWITCH_INTERVAL_MS`
  Jeda minimum antar perpindahan relay.

## Penjelasan Logika Sistem

### 1. Pembacaan Level Sensor
Sensor dibaca sebagai nilai ADC 12-bit dengan rentang 0 sampai 4095. Nilai ini dipetakan ke persentase 0 sampai 100.

Interpretasi sederhananya:
- `0` ADC dianggap `0%`
- `4095` ADC dianggap `100%`

Jika sensor aktual memiliki karakteristik berbeda, kalibrasi min/max bisa ditambahkan pada pengembangan selanjutnya.

Saat ini kalibrasi tersebut sudah tersedia dan bisa diubah dari dashboard atau endpoint `/level/config`.

Persentase level dihitung dari dua titik kalibrasi:
- `emptyRaw` mewakili 0%
- `fullRaw` mewakili 100%

Jika sensor dipasang terbalik, sistem tetap bisa dipakai karena kalibrasi kosong dan penuh boleh memiliki arah raw yang berbeda.

### 2. Mode Relay Manual
Dalam mode manual:
- operator menyalakan pompa dari tombol `Pompa ON`
- operator mematikan pompa dari tombol `Pompa OFF`
- mode ini cocok untuk pengujian atau override manual

### 3. Mode Relay Otomatis
Dalam mode otomatis:
- pompa `ON` saat level `<= RELAY_AUTO_START_PERCENT`
- pompa `OFF` saat level `>= RELAY_AUTO_STOP_PERCENT`

Ini adalah pola hysteresis. Dengan dua ambang berbeda, relay tidak akan sering bolak-balik saat level berada di titik batas.

### 4. Proteksi Anti Rapid Switching
Sistem menahan perubahan relay jika jeda minimum belum terlewati. Ini membantu:
- melindungi relay
- mengurangi stres pada pompa
- menghindari switching terlalu cepat akibat pembacaan sensor yang fluktuatif

### 5. Persistence Ke Flash
Setting relay disimpan otomatis ke flash ESP32 menggunakan `Preferences`. Saat board restart, nilai terakhir dimuat kembali sehingga operator tidak perlu mengatur ulang setiap kali ESP32 reboot.

Selain setting relay, kalibrasi sensor level juga disimpan ke flash dan dimuat kembali saat board restart.

## Endpoint HTTP Dan API

### 1. Dashboard
- `GET /`
  Menampilkan halaman dashboard web.

### 2. Status Perangkat
- `GET /status`
  Mengembalikan informasi status jaringan dan uptime.

Contoh respons:
```json
{
  "mode": "station",
  "ip": "192.168.1.10",
  "ssid": "NamaWiFi",
  "uptime_seconds": 120
}
```

### 3. Data Level Sensor
- `GET /level`
  Mengembalikan nilai ADC mentah, persentase level, dan kalibrasi aktif.

Contoh respons:
```json
{
  "raw": 2048,
  "percent": 50,
  "pin": 4,
  "empty_raw": 300,
  "full_raw": 3200
}
```

### 4. Kalibrasi Sensor Level
- `POST /level/config`
  Mengubah nilai raw untuk kondisi kosong dan penuh.

Parameter:
- `empty_raw=angka`
- `full_raw=angka`

### 5. Status Relay
- `GET /relay`
  Mengembalikan status pompa, mode kontrol, dan threshold auto.

Contoh respons:
```json
{
  "enabled": true,
  "label": "on",
  "pin": 5,
  "auto_mode": false,
  "auto_start_percent": 30,
  "auto_stop_percent": 80
}
```

### 6. Kontrol Manual Relay
- `POST /relay/set`
  Mengubah status pompa saat mode manual.

Parameter:
- `enabled=1` untuk menyalakan pompa
- `enabled=0` untuk mematikan pompa

Catatan:
- jika pompa sedang dalam mode auto, endpoint ini akan menolak kontrol manual

### 7. Konfigurasi Mode Auto Relay
- `POST /relay/config`
  Mengubah mode auto/manual dan threshold pompa.

Parameter yang didukung:
- `auto_mode=1` untuk mode auto
- `auto_mode=0` untuk mode manual
- `auto_start_percent=angka`
- `auto_stop_percent=angka`

## User Manual

### A. Persiapan Awal
1. Hubungkan sensor level ke pin ADC yang sesuai.
2. Hubungkan modul relay pompa ke pin output yang sesuai.
3. Sesuaikan semua konfigurasi pada `include/app_config.h`.
4. Upload firmware ke ESP32.
5. Buka Serial Monitor dengan baud rate `115200`.

### B. Menjalankan Dalam Mode Wi-Fi Router
1. Isi `WIFI_SSID` dan `WIFI_PASSWORD` di konfigurasi.
2. Nyalakan atau restart ESP32.
3. Lihat serial monitor untuk IP address yang didapat.
4. Buka alamat IP tersebut di browser.

### C. Menjalankan Dalam Mode Access Point
Jika koneksi ke router gagal atau SSID belum diisi:
1. Cari jaringan Wi-Fi bernama `ESP32-WebServer`.
2. Sambungkan dengan password `12345678` atau sesuai konfigurasi.
3. Buka `192.168.4.1` di browser.

### D. Menggunakan Dashboard
Dashboard menampilkan:
- persentase level air
- nilai ADC mentah
- nilai kalibrasi kosong dan penuh
- mode jaringan dan IP address
- status pompa
- mode pompa manual atau auto
- batas auto start dan auto stop

### E. Kalibrasi Sensor Level
1. Buka dashboard.
2. Perhatikan nilai `ADC raw` saat kondisi tangki kosong.
3. Tekan `Set Kosong = Raw Sekarang` atau isi manual nilai `Raw saat kosong`.
4. Perhatikan nilai `ADC raw` saat kondisi tangki penuh.
5. Tekan `Set Penuh = Raw Sekarang` atau isi manual nilai `Raw saat penuh`.
6. Tekan `Simpan Kalibrasi`.

Catatan:
- kalibrasi yang disimpan akan langsung dipakai untuk menghitung persen level
- kalibrasi akan tetap tersimpan setelah ESP32 restart

### F. Mengoperasikan Pompa Dalam Mode Manual
1. Buka dashboard.
2. Pilih `Mode Manual`.
3. Tekan `Pompa ON` untuk menyalakan pompa.
4. Tekan `Pompa OFF` untuk mematikan pompa.

Catatan:
- perubahan status mungkin tertunda jika proteksi jeda switching masih aktif

### G. Mengoperasikan Pompa Dalam Mode Otomatis
1. Buka dashboard.
2. Masukkan nilai `Start pompa saat level <= (%)`.
3. Masukkan nilai `Stop pompa saat level >= (%)`.
4. Tekan `Simpan Batas Auto`.
5. Tekan `Mode Auto`.

Cara kerja:
- saat level di bawah atau sama dengan batas start, pompa menyala
- saat level di atas atau sama dengan batas stop, pompa mati

### H. Setelah Restart ESP32
Setting berikut akan kembali otomatis:
- kalibrasi sensor kosong
- kalibrasi sensor penuh
- status pompa terakhir
- mode manual atau auto
- threshold auto start
- threshold auto stop

### I. Skenario Penggunaan Yang Disarankan
Contoh penggunaan mode auto:
- `auto start = 30`
- `auto stop = 80`

Artinya:
- pompa mulai mengisi saat level turun ke 30% atau lebih rendah
- pompa berhenti saat level mencapai 80% atau lebih tinggi

## Panduan Untuk Developer

### Fungsi-Fungsi Penting
- `initializeLevelSensor()`
  Menyiapkan ADC, pin sensor, dan memuat kalibrasi tersimpan.
- `readLevel()`
  Membaca level sensor dalam bentuk `raw` dan `percent`.
- `getLevelCalibration()`
  Mengambil kalibrasi sensor yang sedang aktif.
- `setLevelCalibration()`
  Menyimpan kalibrasi raw kosong dan penuh.
- `initializeNetwork()`
  Menyiapkan koneksi Wi-Fi atau Access Point.
- `getNetworkStatus()`
  Mengambil status jaringan aktif.
- `initializeRelay()`
  Menyiapkan relay, memuat setting tersimpan, dan menerapkan status pompa.
- `setPumpEnabled()`
  Mengubah status pompa.
- `setPumpAutoMode()`
  Mengubah mode manual/auto.
- `setPumpAutoThresholds()`
  Mengubah batas auto start dan auto stop.
- `updatePumpControl()`
  Menjalankan logika otomatis pompa.
- `getRelayStatus()`
  Mengambil status relay untuk API dan UI.
- `initializeWebApp()`
  Mendaftarkan seluruh route HTTP.
- `handleWebAppClient()`
  Memproses request web server.

### Catatan Pengembangan Lanjutan
Pengembangan yang logis setelah tahap ini:
- kalibrasi sensor level min/max
- penyimpanan konfigurasi Wi-Fi dari web
- autentikasi dashboard
- notifikasi status pompa
- indikator waktu tunggu relay akibat anti rapid switching
- logging event ke serial atau penyimpanan lokal

## Troubleshooting

### ESP32 tidak muncul di browser
- cek serial monitor untuk IP address aktif
- pastikan perangkat client berada di jaringan yang sama
- jika mode AP aktif, gunakan IP `192.168.4.1`

### Pompa tidak merespons tombol manual
- pastikan sedang di `Mode Manual`
- cek wiring relay
- cek apakah relay menggunakan logika aktif-HIGH atau aktif-LOW
- cek apakah jeda `RELAY_MIN_SWITCH_INTERVAL_MS` masih aktif

### Pompa tidak bekerja benar di mode auto
- cek nilai `auto start` dan `auto stop`
- pastikan `auto start` lebih kecil atau sama dengan `auto stop`
- cek pembacaan level sensor pada dashboard

### Nilai level tidak akurat
- cek wiring sensor
- cek apakah pin yang digunakan benar
- lakukan kalibrasi sensor dari dashboard
- pastikan nilai kosong dan penuh tidak tertukar secara tidak sengaja

## Catatan Penting
- build `pio run` belum bisa divalidasi dari terminal percakapan ini karena `pio` CLI belum tersedia di shell yang aktif
- validasi editor untuk file source yang diubah sudah bersih tanpa error terdeteksi
