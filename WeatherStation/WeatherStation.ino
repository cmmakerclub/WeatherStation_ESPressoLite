// Copyright Nat Weerawan 2015-2016
// MIT License
#include <ESPert.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiConnector.h>
#include <ESP8266HTTPClient.h>

ESPert espert;

#ifndef WIFI_SSID
#define WIFI_SSID       "Apirak_RMUTL"
#define WIFI_PASSPHRASE "rmutlqwerty"
#endif

int wificount = 0;
const int sleepTimeS = 300; // 300 = 30นาที


WiFiConnector wifi(WIFI_SSID, WIFI_PASSPHRASE);

void init_hardware()  {
  espert.init();
  espert.dht.init();
  espert.oled.init();
  delay(2000);
  Serial.begin(115200);
  WiFi.disconnect(true);
  delay(1000);
}

void init_wifi() {
  wifi.init();
  wifi.on_connected([&](const void* message)  {
    Serial.print("WIFI CONNECTED WITH IP: ");
    Serial.println(WiFi.localIP());
  });

  wifi.on_connecting([&](const void* message) {
    Serial.print("Connecting to ");
    Serial.println(wifi.get("ssid") + ", " + wifi.get("password"));
    delay(200);
    if (wificount >= 10) {    // if can't connect wifi, this board is sleep 30 minute
      espert.oled.clear();
      ESP.deepSleep(sleepTimeS * 6000000);
      wificount = 0;
    }
  });
}


void doHttpGet() {
  HTTPClient http;
  bool isFarenheit = false;
  float t = espert.dht.getTemperature(isFarenheit);
  float h = espert.dht.getHumidity();

  if (!isnan(t) && !isnan(h)) {
    String dht = "Temperature: " + String(t) + (isFarenheit ?  " F" : " C") + "\n";
    dht += " Humidity   : " + String(h) + " %\n";
    espert.oled.clear();
    espert.oled.println("   Weather Station ");
    espert.oled.println("");
    espert.oled.println(dht);
    espert.oled.println("RMUTLlanna.github.io");
    espert.oled.update();
    espert.println(dht);
  }

  Serial.print("[HTTP] begin...\n");
  http.begin("http://api.thingspeak.com/update?api_key=WZ5K3ETH85H9WWXZ&field1=" + String(t) + "&field2=" + String(h)); //HTTP
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    Serial.print("[CONTENT]\n");

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void setup()  {
  init_hardware();
  init_wifi();
  wifi.connect();
}

void loop() {
  wifi.loop();
  if (wifi.connected()) {
    espert.oled.clear();
    espert.oled.println("   Weather Station ");
    espert.oled.println("");
    espert.oled.println("");
    espert.oled.println("  Wifi is connected!");
    espert.oled.update();
    delay(2000);
    doHttpGet();
    delay(5000);
    espert.oled.clear();
    espert.oled.println("  Sleep (- -)zzZ");
    espert.oled.update();
    delay(3000);
    espert.oled.clear();
    ESP.deepSleep(sleepTimeS * 6000000);  //  Sleep mode 30 minute
  } else {
    bool isFarenheit = false;
    float t = espert.dht.getTemperature(isFarenheit);
    float h = espert.dht.getHumidity();
    if (!isnan(t) && !isnan(h)) {
      String dht = "Temperature: " + String(t) + (isFarenheit ?  " F" : " C") + "\n";
      dht += " Humidity   : " + String(h) + " %\n";
      espert.oled.clear();
      espert.oled.println("   Weather Station ");
      espert.oled.println("");
      espert.oled.println(dht);
      espert.oled.println("Wifi is not connect !");
      espert.oled.update();
      espert.println(dht);
      delay(5000);
      wificount += 1;
    }
  }
}

