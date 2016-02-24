#include <ESPert.h>

ESPert espert;
const char *host = "api.thingspeak.com";
const int httpPort = 80;
//const String path = String("/update?api_key=1DYAF3DO3X8GR6JS&field1=") += String(t) += String("&field2=") += String(h);
//GET https://api.thingspeak.com/update?api_key=1DYAF3DO3X8GR6JS&field1=0
void setup() {
  espert.init();
  espert.oled.init();
  espert.dht.init();
  delay(2000);

  espert.oled.clear();
  espert.oled.println(espert.info.getId());
  espert.oled.println();

  int mode = espert.wifi.init();

  if (mode == ESPERT_WIFI_MODE_CONNECT) {
    espert.println(">>> WiFi mode: connected.");
    espert.oled.println("WiFi: connected.");
    espert.oled.print("IP..: ");
    espert.oled.println(espert.wifi.getLocalIP());
  } else if (mode == ESPERT_WIFI_MODE_DISCONNECT) {
    espert.println(">>> WiFi mode: disconnected.");
    espert.oled.println("WiFi: not connected.");
  } else if (mode == ESPERT_WIFI_MODE_SMARTCONFIG) {
    espert.println(">>> WiFi mode: smart config.");
  } else if (mode == ESPERT_WIFI_MODE_SETTINGAP) {
    espert.println(">>> WiFi mode: access point.");
  }
}

void loop() {
  espert.loop();
  bool isFarenheit = false;
  float t = espert.dht.getTemperature(isFarenheit);
  float h = espert.dht.getHumidity();

  if (!isnan(t) && !isnan(h)) {
    String dht = "Temperature: " + String(t) + (isFarenheit ?  " F" : " C") + "\n";
    dht += "Humidity...: " + String(h) + " %\n";

    espert.oled.clear();
    espert.oled.setTextSize(2);
    espert.oled.setTextColor(ESPERT_WHITE);
    espert.oled.println(dht);
    espert.println(dht);

  }
  const String path = String("/update?api_key=1DYAF3DO3X8GR6JS&field1=") += String(t) += String("&field2=") += String(h);
  espert.println( "Connecting..." );
  espert.println( ">>" + espert.wifi.getHTTP( host, path.c_str() ) + "<<" );
  delay( 1000 );
}

