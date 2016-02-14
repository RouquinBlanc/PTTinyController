/* PTController for ATTiny85
 * by Jonathan Martin <therouquinblanc@gmail.com>
 * 
 * Allow control of 2 servos and 1 additional power switch via I2C slave mode.
 * Easily controllable via an Arduino with Wire library, or a Raspberry Pi with i2c commands.
 * 
 * The intent of this controller is to control a camera PT platform.
 * The additional switch could control for example a laser, or IR lights in case of a NoIR camera.
 */ 

#include <Arduino.h>
#include <SoftwareServo.h>
#include <TinyWireS.h>

/*
 * I2C Slave address. Change it if you have another device with same address in your setup
 */
#define I2C_SLAVE_ADDRESS 0x4

/*
 * Default sensors pins and positions
 */
#define PANSERVOPIN 4         // Pan servo signal pin
#define PAN_START 90          // Pan servo initial position

#define TILTSERVOPIN 3        // Tilt servo signal pin
#define TILT_START 90         // Tilt servo initial position

#define LASERPIN 1            // Laser module signal pin
#define LASER_START 0         // Laser module initial state

/*
 * Variables
 */
SoftwareServo panServo;
SoftwareServo tiltServo;
int pan = PAN_START;
int tilt = TILT_START;
int laser = LASER_START;

/*
 * I2C control code, from TinyWireS example.
 * @TODO To be moved outside of the script.
 */
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

volatile uint8_t i2c_regs[] =
{
    0x42,        // Answer to question of the universe (just for fun, and check if it works!)
    LASER_START, // Multi purpose pin status (0 is LOW, other is HIGH)
    PAN_START,   // PAN
    TILT_START,  // TILT
};

volatile byte reg_position;

void requestEvent()
{  
    TinyWireS.send(i2c_regs[reg_position]);
    // Increment the reg position on each read, and loop back to zero
    reg_position++;
    if (reg_position >= sizeof(i2c_regs))
    {
        reg_position = 0;
    }
}

void receiveEvent(uint8_t howMany)
{
    if (howMany < 1)
    {
        // Sanity-check
        return;
    }
    if (howMany > TWI_RX_BUFFER_SIZE)
    {
        // Also insane number
        return;
    }

    reg_position = TinyWireS.receive();
    howMany--;
    if (!howMany)
    {
        // This write was only to set the buffer for next read
        return;
    }
    while(howMany--)
    {
        i2c_regs[reg_position] = TinyWireS.receive();
        reg_position++;
        if (reg_position >= sizeof(i2c_regs))
        {
            reg_position = 0;
        }
    }
}

void setup() {
  // Servo init
  panServo.attach(PANSERVOPIN);
  tiltServo.attach(TILTSERVOPIN);
  
  // Move servos
  panServo.write(PAN_START);
  tiltServo.write(TILT_START);

  // Init Laser pin
  pinMode(LASERPIN, OUTPUT);
  digitalWrite(LASERPIN, LASER_START ? HIGH : LOW);

  // I2C init
  TinyWireS.begin(I2C_SLAVE_ADDRESS); // join i2c network
  TinyWireS.onReceive(receiveEvent);
  TinyWireS.onRequest(requestEvent);
}

void loop() {
  // Check if pan changed
  if (i2c_regs[2] != pan) {
    panServo.write(i2c_regs[2]);
    pan = i2c_regs[2];
  }

  // Check if tilt changed
  if (i2c_regs[3] != tilt) {
    tiltServo.write(i2c_regs[3]);
    tilt = i2c_regs[3];
  }
  
  // Check if laser changed
  if (i2c_regs[1] != laser) {
    digitalWrite(LASERPIN, i2c_regs[1] ? HIGH : LOW);
    laser = i2c_regs[1];
  }

  // Check I2C
  TinyWireS_stop_check();

  // Refresh servos
  SoftwareServo::refresh();
}
