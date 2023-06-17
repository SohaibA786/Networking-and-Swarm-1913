#include <algorithm>
#include <ArduinoJson.h>

#include "painlessMesh.h"
// Defining Mesh credentials
#define   MESH_PREFIX     "AirUni"
#define   MESH_PASSWORD   "sirHammad"
#define   MESH_PORT       5555
// Creating mesh object
painlessMesh mesh;

#define RXD2 16
#define TXD2 17

// Prototype Functions
void registerNetwork ();
void change ();
void swarm ();
void data_received(String);

// Declaring variables
unsigned long prev_time = millis();
int curr_nodes = 0;
int prev_nodes = 0;
uint64_t bridge = 533232621;
float total_area = 12;
float patch_area = 0;
int target[] = {2, 5, 7, 11};
char shutDown = '#';
char restartNetwork = '$';
bool dataIncoming = false;
String incoming = "";
String bridgePackage = "1";
String swarmPackage = "1";
char robotPackage = '1';
unsigned int networkUpdateTime = millis();
bool sendReady = false;
String serialData = "";

// *****************************************************
void receivedCallback( uint64_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  Serial.println("----------------------------------------------------");
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

  //pinMode(16, INPUT_PULLUP);
  Serial.begin(115200);
  Serial2.begin(19200, SERIAL_8N1, RXD2, TXD2);

  // Call to register network and respective functions
  registerNetwork();
  Serial.println ("Network Created and functions initialized.");

  //  String str = "qw1[0,1]";
  //  Serial.println("Sending package array: " + str);
  //
  //  // Sending data to bot via Port 2
  //  char sendData[(str.length() + 1)];
  //  str.toCharArray(sendData, str.length() + 1);
  //  Serial.println(sizeof(sendData));
  //  Serial.println(sendData);
  //  Serial2.write(sendData);

}

void loop() {
  mesh.update();
  if (millis() - prev_time >= 1000) {
    Serial.println("\n10s past\n");
    change();
    prev_time = millis();
  }

  if (Serial2.available() > 1) {

    char number = Serial2.read();

    if (number == 'q') {
      Serial.println("q");
      number = Serial2.read();
      if (number == 'w') {
        Serial.println(number);

        incoming = Serial2.readStringUntil('\n');
        Serial.print("Data Received: ");
        Serial.println(incoming);
        if (incoming != "") {
          dataIncoming = true;
        }

        if (dataIncoming) {
          data_received();
        }
      }
    }
  }

  if (curr_nodes = 0 && (millis() - networkUpdateTime > 60000)) {
    ESP. restart();
  }
  if (sendReady && (millis() - networkUpdateTime> 20000)) {
    // Sending data to bot via Port 2
    char sendData[(serialData.length() + 1)];
    serialData.toCharArray(sendData, serialData.length() + 1);
    Serial.println(sizeof(sendData));
    Serial.println(sendData);
    Serial2.write(sendData);
    serialData = "";
    sendReady = false;
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
  //  mesh.setRoot(true);
  mesh.setContainsRoot(true);
}

void change () {
  std::list<uint32_t> nodes = mesh.getNodeList();
  curr_nodes = nodes.size();
  bool skip = false;
  if (curr_nodes != prev_nodes) {
    networkUpdateTime = millis();
    //    if(curr_nodes - prev_nodes < 0) {
    //        ESP.restart();
    //      }
    if (curr_nodes == 1) {
      SimpleList<uint32_t>::iterator node = nodes.begin();
      while (node != nodes.end()) {
        if (bridge == *node) {
          skip = true;
          break;
        }
        node++;
      }
    }

    if (curr_nodes == 0 || skip) {
      Serial2.write('0');
      Serial.print("No node connected: ");
      Serial.println(curr_nodes);
      skip = false;
      prev_nodes = curr_nodes;
    }
    else {
      prev_nodes = curr_nodes;
      Serial.println("Connected Network changed. Calling swarm function.");
      SimpleList<uint32_t>::iterator node = nodes.begin();
      while (node != nodes.end()) {
        Serial.printf(" %u", *node);
        node++;
      }
      Serial.println(" ");
      swarm();
    }
  }
}

bool compareDescending(uint64_t a, uint64_t b) {
  return a > b;
}

void swarm() {
  Serial.println("Proceeding with swarm function.");
  std::list<uint32_t> nodes = mesh.getNodeList();
  curr_nodes = nodes.size();
  uint64_t my_node_id = mesh.getNodeId();
  bool skip = false;

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println(" ");

  node = nodes.begin();
  while (node != nodes.end()) {
    if (bridge == *node) {
      skip = true;
      break;
    }
    node++;
  }

  int nodeListSize = 0;
  if (skip)
    nodeListSize = curr_nodes;
  else
    nodeListSize = curr_nodes + 1;

  Serial.println(nodeListSize);
  uint64_t nodeList [nodeListSize];
  nodeList[0] = my_node_id;

  // Storing the node list in nodeList array
  node = nodes.begin();
  int iter = 1;
  while (node != nodes.end() && iter < nodeListSize) {
    if (bridge != *node) {
      nodeList[iter] = *node;
      Serial.println(nodeList[iter]);
      iter++;
    }
    node++;
  }
  Serial.println("Unsorted Node List Array:");
  for (int i = 0; i < nodeListSize; i++) {
    Serial.print(nodeList[i]);
    Serial.print(" ");
  }
  Serial.println(" ");

  // Sorting available nodes in descending order
  std::sort(nodeList, nodeList + sizeof(nodeList) / sizeof(nodeList[0]), compareDescending);

  int pos = -1;
  Serial.println("Displaying sorted Node List Array and finding node position:");
  for (int i = 0; i < nodeListSize; i++) {
    //    Serial.print(nodeList[i]);
    //    Serial.print(" ");
    Serial.printf("%u ", nodeList[i]);
    if (nodeList[i] == my_node_id) {
      pos = i + 1;
    }
  }
  Serial.println(" ");

  // Calculate the patch area for each node
  patch_area = total_area / nodeListSize;
  Serial.printf("Patch Area: %f\n", patch_area);

  // Allocating patch area w.r.t position
  float my_patch_start = patch_area * (pos - 1);
  float my_patch_end = patch_area * pos;
  Serial.printf("my_patch_start: %f\n", my_patch_start);
  Serial.printf("my_patch_end: %f\n", my_patch_end);
  int patch_index_start = -1;
  int patch_index_end = -1;

  Serial.println("Finding target points in our area vicinity:");
  for (int i = 0; i < sizeof(target) / sizeof(target[0]); i++) {
    if (target[i] >= my_patch_start) {
      patch_index_start = i;
      Serial.printf("Found Patch start Index: %i\n", patch_index_start);
      break;
    }
  }
  if (target[patch_index_start] >= my_patch_end || patch_index_start == -1) {
    patch_index_start = -1;
    patch_index_end = -1;
    Serial.printf("target[patch_index_start] >= my_patch_end || patch_index_start == -1 is true : %i\n", target[patch_index_start]);
  }
  else {
    for (int i = patch_index_start; i < sizeof(target) / sizeof(target[0]); i++) {
      if (target[i] >= my_patch_end) {
        patch_index_end = i;
        break;
        Serial.printf("Found Patch end Index: %i\n", patch_index_end);
      }
    }
    if (patch_index_end == -1) {
      patch_index_end = (sizeof(target) / sizeof(target[0]));
    }
  }
  Serial.printf("Start Index: %i\n", patch_index_start);
  Serial.printf("End Index: %i\n", patch_index_end);
  Serial.println(" ");

  // Converting data to JSON array
  DynamicJsonDocument doc(128);
  JsonArray jsonArray = doc.to<JsonArray>();
  jsonArray.add(patch_index_start);
  jsonArray.add(patch_index_end);
  String jsonString;
  serializeJson(doc, jsonString);
  jsonString = "qw" + swarmPackage + jsonString;
  Serial.println("Sending package array: " + jsonString);
  serialData = jsonString;
  sendReady = true;
}

void data_received () {
  if (incoming[0] == shutDown) {
    mesh.stop();
  }
  else if (incoming[0] == restartNetwork) {
    registerNetwork ();
  }
  else if (incoming[0] == robotPackage) {
    // Converting data to JSON array
    incoming = incoming.substring(1);

    DynamicJsonDocument doc(512);
    JsonArray jsonArray = doc.to<JsonArray>();
    jsonArray.add(mesh.getNodeId());

    // Converting incoming json coordinate array back to json
    DynamicJsonDocument doc2(512);
    deserializeJson(doc2, incoming);
    // Retrieve the JSON array from the parsed JSON object
    JsonArray jsonArray2 = doc2.as<JsonArray>();

    jsonArray.add(jsonArray2);

    String jsonString;
    serializeJson(doc, jsonString);
    jsonString = bridgePackage + jsonString;
    Serial.println("Sending package array: " + jsonString);
    Serial.println("Message Sent: " + jsonString);
  }
  else {
    Serial.println("Incoming Data not valid: " + incoming);
  }
  dataIncoming = false;
  incoming = "";
}
