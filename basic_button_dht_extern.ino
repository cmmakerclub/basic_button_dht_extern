#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include <CMMC_Manager.h>
#include <MqttConnector.h>

#include "init_mqtt.h"
#include "_publish.h"
#include "_receive.h"

//CMMC_Manager manager(0, LED_BUILTIN);
#define AButtonPin 2

// include libraries
#include <Task.h>

// include sub files
#include "ButtonTask.h" // this implements the button task
TaskManager taskManager;
void HandleAButtonChanged(ButtonState state);

// foreward delcare functions passed to task constructors now required
ButtonTask AButtonTask(HandleAButtonChanged, AButtonPin);


/* WIFI INFO */
#ifndef WIFI_SSID
#define WIFI_SSID        "DEVICES-AP"
#define WIFI_PASSWORD    "devicenetwork"
#endif

String MQTT_HOST        = "mqtt.cmmc.io";
String MQTT_USERNAME    = "";
String MQTT_PASSWORD    = "";
String MQTT_CLIENT_ID   = "";
String MQTT_PREFIX      = "CMMC";
int    MQTT_PORT        = 1883;

int PUBLISH_EVERY       = 1000;

MqttConnector *mqtt;
extern int pin_state;

void init_hardware()
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("Starting...");
  pinMode(LED_BUILTIN, OUTPUT);
}

void init_wifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf ("Connecting to %s:%s\r\n", WIFI_SSID, WIFI_PASSWORD);
    delay(300);
  }
  //  manager.start();
  Serial.println("WiFi Connected.");
  digitalWrite(LED_BUILTIN, HIGH);
}

void init_button_task() {
  taskManager.StartTask(&AButtonTask);
}

void setup()
{
  init_hardware();
  init_wifi();
  init_mqtt();
  init_button_task();
}

void loop()
{
  mqtt->loop();
  taskManager.Loop();
}

void HandleAButtonChanged(ButtonState state)
{
  // apply on press
  if (state == ButtonState_Released) // any state that is pressed
  {
    pin_state = !pin_state;
    String _state =  (pin_state == 1 ? "ON": "OFF");
    mqtt->publish(MQTT_PREFIX + "/" + MQTT_CLIENT_ID + "/$/button", _state, true);
  }
}
