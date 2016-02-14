# PTTinyController
Control a PT platform using an AT-Tiny85, 2 servos and I2C communication

This project is based on the excellent work from aboudou and his excellent [Tiny Laser Turret](https://github.com/aboudou/tinylaserturret) project. But while his goal was to have fun with cats (which is a noble goal), my is to control a camera.

Using a NoIR camera attached to a raspberry pi 2, I needed to control the pan and tilt, as well as the infrared lighting, which needs to be powered externally. On a setup with a regular camera camera, you could replace the lighting by a laser... and scare cats!

In order for the raspberry to control the pan/tilt/lighting, I used TinyWireS library to implement an I2C slave interface.

As of today, I have already built a small setup, with LEGOs and a small poorly soldered chip. I validated that the laser can be controlled, but neither mounted it, nor tried to mount the IR lighting.

![Current version](imgs/lego_platform.jpg "Current version of my platform")

## Requirements
- An ATtiny85 microcontroller, with 8MHz internal clock
- A BCU-81 transistor or equivalent to drive the optional module (laser, IR lighting)
- A 330Ω resistor to drive BCU-81 transistor.
- A pan & tilt platform with servos to orient the camera. Can be build with a 3D printer or bought online.
- An external power supply for servos and optional module (assumed regulated)

This is it! While the attiny85 will be powered and controlled by the raspberry, you still need an external 5V power supply to power up your payload.

## TODOS
This project is not terminated. Please do not hesitate to provide comments and suggestions!
Remaining things:
- Properly documenting everything
- Checking that every lib used is properly referenced, and that we have the correct licences with them
- Try replacing SoftwareServo by an interrupt-based lib
- Have a full 360 rotation!

[To be continued]
