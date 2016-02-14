# External libraries

Those libraries must be installed:
- [TinyWireS](https://github.com/rambo/TinyWire/tree/master/TinyWireS) I2C-slave interface (untouched)
- [SoftwareServo](https://github.com/nicolaskruchten/arduino/tree/master/libraries/SoftwareServo) servo control lib (modified)

Ideally I would like to replace SoftwareServo by a fully interrupt-based servo lib.
I tried to use Servo8Bit lib but failed to have it working properly (to be investigated).
