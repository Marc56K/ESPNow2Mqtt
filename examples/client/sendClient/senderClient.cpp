/* example with LCD screen that sends single mesages*/

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <esp_wifi.h>
#include "EspNow2MqttClient.hpp"

long timeSent, timeack, timePreSetup, timePostSetup;

void displayTimeArriving(response &rsp)
{
  long arrivalTime = millis();
  char line[30];
  long lapseSent = timeack - timeSent;
  long lapse = arrivalTime - timeSent;
  long lapseRadioSetup = timePostSetup - timePreSetup;

  snprintf(line, sizeof(line), "time %d+ %d, %d", lapseRadioSetup, lapseSent, lapse);
  Serial.println(line);
  if (1 == rsp.opResponses_count)
  {
    snprintf(line, sizeof(line), "status : %d", rsp.opResponses[0].result_code);
  }
  else
  {
    snprintf(line, sizeof(line), "error: %d", rsp.opResponses_count);
  }
  Serial.println(line);
}

byte sharedKey[16] = {10, 200, 23, 4, 50, 3, 99, 82, 39, 100, 211, 112, 143, 4, 15, 106};
byte sharedChannel = 8;
uint8_t gatewayMac[6] = {0xA4, 0xCF, 0x12, 0x25, 0x9A, 0x30};
EspNow2MqttClient client = EspNow2MqttClient("tstCl", sharedKey, gatewayMac, sharedChannel);

int counter = 0;

void onDataSentUpdateDisplay(bool success)
{
  timeack = millis();
  Serial.println(success ? "Delivery Success" : "Delivery Fail");
}

void displayMyMac()
{
  char macStr[22];
  strcpy(macStr, "Mac ");
  strcat(macStr, WiFi.macAddress().c_str());
  Serial.println(macStr);
}

int32_t getWiFiChannel(const char *ssid)
{
  if (int32_t n = WiFi.scanNetworks())
  {
    for (uint8_t i = 0; i < n; i++)
    {
      if (!strcmp(ssid, WiFi.SSID(i).c_str()))
      {
        return WiFi.channel(i);
      }
    }
  }
  return 0;
}

void testPing()
{
  char pingNs[6];
  Serial.print("ping ");
  itoa(client.pingCounter, pingNs, 10);
  Serial.println(pingNs);
  client.doPing();
}

void testSend()
{
  Serial.print("send ");
  char payload[6];
  itoa(counter, payload, 10);
  Serial.println(payload);
  client.doSend(payload, "spnowq", true);
  counter++;
}

void setup()
{
  Serial.begin(115200);
  display.init();
  displayMyMac();

  int initcode;
  do
  {
    Serial.println("init");
    timePreSetup = millis();
    initcode = client.init();
    timePostSetup = millis();
    switch (initcode)
    {
    case 1:
      Serial.println("CANNOT INIT");
      break;
    case 2:
      Serial.println("CANNOT PAIR");
      break;
    default:
      Serial.println("PAIRED");
      break;
    }
    delay(1001);
  } while (initcode != 0);

  client.onSentACK = onDataSentUpdateDisplay;
  client.onReceiveSomething = displayTimeArriving;
}

void loop()
{
  timeSent = millis();
  // testPing();
  testSend();
  delay(2000);
}