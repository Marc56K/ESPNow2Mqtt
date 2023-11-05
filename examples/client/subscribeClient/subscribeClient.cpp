/* example with LCD screen that sends single mesages*/

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <esp_wifi.h>
#include "EspNow2MqttClient.hpp"

#define MESSAGE_TYPE_A 1
#define MESSAGE_TYPE_B 2
char *queueA = "qa";
char *queueB = "qb";

byte sharedKey[16] = {10, 200, 23, 4, 50, 3, 99, 82, 39, 100, 211, 112, 143, 4, 15, 106};
byte sharedChannel = 8;
uint8_t gatewayMac[6] = {0xA4, 0xCF, 0x12, 0x25, 0x9A, 0x30};
EspNow2MqttClient client = EspNow2MqttClient("tstRq", sharedKey, gatewayMac, sharedChannel);

void displayDataOnCompletion(response &rsp)
{
  char line[30];

  if (1 == rsp.opResponses_count)
  {
    int resultCode = rsp.opResponses[0].result_code;
    char *queue = rsp.message_type == MESSAGE_TYPE_A ? queueA : queueB;
    switch (resultCode)
    {
    case response_Result_OK:
      snprintf(line, sizeof(line), "%s ok: %s", queue, rsp.opResponses[0].payload);
      break;
    case response_Result_NO_MSG:
      snprintf(line, sizeof(line), "%s ok but no msg %d", queue, rsp.message_type);
      break;
    default:
      snprintf(line, sizeof(line), "%s status: %d %s", queue, resultCode, rsp.opResponses[0].payload);
      break;
    }
  }
  else
  {
    snprintf(line, sizeof(line), "error: %d", rsp.opResponses_count);
  }
  Serial.println(line);
}

void onDataSentUpdateDisplay(bool success)
{
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

void testRequest(int msgType)
{
  char *queue = MESSAGE_TYPE_A == msgType ? queueA : queueB;
  Serial.print("request ");
  Serial.println(queue);
  client.doSubscribe(queue, msgType);
}

void setup()
{
  Serial.begin(115200);
  displayMyMac();

  int initcode;
  do
  {
    Serial.println("init");
    initcode = client.init();
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
  client.onReceiveSomething = displayDataOnCompletion;
}

void loop()
{
  testRequest(MESSAGE_TYPE_A);
  delay(3000);
  testRequest(MESSAGE_TYPE_B);
  delay(3000);
}