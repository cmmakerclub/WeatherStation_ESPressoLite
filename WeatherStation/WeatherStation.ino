#include <ESPert.h>

ESPert espert;
const char *host = "api.thingspeak.com"; // ส่งค่าไปยังเว็บ thingspeak
const int httpPort = 80;

void setup() {
  // ประกาศใช้งานทั่วไป
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
  float t = espert.dht.getTemperature(isFarenheit); // อ่านค่าอุณหภูมิ
  float h = espert.dht.getHumidity();               // อ่านค่าความชื้น

  // ฟังก์ชั่นตรวจสอบเซ็นเซอร์ว่ายังสามารถอ่านค่าได้รึป่าว แล้วแสดงผลออกจอ OLED 
  if (!isnan(t) && !isnan(h)) {
    String dht = "Temperature: " + String(t) + (isFarenheit ?  " F" : " C") + "\n";
    dht += "Humidity...: " + String(h) + " %\n";
    espert.oled.clear();
    espert.oled.setTextSize(2);
    espert.oled.setTextColor(ESPERT_WHITE);
    espert.oled.println(dht);
    espert.println(dht);
  }
  
  // ทำการอัพค่าขึ้น thingspeak
  //                                    ส่งค่าไปยัง API Key 1DYAF3DO3X8GR6JS
  //const String path = String("/update?api_key=1DYAF3DO3X8GR6JS&field1=") += String(t)  // field1 ให้ส่งค่าจากตัวแปร t
  //                    += String("&field2=") += String(h);                              // field2 ให้ส่งค่าจากตัวแปร h
  const String path = String("/update?api_key=1DYAF3DO3X8GR6JS&field1=") += String(t) += String("&field2=") += String(h);
  
  espert.println( "Connecting..." );
  espert.println( ">>" + espert.wifi.getHTTP( host, path.c_str() ) + "<<" ); // ส่งค่าไปโล้ดดดด
  delay( 1000 );
}

