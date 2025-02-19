#include "ECE140_WIFI.h"
#include "ECE140_MQTT.h"
#include <Adafruit_BMP085.h>

// MQTT client - using descriptive client ID and topic
#define CLIENT_ID "esp32-sensors"
#define TOPIC_PREFIX "alli/ece140/sensors"

ECE140_MQTT mqtt(CLIENT_ID, TOPIC_PREFIX);
ECE140_WIFI wifi;   // wifi object 

// WiFi credentials
const char* ucsdUsername = UCSD_USERNAME;
const char* ucsdPassword = "H0ng@n041512&N";
const char* wifiSsid = WIFI_SSID;
const char* nonEnterpriseWifiPassword = NON_ENTERPRISE_WIFI_PASSWORD;

unsigned long lastPublish = 0; 

// void mqttCallback(char* topic, uint8_t* payload, unsigned int length){
//     Serial.print("Recieved message on topic: ");
//     Serial.println(topic); 
//     Serial.print("Message: ");
//     for(unsigned int i = 0; i < length; i++){
//         Serial.print((char)payload[i]);
//     }
//     Serial.println();
// }

Adafruit_BMP085 bmp;

void setup() {
    Serial.begin(115200);

    // Connecting to Wifi here:
    Serial.println("[Main] Connecting to Wifi...");
    // wifi.connectToWPAEnterprise(wifiSsid, ucsdUsername, ucsdPassword);
    
    // *** not UCSD wifi:  ***
    wifi.connectToWiFi(wifiSsid, nonEnterpriseWifiPassword);

    // To connect to the MQTT :
    Serial.println("Connecting to MQTT broker...");
    if (!mqtt.connectToBroker(1883)){
        Serial.println("Failed to connect to MQTT broker");
    }

    // if(mqtt.connectToBroker()){
    //     mqtt.publishMessage("status", "Sensor device connected!");
    //     Serial.println("Connected to MQTT broker");
    // }
    // else{
    //     Serial.println("Failed to connect to MQTT broker");
    // }

    //TA6 pt3: checking for bmp sensor
    if (!bmp.begin()) {
        Serial.println("Could not find a valid BMP180 sensor, check wiring!");
        while (1) {}
    }
}

void loop(){
    // Keep MQTT connection alive 
    mqtt.loop();

    // Read hall sensor 
    int hallValue = hallRead(); 

    // temp sensor 
    float temperature = temperatureRead();

    //temperature from bmp
    float temperatureBMP = bmp.readTemperature();

    //pressure from bmp
    int pressureBMP = bmp.readPressure();

    // String message = "{\"hall_sensor\": " + String(hallValue) + ", \"temperature\": " + String(temperature) + "}";
    String bmp_message = "{\"temperature_bmp\": " + String(temperatureBMP) + ", \"pressure_bmp\": " + String(pressureBMP) + "}";

    //lab 6
    // Serial.println("\n== Sensor Readings ===");
    // Serial.print("Hall Sensor: ");
    // Serial.println(hallValue);
    // Serial.print("Temperature: ");
    // Serial.print(temperature);
    // Serial.println("C");
    // Serial.println("==============");
    
    Serial.print("Temperature = ");
    Serial.print(temperatureBMP);
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(pressureBMP);
    Serial.println(" Pa");
    
    // Serial.println("Publishing Sensor Data...");
    // Serial.println(message);
    // mqtt.publishMessage("readings", message);

    Serial.println("Publishing BMP Sensor Data...");
    Serial.println(bmp_message);
    mqtt.publishMessage("readings", bmp_message);
    
    delay(2000); // Waiting 2 seconds before next reading 
}
    