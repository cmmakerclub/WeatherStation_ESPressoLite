// Copyright Nat Weerawan 2015-2016
// MIT License
#include <ESPert.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiConnector.h>
#include <ESP8266HTTPClient.h>

ESPert espert;

#ifndef WIFI_SSID
#define WIFI_SSID       "@ESPertAP_001" // "@ESPertAP_001"
#define WIFI_PASSPHRASE "espertap"  // espertap
#endif

const int sleepTimeS = 10; // 30 = 30นาที

WiFiConnector wifi(WIFI_SSID, WIFI_PASSPHRASE);

void init_hardware()
{
  Serial.begin(115200);
  WiFi.disconnect(true);
  delay(1000);
  Serial.flush();
  Serial.println();
  Serial.println();
  Serial.println("will be started in 500ms..");
}

void init_wifi() {
  wifi.init();
  wifi.on_connected([&](const void* message)
  {
    Serial.print("WIFI CONNECTED WITH IP: ");
    Serial.println(WiFi.localIP());
  });

  wifi.on_connecting([&](const void* message)
  {
    Serial.print("Connecting to ");
    Serial.println(wifi.get("ssid") + ", " + wifi.get("password"));
    delay(200);
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
    espert.oled.println("  Bavensky.github.io");
    espert.oled.update();

    espert.println(dht);
  }

  Serial.print("[HTTP] begin...\n");
  http.begin("http://api.thingspeak.com/update?api_key=QVLUV8I946J3TG6Z&field1=" + String(t) + "&field2=" + String(h)); //HTTP
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

void setup()
{
  delay(2000);
  espert.init();
  espert.dht.init();
  espert.oled.init();
  delay(2000);

  init_hardware();
  init_wifi();

  wifi.connect();
}

void loop()
{
  wifi.loop();
  if (wifi.connected()) {
    doHttpGet();
    delay(3000);
    espert.oled.clear();
    espert.oled.println("  Sleep (- -)zzZ");
    espert.oled.update();
    ESP.deepSleep(sleepTimeS * 6000000);
  }
}

