#include <algorithm>

#define RXD2 16
#define TXD2 17
#define led 23

#include "painlessMesh.h"
// Defining Mesh credentials
#define   MESH_PREFIX     "legendKon"
#define   MESH_PASSWORD   "sirHammad"
#define   MESH_PORT       5555
// Creating mesh object
painlessMesh mesh;

// Prototype Functions
void registerNetwork ();
void change ();
void sendList ();

// Declaring variables
unsigned long prev_time = millis();
int curr_nodes = 0;
int prev_nodes = 0;
String nodeListPackage = "2";

// *****************************************************
void receivedCallback( uint32_t from, String &msg ) {
  Serial.println("Received Callback Activated!");
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  Serial.println("----------------------------------------------------");
  // Sending data to bot via Port 2
  String msgRcvd = msg.c_str();
  Serial.println("Sending data package array to gateway: " + msgRcvd);
  char sendData[(msgRcvd.length() + 1)];
  msgRcvd.toCharArray(sendData, msgRcvd.length() + 1);
  Serial.println(sizeof(sendData));
  Serial.println(sendData);
  Serial2.write(sendData);
}
void newConnectionCallback(uint64_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}
// *****************************************************


void setup() {
  Serial.begin(115200);

  // Call to register network and respective functions
  registerNetwork();
  Serial.println ("Network Created and functions initialized.");

  Serial.println("Serial Communication Enabled.");
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Let's start");

}

void loop() {
  mesh.update();
  if (millis() - prev_time >= 10000) {
    Serial.println("\n10s past\n");
    change();
    prev_time = millis();
  }
}

void registerNetwork () {
  // set before init() so that you can see startup messages
  mesh.setDebugMsgTypes( ERROR | STARTUP );
  // Initializing mesh and registering functions
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.setRoot(true);
  mesh.setContainsRoot(true);
}

bool compareDescending(uint64_t a, uint64_t b) {
  return a > b;
}

void change () {
  std::list<uint32_t> nodes = mesh.getNodeList();
  curr_nodes = nodes.size();
  if (curr_nodes != prev_nodes) {
//    if(curr_nodes - prev_nodes < 0) {
//        ESP.restart();
//      }
    if (curr_nodes == 0) {
      Serial.println("Echo: No node connected to Bridge!");
      // Sending data to bot via Port 2
      String jsonString = nodeListPackage + "[]";
      char sendData[(jsonString.length() + 1)];
      jsonString.toCharArray(sendData, jsonString.length() + 1);
      Serial.println(sizeof(sendData));
      Serial.println(sendData);
      Serial2.write(sendData);
      prev_nodes = curr_nodes;
    }
    else {
      sendList();
      prev_nodes = curr_nodes;
    }
  }
}

void sendList() {
  std::list<uint32_t> nodes = mesh.getNodeList();
  curr_nodes = nodes.size();
  uint64_t nodeList [curr_nodes];
  SimpleList<uint32_t>::iterator node = nodes.begin();
  int iter = 0;
  Serial.println("Storing Node list in array: ");
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    nodeList [iter] = *node;
    iter++;
    node++;
  }
  Serial.println(" ");

  std::sort(nodeList, nodeList + sizeof(nodeList) / sizeof(nodeList[0]), compareDescending);

  Serial.println("Displaying sorted Node List Array: ");
  for (int i = 0; i < curr_nodes; i++) {
    Serial.printf("%u ", nodeList[i]);
  }
  Serial.println(" ");

  // Converting data to JSON array
  DynamicJsonDocument doc(1024);
  JsonArray jsonArray = doc.to<JsonArray>();
  for (int i = 0; i < curr_nodes; i++) {
    jsonArray.add(nodeList[i]);
  }
  String jsonString;
  serializeJson(doc, jsonString);
  jsonString = nodeListPackage + jsonString;

  Serial.println("Sending Node List Package Array: " + jsonString);
  // Sending data to bot via Port 2
  char sendData[(jsonString.length() + 1)];
  jsonString.toCharArray(sendData, jsonString.length() + 1);
  Serial.println(sizeof(sendData));
  Serial.println(sendData);
  Serial2.write(sendData);
}
