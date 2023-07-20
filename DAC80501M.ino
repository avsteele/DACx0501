#include <Arduino.h>
#include <Streaming.h>
#include <Wire.h>

// if A0 tied to AGND this is the I2C addr
#define DAC805_ADDR (0b1001000)
// if A0 tried to VCC this is the I2C addr
//#define DAC805_ADDR (0b10010010)

uint8_t _address = (byte)DAC805_ADDR;
TwoWire *_i2c;

#define COMMAND_DEVID  (uint8_t)(0x01)
#define COMMAND_CONFIG (uint8_t)(0x03)
#define COMMAND_GAIN   (uint8_t)(0x04)
#define COMMAND_DAC    (uint8_t)(0x08)

void setup()
{
    Serial.begin(9600);

    // &Wire is from Wire.h
    _i2c = &Wire;
    _i2c->begin();

    uint16_t devID =  readSequence(COMMAND_DEVID);
    Serial << "Device ID: " << _BIN(devID) << endl;
    delay(1000);

    Serial << "Attempting to Set gain to 1 (0 value)..." << endl;
    writeSequence(COMMAND_GAIN, 0x0000);
    delay(1000);

    Serial << "Attempting to set output to max ..." << endl;
    writeSequence(COMMAND_DAC, 0x0FFF);
    delay(1000);
    Serial << "Attempting to set output to min ..." << endl;
    writeSequence(COMMAND_DAC, 0x0000);
    delay(1000);

}

void loop()
{
    for(uint16_t i=0; i<0xFF; i++)
    {
        writeSequence(COMMAND_DAC, i*256);
    }
}

uint16_t readSequence(uint8_t command)
{    
    //Serial << "Attempting to read "<< command << endl;
    _i2c->beginTransmission(_address);
    _i2c->write(command);
    _i2c->endTransmission(false);
    delay(1);
    int read = _i2c->requestFrom(_address, (uint8_t)2);

    if(read != 2)
    {
        Serial << "Did not rad 2 BYTES! bytes read = " << read << endl;
    }
    uint8_t byte1 = _i2c->read();
    uint8_t byte2 = _i2c->read();

    return ((byte1 << 8) | byte2);
}

int writeSequence(uint8_t command, uint16_t value)
{
    _i2c->beginTransmission(_address);
    _i2c->write(command);
    _i2c->write((uint8_t)(value >> 8));
    _i2c->write((uint8_t)(value & 0xFF));
    _i2c->endTransmission(true);
}