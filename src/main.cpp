#include <Arduino.h>

#include <NimBLEDevice.h>

#include "blemidi/BLEMidiClient.h"

// #include "options.h"

#include "ledstrip.h"

#include <WiFi.h>
// #include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "weblog/weblog.h"
#include "OTA/OTA.h"

AsyncWebServer server(80);
WebLog weblog;
OTA ota;
const char *ssid = "mrichana";
const char *password = "2106009557";
const char *mDSNName = "pianolights";

hw_timer_t *timer = NULL;

#pragma region midi
void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  weblog.print("Note: ");
  weblog.print(channel);
  weblog.print(", ");
  weblog.print(note);
  weblog.print(", ");
  weblog.print(velocity);
  weblog.println("");
  if (channel == 15)
  {
  }
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  weblog.print("Note Off: ");
  weblog.print(channel);
  weblog.print(", ");
  weblog.print(note);
  weblog.print(", ");
  weblog.print(velocity);
  weblog.println("");
  if (channel == 15)
  {
  }
}

void onClientNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  weblog.print("Client Note: ");
  weblog.print(channel);
  weblog.print(", ");
  weblog.print(note);
  weblog.print(", ");
  weblog.print(velocity);
  weblog.println("");
  ledStrip.ledOnFromNote(note, velocity, LedStrip::LightType::random);
}

void onClientNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  weblog.print("Client Note Off: ");
  weblog.print(channel);
  weblog.print(", ");
  weblog.println(note);
  ledStrip.ledOffFromNote(note);
}

void onMidiConnect()
{
  weblog.println("Connected to Piano");
}

void onMidiDisconnect()
{
}

// bool connectToPiano()
// {
//   weblog.println("Establishing Piano connection");
//   ledStrip.reset();
//   int nDevices = BLEMidiClient.backgroundScan();
//   // int nDevices = BLEMidiClient.scan();
//   // if (nDevices > 0 && BLEMidiClient.connect(0))
//   // {
//   //   weblog.println("Piano connection established");
//   //   options.midiConnected = true;
//   //   ledStrip.setBrightness(128);
//   //   ledStrip.ledOn(0, CRGB::Green);
//   //   delay(200);
//   //   ledStrip.ledOff(0);
//   //   delay(200);
//   //   ledStrip.ledOn(0, CRGB::Green);
//   //   delay(200);
//   //   ledStrip.ledOff(0);
//   //   delay(200);
//   //   ledStrip.ledOn(0, CRGB::Green);
//   //   delay(200);
//   //   ledStrip.ledOff(0);
//   // }
//   // else
//   // {
//   //   weblog.println("Piano connection failed");
//   //   options.midiConnected = false;
//   //   ledStrip.ledOn(0, CRGB::Red);
//   //   delay(200);
//   //   ledStrip.ledOff(0);
//   //   delay(200);
//   //   ledStrip.ledOn(0, CRGB::Red);
//   //   delay(200);
//   //   ledStrip.ledOff(0);
//   //   delay(200);
//   //   ledStrip.ledOn(0, CRGB::Red);
//   //   delay(200);
//   //   ledStrip.ledOff(0);
//   //   // ESP32reset();
//   // }
//   return true;//options.midiConnected;
// }

#pragma endregion

#pragma region bt
// static NimBLEAdvertisedDevice *advDevice;
// static const std::string MIDI_SERVICE_UUID = "03b80e5a-ede8-4b33-a751-6ce34ec4c700";
// static const std::string MIDI_CHARACTERISTIC_UUID = "7772e5db-3868-4112-a1a9-f2669d106bf3";

// static bool startScan = false;
// static bool doConnect = false;

// class ClientCallbacks : public NimBLEClientCallbacks
// {
//   void onConnect(NimBLEClient *pClient)
//   {
//     Serial.println("Connected");
//   };

//   void onDisconnect(NimBLEClient *pClient)
//   {
//     Serial.print(pClient->getPeerAddress().toString().c_str());
//     Serial.println(" - Disconnected");
//     NimBLEDevice::getScan()->erase(pClient->getPeerAddress());
//     startScan = true;
//   }
// };

// class AdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks
// {

//   void onResult(NimBLEAdvertisedDevice *advertisedDevice)
//   {
//     Serial.print("Advertised Device found: ");
//     Serial.println(advertisedDevice->toString().c_str());
//     if (advertisedDevice->isAdvertisingService(NimBLEUUID(MIDI_SERVICE_UUID)))
//     {
//       Serial.println("Found Our Service");
//       /** stop scan before connecting */
//       NimBLEDevice::getScan()->stop();
//       /** Save the device reference in a global for the client to use*/
//       advDevice = advertisedDevice;
//       /** Ready to connect now */
//       doConnect = true;
//     }
//   };
// };

// static ClientCallbacks clientCB;

// void notifyCB(NimBLERemoteCharacteristic *pRemoteCharacteristic, uint8_t *data, size_t size, bool isNotify)
// {
//   weblog.print("Received data : ");
//   for (uint8_t i = 0; i < size; i++)
//     weblog.printf("%x ", data[i]);
//   weblog.println();

//   if (size < 3)
//   {
//     weblog.println("Invalid packet (size < 3)");
//     return;
//   }

//   if ((!(data[0] & 0b10000000)) || (!(data[1] & 0b10000000)))
//   {
//     weblog.println("Invalid packet");
//     return;
//   }

//   uint16_t currentTimestamp = ((data[0] & 0b111111) << 7);

//   uint8_t *ptr = &data[1];

//   uint8_t runningStatus = 0;

//   while (ptr - data < size)
//   {
//     if (ptr[0] & 0b10000000)
//     {
//       currentTimestamp = (currentTimestamp & 0b1111110000000) | (ptr[0] & 0b1111111);
//       ptr++;
//     }
//     if (ptr[0] & 0b10000000)
//     { // Full midi message
//       runningStatus = *ptr;
//       ptr++;
//     }
//     uint8_t command = runningStatus >> 4;
//     uint8_t channel = runningStatus & 0b1111;

//     switch (command)
//     {
//     case 0:
//       weblog.println("Invalid packet : a running status message must be preceded by a full midi message");
//       return;
//     case 0b1000:
//     { // Note off
//       uint8_t note = ptr[0];
//       uint8_t velocity = ptr[1];
//       ptr += 2;
//       // if (noteOffCallback != nullptr)
//       //   noteOffCallback(channel, note, velocity, currentTimestamp);
//       // weblog.printf("Note off, channel %d, note %d, velocity %d\n", channel, note, velocity);
//       break;
//     }

//     case 0b1001:
//     { // Note on
//       uint8_t note = ptr[0];
//       uint8_t velocity = ptr[1];
//       ptr += 2;
//       // if (noteOnCallback != nullptr)
//       //   noteOnCallback(channel, note, velocity, currentTimestamp);
//       weblog.print("Note on, channel ");
//       weblog.print(channel);
//       weblog.print(", note ");
//       weblog.print(note);
//       weblog.print(", velocity ");
//       weblog.println(velocity);
//       break;
//     }

//     case 0b1010:
//     { // Polyphonic after touch
//       uint8_t note = ptr[0];
//       uint8_t pressure = ptr[1];
//       ptr += 2;
//       // if (afterTouchPolyCallback != nullptr)
//       //   afterTouchPolyCallback(channel, note, pressure, currentTimestamp);
//       // weblog.printf("Polyphonic after touch, channel %d, note %d, pressure %d\n", channel, note, pressure);
//       break;
//     }

//     case 0b1011:
//     { // Control Change
//       uint8_t controller = ptr[0];
//       uint8_t value = ptr[1];
//       ptr += 2;
//       // if (controlChangeCallback != nullptr)
//       //   controlChangeCallback(channel, controller, value, currentTimestamp);
//       // weblog.printf("Control Change, channel %d, controller %d, value %d\n", channel, controller, value);

//       break;
//     }

//     case 0b1100:
//     { // Program Change
//       uint8_t program = ptr[0];
//       ptr++;
//       // if (programChangeCallback != nullptr)
//       //   programChangeCallback(channel, program, currentTimestamp);
//       // weblog.printf("Program Change, channel %d, program %d\n", channel, program);
//       break;
//     }

//     case 0b1101:
//     { // After touch
//       uint8_t pressure = ptr[0];
//       ptr++;
//       // if (afterTouchCallback != nullptr)
//       //   afterTouchCallback(channel, pressure, currentTimestamp);
//       // weblog.printf("After touch, channel %d, pressure %d\n", channel, pressure);
//       break;
//     }

//     case 0b1110:
//     { // Pitch bend
//       uint8_t lsb = ptr[0];
//       uint8_t msb = ptr[1];
//       ptr += 2;
//       // if (pitchBendCallback != nullptr)
//       //   pitchBendCallback(channel, lsb, msb, currentTimestamp);
//       // weblog.printf("Pitch bend, channel %d, lsb %d, msb %d\n", channel, lsb, msb);
//       uint16_t integerPitchBend = ((msb & 127) << 7) | (lsb & 127);
//       // if (pitchBendCallback2 != nullptr)
//       //   pitchBendCallback2(channel, integerPitchBend, currentTimestamp);
//       weblog.printf("Integer value of pitch bend : %d\n", integerPitchBend);
//       break;
//     }

//     case 0b1111:
//       // weblog.println("System common message, not implemented yet");
//       return;
//       break;

//     default:
//       // weblog.println("Invalid packet");
//       return;
//       break;
//     }
//   }
// }

// void btSetup()
// {
//   NimBLEDevice::init("");
//   NimBLEScan *pScan = NimBLEDevice::getScan();
//   pScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
//   pScan->setInterval(45);
//   pScan->setWindow(15);
//   pScan->setActiveScan(false);
//   pScan->start(0, false);
// }

// bool connectToServer()
// {
//   NimBLEClient *pClient = NULL;

//   if (NimBLEDevice::getClientListSize())
//   {
//     pClient = NimBLEDevice::getClientByPeerAddress(advDevice->getAddress());
//     if (pClient)
//     {
//       weblog.println("Got client by address");
//       if (!pClient->connect(advDevice, false))
//       {
//         weblog.println("Reconnect failed");
//         return false;
//       }
//       weblog.println("Reconnected client");
//     }
//     else
//     {
//       pClient = NimBLEDevice::getDisconnectedClient();
//     }
//   }

//   if (!pClient)
//   {
//     if (NimBLEDevice::getClientListSize() >= NIMBLE_MAX_CONNECTIONS)
//     {
//       weblog.println("Max clients reached - no more connections available");
//       return false;
//     }

//     pClient = NimBLEDevice::createClient();

//     weblog.println("New client created");

//     pClient->setClientCallbacks(&clientCB, false);
//     pClient->setConnectionParams(12, 12, 0, 51);
//     pClient->setConnectTimeout(30);

//     if (!pClient->connect(advDevice))
//     {
//       /** Created a client but failed to connect, don't need to keep it as it has no data */
//       NimBLEDevice::deleteClient(pClient);
//       Serial.println("Failed to connect, deleted client");
//       return false;
//     }
//   }

//   if (!pClient->isConnected())
//   {
//     if (!pClient->connect(advDevice))
//     {
//       Serial.println("Failed to connect");
//       return false;
//     }
//   }

//   Serial.print("Connected to: ");
//   Serial.println(pClient->getPeerAddress().toString().c_str());
//   Serial.print("RSSI: ");
//   Serial.println(pClient->getRssi());

//   /** Now we can read/write/subscribe the charateristics of the services we are interested in */
//   NimBLERemoteService *pSvc = nullptr;
//   NimBLERemoteCharacteristic *pChr = nullptr;
//   NimBLERemoteDescriptor *pDsc = nullptr;

//   pSvc = pClient->getService(NimBLEUUID(MIDI_SERVICE_UUID));
//   if (pSvc)
//   { /** make sure it's not null */
//     pChr = pSvc->getCharacteristic(NimBLEUUID(MIDI_CHARACTERISTIC_UUID));

//     if (!pChr)
//     {
//       pClient->disconnect();
//       return false;
//     }

//     if (pChr->canNotify())
//     {
//       // if(!pChr->registerForNotify(notifyCB)) {
//       if (!pChr->subscribe(true, notifyCB))
//       {
//         /** Disconnect if subscribe failed */
//         pClient->disconnect();
//         return false;
//       }
//     }
//   }

//   return true;
// }

// void btLoop()
// {
//   if (startScan == true)
//   {
//     NimBLEDevice::getScan()->start(0, false);
//     startScan = false;
//   }

//   if (doConnect == false)
//     return;
//   if (connectToServer())
//   {
//     weblog.println("Success! we should now be getting notifications, stopped scanning for more!");
//   }
//   else
//   {
//     weblog.println("Failed to connect, starting scan");
//     startScan = true;
//   }
//   doConnect = false;
//   // NimBLEDevice::getScan()->start(0, true);
// }

#pragma endregion

void setup()
{
  Serial.begin(115200);
  weblog.init(server, Serial);
  // weblog.init(Serial);
  // weblog.init(server);
  weblog.println("Booting");

  // options.init();
  // ledStrip.init();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    weblog.println("WiFi connection failed...");
  }

  weblog.print("IP Address: ");
  weblog.println(WiFi.localIP());

  server.begin();
  ota.setup();
  ledStrip.setup();
  // btSetup();

  // BLEMidiServer.begin("Piano Lights");

  // BLEMidiServer.setOnConnectCallback(onMidiConnect);
  // BLEMidiServer.setOnDisconnectCallback(onMidiDisconnect);

  // BLEMidiServer.setNoteOnCallback(onNoteOn);
  // BLEMidiServer.setNoteOffCallback(onNoteOff);

  weblog.println("Intializing client");

  BLEMidiClient.begin("Piano Lights Client");
  BLEMidiClient.setNoteOnCallback(&onClientNoteOn);
  BLEMidiClient.setNoteOffCallback(&onClientNoteOff);
  BLEMidiClient.setOnDisconnectCallback(&onMidiDisconnect);

  // BLEMidiClient.enableDebugging(); // Uncomment to see debugging messages from the library

  BLEMidiClient.backgroungScan();

  // connectToPiano();
}

void loop()
{
  static unsigned long lastTime = millis();
  unsigned long time = millis();
  unsigned long elapsedTime = time - lastTime;

  ota.loop();
  BLEMidiClient.loop();

  // if (pBLEScan) {
  //   bleloop();
  // }

  lastTime = time;
}
