#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_dsp.h>

// Definições para o Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// URL do servidor (simulada para o exemplo)
const char* serverUrl = "http://10.1.1.1/upload";

// Definições para FFT
#define SAMPLES 512
#define SAMPLING_FREQUENCY 16000

float vReal[SAMPLES * 2]; // Aloca espaço para componentes real e imaginário

void setup() {
  Serial.begin(115200);

  // Conexão Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Inicializa os arrays com zeros
  for (int i = 0; i < SAMPLES * 2; i++) {
    vReal[i] = 0;
  }

  // Simulação de leitura de áudio (sinal de 400 Hz)
  for (int i = 0; i < SAMPLES; i++) {
    vReal[2 * i] = 0.5 * (1 - cosf(2 * M_PI * i / (SAMPLES - 1))); // Janela Hanning
    vReal[2 * i + 1] = sinf(2 * M_PI * 400 * i / SAMPLING_FREQUENCY); // Real
  }

  // Inicializa a biblioteca esp-dsp
  dsps_fft2r_init_fc32(NULL, SAMPLES);

  // Aplica a FFT
  dsps_fft2r_fc32(vReal, SAMPLES);
  dsps_bit_rev_fc32(vReal, SAMPLES);
  dsps_cplx2reC_fc32(vReal, SAMPLES);

  float maxMagnitude = 0;
  int maxIndex = 0;
  for (int i = 1; i < (SAMPLES / 2); i++) {
    float real = vReal[2 * i];
    float imag = vReal[2 * i + 1];
    float magnitude = sqrtf(real * real + imag * imag);
    if (magnitude > maxMagnitude) {
      maxMagnitude = magnitude;
      maxIndex = i;
    }
  }

  float frequency = (maxIndex * SAMPLING_FREQUENCY) / SAMPLES;
  float max_dB = 20 * log10(maxMagnitude / 32768.0);

  Serial.print("Frequency: ");
  Serial.print(frequency);
  Serial.print(" Hz, Max dB: ");
  Serial.println(max_dB);

  // Verificação da faixa de choro de bebê
  if (frequency >= 300 && frequency <= 600) {
    Serial.println("Baby cry detected!");
    // Enviar dados simulados para o servidor
    sendAudioData(max_dB, 10); // Assume duração simulada de 10 segundos
  } else {
    Serial.println("No baby cry detected.");
  }
}

void sendAudioData(float max_dB, int duration) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Dados em formato JSON
    String jsonPayload = "{\"max_dB\":" + String(max_dB) + ",\"duration\":" + String(duration) + "}";
    
    int httpResponseCode = http.POST((uint8_t*)jsonPayload.c_str(), jsonPayload.length());

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.println("Error on sending POST");
    }

    http.end(); // Fecha a conexão HTTP
  }
}

void loop() {
  // O loop principal está vazio, pois a simulação é feita no setup
}
