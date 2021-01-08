
#include <WSWire.h>
union
{
  float txFloat;
  byte txArray[4];
} txData;

// RADIO
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte addresses[][6] = {"10000", "20000"};


void setup() {
  Wire.begin();


  // RADIO
  Serial.begin(9600);
  Serial.println("Listening...");
  radio.begin();

  radio.openReadingPipe(1, addresses[0]);
  radio.openReadingPipe(2, addresses[1]);

  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

}

void loop() {

  byte pipeIndex = 0;
  if (radio.available(&pipeIndex)) {
    float sensorData[3] = {};
    radio.read(&sensorData, sizeof(sensorData));

    // weird phenomena where it reads pipe 1 as 5
    if (pipeIndex == 5) {
      pipeIndex = 1;
    }
    sendI2C(pipeIndex, sensorData);
  }
}

void serialVisual(int pipeIndex, float sensorData[3]) {

  int percision = 20;
  Serial.print(pipeIndex);

  for (int i = 0; i < 3; i++) {
    Serial.print("   |   ");
    float val = sensorData[i];
    int pos = int(val * percision);
    for (int x = -percision; x < percision; x++) {
      Serial.print((x == pos) ? "@" : "-");
    }
  }

  Serial.println();
}

void sendToProcessing(int pipeIndex, float sensorData[2]) {
  Serial.print("S,");
  Serial.print(pipeIndex);
  for (int i = 0; i < 2; i ++) {
    Serial.print(",");
    Serial.print(sensorData[i], 10);
  }
  Serial.print(",E");
}

void sendI2C(int pipeIndex, float sensorData[3]) {
  Serial.print("recieved data: ");
  Serial.print(pipeIndex);
  Serial.print(",");
  Serial.print(sensorData[0]);
  Serial.print(",");
  Serial.print(sensorData[1]);
  Serial.print(",");
  Serial.print(sensorData[2]);
  Serial.println();

  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(pipeIndex);
  for (int i = 0; i < 3; i ++) {
    txData.txFloat = sensorData[i];
    Wire.write(txData.txArray, sizeof(txData.txArray));
  }

  byte error = Wire.endTransmission();    // stop transmitting
  if (error != 0) {
    Serial.print("WSWire error: ");
    Serial.println(error);
  }
}
