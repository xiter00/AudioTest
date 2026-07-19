#include <WiFi.h>
#include <WebServer.h>
#include <Audio.h>

// Konfigurasi WiFi
const char* ssid = "NOT MASTAH";
const char* password = "123456789001"; 

WebServer server(80);
Audio audio;

// Konfigurasi Pin I2S untuk MAX98357A
#define I2S_DOUT 21
#define I2S_BCLK 22
#define I2S_LRC  23

// Tampilan Halaman Web Lokal
const char* htmlPage = R"rawliteral(
<!DOCTYPE html><html>
<head><meta name="viewport" content="width=device-width, initial-scale=1"></head>
<body>
<h2>ESP32 TTS Speaker</h2>
<input type="text" id="text" placeholder="Ketik teks di sini...">
<button onclick="playTTS()">Putar Suara</button>
<script>
function playTTS() {
  var text = document.getElementById('text').value;
  fetch('/play?text=' + encodeURIComponent(text));
}
</script>
</body></html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handlePlay() {
  if (server.hasArg("text")) {
    String text = server.arg("text");
    server.send(200, "text/plain", "Sedang memutar: " + text);
    
    // GAK PERLU URL MANUAL LAGI!
    // Pake fungsi sakti bawaan library khusus buat Google TTS
    // Otomatis ngatur HTTPS, buffer memori yang aman, dan bahasa (id = Indonesia)
    audio.connecttospeech(text.c_str(), "id");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Inisialisasi Audio I2S (Gak usah pusingin set buffer lagi)
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(15); 
  
  // Aktifkan Hotspot
  WiFi.softAP(ssid, password);
  Serial.println("Hotspot aktif di: " + WiFi.softAPIP().toString());

  server.on("/", handleRoot);
  server.on("/play", handlePlay);
  server.begin();
}

void loop() {
  server.handleClient();
  audio.loop();
}
