# Drone-hand-controller
This is a project that alows you to control a drone simulator with the rotation of your hands rather then with joysticks.
![](showcase.gif)

## Setup

1. You must 3D print HandSensorCase.stl and MPUcase.stl and super glue them together, do this twice. These can be attached to your hand by sliding them onto a strap around your hand.
Place the components into the 3d printed parts. 

2. Use the schematics to wire the components together. You also need a power supply, I connected a 9V to the hand controllers through VIN.

3. Then upload the code to the associated microcontroller.
  * ControllerReciever.ino gets uploaded to arduino leonardo
  * radioReciever.ino gets uploaded to arduino nano connected to the leonardo
  * handTransmitter.ino gets uploaded to arduino nano on your hand. Make sure to have the radio address be different for each hand, 1000 for the left and 2000 for the right.

4. Test everything is working at https://gamepad-tester.com/

5. Run a simulator (I used a free simulator: https://github.com/CurryKitten/CurryKitten-Sim). 
