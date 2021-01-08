#include "Joystick.h"

const int JOYSTICK_TYPE = 11;

Joystick_ Joystick;
int maxVal = 127;


#include <WSWire.h>
union
{
  float rxFloat;
  byte rxArray[4];
} rxData;

float sensorData[2][3];
int16_t counter = -127;

void setup() {
  Joystick.setXAxisRange(-maxVal, maxVal);
  Joystick.setYAxisRange(-maxVal, maxVal);
  Joystick.setZAxisRange(-maxVal, maxVal);
  Joystick.setRxAxisRange(-maxVal, maxVal);

  Joystick.begin();
  pinMode(JOYSTICK_TYPE, INPUT_PULLUP);

  pinMode(A0, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);


  Wire.begin(4);
  Wire.onReceive(recieveI2C); // register event

  Serial.begin(9600);
}


void loop() {
  counter = (counter + 1) % maxVal;

  updateJoystick();
}


void updateJoystick() {
  // System Disabled
  if (digitalRead(A0) != 0)
  {
    // Turn indicator light off.
    digitalWrite(LED_BUILTIN, 0);
    return;
  }
  // Turn indicator light on.
  digitalWrite(LED_BUILTIN, 1);

  if (digitalRead(JOYSTICK_TYPE) == LOW) {
    Joystick.setXAxis(constrain(sensorData[0][1]*maxVal, -maxVal, maxVal));
    Joystick.setYAxis(constrain(-sensorData[1][1]*maxVal, -maxVal, maxVal));
    Joystick.setZAxis(constrain(sensorData[1][2]*maxVal, -maxVal, maxVal));
    Joystick.setRxAxis(constrain(sensorData[1][0]*maxVal, -maxVal, maxVal));
  } else {
    Joystick.setXAxis(constrain(sensorData[0][1]*maxVal, -maxVal, maxVal));
    Joystick.setYAxis(constrain(-sensorData[1][1]*maxVal, -maxVal, maxVal));
    Joystick.setZAxis(constrain(sensorData[1][2]*maxVal, -maxVal, maxVal));
    Joystick.setRxAxis(constrain(sensorData[0][2]*maxVal, -maxVal, maxVal));
  }
}


void recieveI2C(int howMany) {
  Serial.print("recieced wire: \t");
  int sensorIndex = Wire.read() - 1;
  Serial.print(sensorIndex);

  if (sensorIndex != 0 && sensorIndex != 1) {
    // first flush buffer, then quit
    while (Wire.available()) {
      Wire.read();
    }
    return;
  }

  Serial.print("\t");

  for (int dataIndex = 0; dataIndex < 3; dataIndex++) {
    for (int i = 0; i < 4; i++)
    {
      rxData.rxArray[i] = Wire.read();
    }
    sensorData[sensorIndex][dataIndex] = rxData.rxFloat;
    Serial.print(rxData.rxFloat);
    Serial.print(", ");

  }
  Serial.println();
}
