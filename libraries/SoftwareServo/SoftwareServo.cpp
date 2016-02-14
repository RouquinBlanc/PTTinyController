#include <SoftwareServo.h>

SoftwareServo *SoftwareServo::first;
SoftwareServo *SoftwareServo::servos[MAX_SERVOS];
uint8_t SoftwareServo::count = 0;

#define NO_ANGLE (0xff)

SoftwareServo::SoftwareServo() : pin(0),angle(NO_ANGLE),pulse0(0),min16(34),max16(150),next(0)
{}

void SoftwareServo::setMinimumPulse(uint16_t t)
{
    min16 = t/16;
    update();
}

void SoftwareServo::setMaximumPulse(uint16_t t)
{
    max16 = t/16;
    update();
}

uint8_t SoftwareServo::attach(int pinArg)
{
    pin = pinArg;
    angle = NO_ANGLE;
    pulse0 = 0;
    next = first;
    first = this;
    digitalWrite(pin,0);
    pinMode(pin,OUTPUT);
    update();
    return 1;
}

void SoftwareServo::detach()
{
    for ( SoftwareServo **p = &first; *p != 0; p = &((*p)->next) ) {
        if ( *p == this) {
            *p = this->next;
            this->next = 0;
            return;
        }
    }
    update();
}

void SoftwareServo::write(int angleArg)
{
    if ( angleArg < 0) angleArg = 0;
    if ( angleArg > 180) angleArg = 180;
    angle = angleArg;
    // bleh, have to use longs to prevent overflow, could be tricky if always a 16MHz clock, but not true
    // That 64L on the end is the TCNT0 prescaler, it will need to change if the clock's prescaler changes,
    // but then there will likely be an overflow problem, so it will have to be handled by a human.
    pulse0 = (min16*16L*clockCyclesPerMicrosecond() + (max16-min16)*(16L*clockCyclesPerMicrosecond())*angle/180L)/64L;
    
    update();
}

uint8_t SoftwareServo::read()
{
    return angle;
}

uint8_t SoftwareServo::attached()
{
    for ( SoftwareServo *p = first; p != 0; p = p->next ) {
	   if ( p == this) return 1;
    }
    return 0;
}

void SoftwareServo::update()
{
    count = 0;
    
    for (SoftwareServo *p = first; p != 0; p = p->next) 
        if (p->pulse0) servos[count++] = p;
        
    if (count == 0)
        return;

    // bubblesort the SoftwareServos by pulse time, ascending order
    for(;;) {
        uint8_t moved = 0;
        for (uint8_t i = 1; i < count; i++) {
            if (servos[i]->pulse0 < servos[i-1]->pulse0) {
            SoftwareServo *t = servos[i];
            servos[i] = servos[i-1];
            servos[i-1] = t;
            moved = 1;
            }
        }
        if (!moved) break;
    }
}

void SoftwareServo::refresh()
{
    uint8_t i, ports;
    uint16_t base = 0;
    static unsigned long lastRefresh = 0;
    unsigned long m = millis();
    uint16_t go[count];

    // if we haven't wrapped millis, and 20ms have not passed, then don't do anything
    if ( m >= lastRefresh && m < lastRefresh + 20) return;
    lastRefresh = m;

    ports = 0;
    for (i = 0; i < count; i++) {
        SoftwareServo *s = servos[i];
        ports |= 1 << (PB0 + s->pin);
        go[i] = s->pulse0;
    }
    
    PORTB |= ports;

    uint8_t start = TCNT0;
    uint8_t now;
    uint8_t last = start;

    // Now wait for each pin's time in turn..
    for (i = 0; i < count; i++) {
        uint16_t stop = go[i] + start;
        
        // loop until we reach or pass 'go' time
        for (;;) {
            now = TCNT0;
            if (now < last) base += 256;
            last = now;

            if ((base + now) > stop) {
                digitalWrite(servos[i]->pin, 0);
                break;
            }
        }
    }
}
