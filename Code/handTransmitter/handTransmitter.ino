
// MPU

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps_V6_12.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 MPU(0x68);

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t MPUbuffer[64]; // FIFO storage buffer

float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// RADIO

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "20000";

unsigned long lastSendTimestamp;
int waitTime = 100;

void setup() {
  Serial.begin(9600);
  setupRadio();
  setupMPU();
}


void loop() {
  bool dataReady = updateMPU();

  if (dataReady && millis() - lastSendTimestamp > waitTime) {
    sendData();
    lastSendTimestamp = millis();
  }
}


void sendData() {
  for (int i = 0; i < 3; i ++) {
    Serial.print(ypr[i]);
    Serial.print(",");
  }
  Serial.println();

  radio.write(&ypr, sizeof(ypr));

}

bool updateMPU() {
  // orientation/motion vars
  Quaternion q;           // [w, x, y, z]         quaternion container
  VectorFloat gravity;    // [x, y, z]            gravity vector

  // if programming failed, don't try to do anything
  if (!dmpReady) return false;

  bool dataReady = MPU.dmpGetCurrentFIFOPacket(MPUbuffer);

  if (dataReady) {
    // display Euler angles in degrees
    MPU.dmpGetQuaternion(&q, MPUbuffer);
    MPU.dmpGetGravity(&gravity, &q);
    MPU.dmpGetYawPitchRoll(ypr, &q, &gravity);

    return true;
  }

  return false;
}

void setupRadio() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void setupMPU() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  Serial.println(F("Initializing I2C devices..."));
  MPU.initialize();

  Serial.println(F("Testing device connections..."));
  Serial.println(MPU.testConnection() ? F(" connection successful") : F(" connection failed"));

  devStatus = MPU.dmpInitialize();

  MPU.CalibrateAccel(6);
  MPU.CalibrateGyro(6);
  MPU.PrintActiveOffsets();

  Serial.println(F("Enabling DMP..."));
  MPU.setDMPEnabled(true);
  MPU.getIntStatus();
  Serial.println(F("DMP ready! Waiting for first interrupt..."));
  dmpReady = true;

  // get expected DMP packet size for later comparison
  packetSize = MPU.dmpGetFIFOPacketSize();
}
