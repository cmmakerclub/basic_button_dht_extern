#include <MqttConnector.h>
#include <DHT.h>
#define DHTPIN 12     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

extern String DEVICE_NAME;
extern int pin_state;
extern MqttConnector* mqtt;

static void read_dht();
float t_dht, h_dht = 0;

void register_publish_hooks() {
  mqtt->on_prepare_data_once([&](void) {
    dht.begin();
  });

  mqtt->on_before_prepare_data([&](void) {
    read_dht();
  });

  mqtt->on_prepare_data([&](JsonObject * root) {
    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];
    data["myName"] = DEVICE_NAME;
    data["millis"] = millis();
    data["temp"] = t_dht;
    data["humid"] = h_dht;
    data["state"] = pin_state;
  });

  mqtt->on_after_prepare_data([&](JsonObject * root) {
    /**************
      JsonObject& data = (*root)["d"];
      data.remove("version");
      data.remove("subscription");
    **************/
  });
}

static void read_dht() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else {
    t_dht = t;
    h_dht = h;
    Serial.print("Temp: ");
    Serial.println(t_dht);
    Serial.print("Humid: ");
    Serial.println(h_dht);
  }
}
