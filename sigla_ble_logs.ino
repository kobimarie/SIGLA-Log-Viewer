// SIGLA - BLE log transfer for the web app (XIAO ESP32-S3, Arduino IDE)
// Sends each saved session as one text line ending in '\n', then "END\n".
// Line format: date,time,minutes,extensorOnly,coContraction,flexorDominant,rest,signalFaults
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <LittleFS.h>

#define SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define RX_UUID      "6e400002-b5a3-f393-e0a9-e50e24dcca9e"  // app writes "SEND"
#define TX_UUID      "6e400003-b5a3-f393-e0a9-e50e24dcca9e"  // device notifies log lines
#define LOG_FILE     "/sessions.csv"

BLECharacteristic *txChar;
volatile bool sendRequested = false;
bool connected = false;

class ServerCb : public BLEServerCallbacks {
  void onConnect(BLEServer*) override { connected = true; }
  void onDisconnect(BLEServer* s) override { connected = false; s->getAdvertising()->start(); }
};
class RxCb : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* c) override {
    String v = String(c->getValue().c_str());
    if (v.startsWith("SEND")) sendRequested = true;
  }
};

// Call this at the end of each training session.
void saveSession(const String &line) {
  File f = LittleFS.open(LOG_FILE, FILE_APPEND);
  if (f) { f.println(line); f.close(); }
}

// Sends text in small chunks; the app joins them back into lines.
void notifyText(const String &s) {
  const size_t CHUNK = 20;
  for (size_t i = 0; i < s.length(); i += CHUNK) {
    String part = s.substring(i, min(s.length(), i + CHUNK));
    txChar->setValue((uint8_t*)part.c_str(), part.length());
    txChar->notify();
    delay(15);
  }
}

void sendLogs() {
  File f = LittleFS.open(LOG_FILE, FILE_READ);
  if (f) {
    while (f.available()) {
      String line = f.readStringUntil('\n');
      line.trim();
      if (line.length()) notifyText(line + "\n");
    }
    f.close();
  }
  notifyText("END\n");
}

void setup() {
  Serial.begin(115200);
  LittleFS.begin(true);
  BLEDevice::init("SIGLA");
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new ServerCb());
  BLEService *svc = server->createService(SERVICE_UUID);
  txChar = svc->createCharacteristic(TX_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  txChar->addDescriptor(new BLE2902());
  BLECharacteristic *rx = svc->createCharacteristic(RX_UUID, BLECharacteristic::PROPERTY_WRITE);
  rx->setCallbacks(new RxCb());
  svc->start();
  server->getAdvertising()->addServiceUUID(SERVICE_UUID);
  server->getAdvertising()->start();

  // For testing only: create one example session if the log is empty.
  if (!LittleFS.exists(LOG_FILE)) saveSession("2026-11-11,09:00,45,30,3,2,1,0");
}

void loop() {
  if (connected && sendRequested) { sendRequested = false; sendLogs(); }
  delay(20);
}
