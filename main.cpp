
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <driver/i2s.h>
#include "arduinoFFT.h"

#define I2S_WS 26  // LRC pin
#define I2S_SD 25  // DOUT pin
#define I2S_SCK 33 // BCLK pin
#define SAMPLES 1024
#define SAMPLE_RATE 44100

const char* ssid = "your_SSID";        // Replace with your network SSID
const char* password = "your_PASSWORD"; // Replace with your network password
const char* serverUrl = "http://your_server/upload";

arduinoFFT FFT = arduinoFFT();
double vReal[SAMPLES];
double vImag[SAMPLES];

void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // I2S configuration
    i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = i2s_bits_per_sample_t(16),
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S),
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false
    };
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_SD
    };
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}

void loop() {
    size_t bytesRead;
    int16_t samples[SAMPLES];
    i2s_read(I2S_NUM_0, &samples, sizeof(samples), &bytesRead, portMAX_DELAY);

    for (int i = 0; i < SAMPLES; i++) {
        vReal[i] = samples[i];
        vImag[i] = 0;
    }

    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

    // Detect frequencies within the range typical of baby crying (300-600 Hz)
    double sum = 0;
    int count = 0;
    for (int i = 0; i < (SAMPLES / 2); i++) {
        if (FFT.MajorPeak(vReal, SAMPLES, SAMPLE_RATE) > 300 && FFT.MajorPeak(vReal, SAMPLES, SAMPLE_RATE) < 600) {
            sum += vReal[i];
            count++;
        }
    }
    double avg = sum / count;
    Serial.printf("Average dB in crying range: %f
", avg);

    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");
        String payload = String("{"decibels": "") + String(avg) + " dB", "duration_seconds": "10"}";
        int httpResponseCode = http.POST(payload);
        if (httpResponseCode > 0) {
            Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        } else {
            Serial.printf("Error code: %d\n", httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }

    delay(10000); // Delay to prevent spamming
}
