/**
 * @file
 *
 * EspNowUnicast.ino demonstrates how to transmit unicast ESP-NOW messages with @c WifiEspNow .
 * You need two ESP8266 or ESP32 devices to run this example.
 *
 * Unicast communication requires the sender to specify the MAC address of the recipient.
 * Thus, you must modify this program for each device.
 *
 * The recommended workflow is:
 * @li 1. Flash the program onto device A.
 * @li 2. Run the program on device A, look at serial console for its MAC address.
 * @li 3. Copy the MAC address of device A, paste it in the @c PEER variable below.
 * @li 4. Flash the program that contains A's MAC address onto device B.
 * @li 5. Run the program on device A, look at serial console for its MAC address.
 * @li 6. Copy the MAC address of device B, paste it in the @c PEER variable below.
 * @li 7. Flash the program that contains B's MAC address onto device A.
 */

/**！
 * @file getAcceleration.ino
 * @brief Get the acceleration in the three directions of xyz, the range can be ±2g、±4g、±8g、±16g
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [tangjie](jie.tang@dfrobot.com)
 * @version  V1.0
 * @date  2021-01-16
 * @url https://github.com/DFRobot/DFRobot_LIS
 */

// CODE FOR ESP32_transmitter

#include <WifiEspNow.h>
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif

#include <Wire.h>
#include <DFRobot_LIS2DH12.h>

// Accelerometer constructor - I2C controller and address
DFRobot_LIS2DH12 acce(&Wire, 0x18);

#define BUZZER_PIN D0
#define TIMER_MAX 50

hw_timer_t *My_timer = NULL;
int timer_counter = 0;

static uint8_t PEER[]{0xD4, 0xD4, 0xDA, 0x96, 0xFF, 0x65}; // MAC addres of ESP32_receiver

void printReceivedMessage(const uint8_t mac[WIFIESPNOW_ALEN], const uint8_t *buf, size_t count,
                          void *arg)
{
  Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3],
                mac[4], mac[5]);
  for (int i = 0; i < static_cast<int>(count); ++i)
  {
    Serial.print(static_cast<char>(buf[i]));
  }
  Serial.println();
}

void IRAM_ATTR onTimer()
{

  if (timer_counter == 0)
  {
    digitalWrite(BUZZER_PIN, HIGH);
  }
  else if (timer_counter == TIMER_MAX)
  {
    timer_counter = -1;
  }
  else
  {
    digitalWrite(BUZZER_PIN, LOW);
  }
  timer_counter++;
}

void setup()
{
  // Setting up serial
  Serial.begin(115200);
  Serial.println();

  // Setting up the buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  My_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 100000, true);
  timerAlarmEnable(My_timer);

  // Setting up ESPNOW protocol
  WiFi.persistent(false);
  WiFi.mode(WIFI_AP);
  WiFi.disconnect();
  WiFi.softAP("ESPNOW", nullptr, 3);
  WiFi.softAPdisconnect(false);

  Serial.print("MAC address of this node is ");
  Serial.println(WiFi.softAPmacAddress());

  uint8_t mac[6];
  WiFi.softAPmacAddress(mac);
  Serial.println();
  Serial.println("You can paste the following into the program for the other device:");
  Serial.printf("static uint8_t PEER[]{0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X};\n", mac[0],
                mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println();

  bool ok = WifiEspNow.begin();
  if (!ok)
  {
    Serial.println("WifiEspNow.begin() failed");
    ESP.restart();
  }

  WifiEspNow.onReceive(printReceivedMessage, nullptr);

  ok = WifiEspNow.addPeer(PEER);
  if (!ok)
  {
    Serial.println("WifiEspNow.addPeer() failed");
    ESP.restart();
  }

  while (!acce.begin())
  {
    Serial.println("Initialization failed, please check the connection and I2C address settings");
    delay(1000);

    char msg[64];
    int len = snprintf(msg, sizeof(msg), "Initialization failed, please check the connection and I2C address settings");

    WifiEspNow.send(PEER, reinterpret_cast<const uint8_t *>(msg), len);
  }
  Serial.print("chip id : ");
  Serial.println(acce.getID(), HEX);

  acce.setRange(/*Range = */ DFRobot_LIS2DH12::eLIS2DH12_16g);
  acce.setAcquireRate(/*Rate = */ DFRobot_LIS2DH12::eLowPower_400Hz);
}

void loop()
{

  long ax, ay, az;
  ax = acce.readAccX();
  az = acce.readAccZ();
  ay = acce.readAccY();

  int64_t time = esp_timer_get_time();
  char msg[64];
  int len = snprintf(msg, sizeof(msg), "%ld %ld %ld %" PRId64 "", ax, ay, az, time);
  
  WifiEspNow.send(PEER, reinterpret_cast<const uint8_t *>(msg), len);
  Serial.println(msg);
}