=bool flagreadButton = false;
#include <ArduinoJson.h>
String str = "[3,5]";
int count = 0;

int sendCoor = 11;

void sendMessage();

#define RXD2 16
#define TXD2 17
#define pushButton_pin 15
#define LED_pin   23
unsigned long previous_time = millis();
unsigned long curr_time = 0;

void IRAM_ATTR readButton()
{
  curr_time = millis() - previous_time;
  //  Serial.println(curr_time);
  if (curr_time > 2000) {
    flagreadButton = true;
    //    Serial.println(flagreadButton);
  }
  previous_time = millis();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(LED_pin, OUTPUT);
  pinMode(pushButton_pin, INPUT);
  attachInterrupt(pushButton_pin, readButton, FALLING);

  Serial.println("Serial Communication Enabled.");
  Serial2.begin(19200, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Let's start");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (flagreadButton) {
    //    digitalWrite(LED_pin, !digitalRead(LED_pin));
    //    Serial.println("Calling Send Message");
    sendMessage();
  }

  if (Serial2.available() >= 1) {
    char number = Serial2.read();

    if (number == '@') {
      Serial.println(number);
      number = Serial2.read();
      Serial.println(number);
      if (number == '&') {
        Serial.println(number);
        //    digitalWrite(LED_pin, HIGH);
        Serial.println("Data Received...");
        String incoming = Serial2.readStringUntil('\n');
        Serial.println(incoming);
        //    digitalWrite(LED_pin, LOW);
      }
    }
  }
}

void sendMessage() {
  String jsonString = "@&1[3,5]";
  Serial.println("Sending Node List Package Array: " + jsonString);
  // Sending data to bot via Port 2
  char sendData[(jsonString.length() + 1)];
  jsonString.toCharArray(sendData, jsonString.length() + 1);
  Serial.println(sizeof(sendData));
  Serial.println(sendData);
  Serial2.write(sendData);

  sendCoor++;
  flagreadButton = false;
}
