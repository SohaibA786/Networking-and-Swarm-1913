#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "dev"
#define WIFI_PASSWORD "12345678"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCWf5cM5vOoN8jb2SdAz3y0lafvtZYa798"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://data-receiver-1913-default-rtdb.asia-southeast1.firebasedatabase.app/"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

#define RXD2 16
#define TXD2 17

String incoming = "";
char nodeListPackage = '2';
char bridgePackage = '1';

#define led 23
void pushRTDB(String);

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Serial.println("RTDB Connection Established!");

  Serial.println("Serial Communication Enabled.");
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Let's start");
}

void loop() {
  if (Serial2.available() >= 1) {
    digitalWrite(led, HIGH);
    Serial.println("Data Received...");
    incoming = Serial2.readStringUntil('\n');
    Serial.println(incoming);
    pushRTDB(incoming);
    incoming = "";
    digitalWrite(led, LOW);
  }
}

void pushRTDB(String incoming) {

  if (incoming[0] == nodeListPackage) {
    String jsonString = incoming.substring(1);
    Serial.println(jsonString);

    FirebaseJson json;
    json.setJsonData(jsonString);

    if (Firebase.ready() && signupOK) {
      Serial.println("Ready to send to Firebase: ");
      Serial.println(jsonString);
      if (Firebase.RTDB.setJSON(&fbdo, "Nodelist", &json)) {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }

  }
  else if (incoming[0] == bridgePackage) {
    String jsonString = incoming.substring(1);
    Serial.println(jsonString);

    // Parse the jsonString into a JSON object
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, jsonString);

    // Retrieve the JSON array from the parsed JSON object
    JsonArray jsonArray = doc.as<JsonArray>();
    Serial.printf("Bridge Data Packet Size: %i", jsonArray.size());

    String path = "/data/" + jsonArray[0].as<String>();
    Serial.println("Bridge Data Sending Path: \n" + path);

    FirebaseJson json;
    json.setJsonData(jsonArray[1].as<String>());

    if (Firebase.ready() && signupOK) {
      Serial.println("Ready to send to Firebase: ");
      Serial.println(jsonArray[1].as<String>());


      if (Firebase.RTDB.setJSON(&fbdo, path, &json)) {
      Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }
  }
  else {
    Serial.println("Invalid Data Received!");
  }
}
