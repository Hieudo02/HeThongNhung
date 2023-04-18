//SPI
#include <wiringPiSPI.h>
#include "MAX7219.h"
#include <stdint.h>

uint8_t buf[2];

void Init_MAX7219(void){
    // setup SPI interface
    wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED);
    
    // set decode mode: 0x09FF
    sendData(DECODE_MODE,0xFF);
    // set intensity: 0x0A09
    sendData(INTENSITY, 9);
    // scan limit: 0x0B07
    sendData(SCAN_LIMIT, 7);
    // no shutdown, turn off display test
    sendData(SHUTDOWN, 1);
    sendData(DISPLAY_TEST, 0);
}

void sendData(unsigned char address, unsigned char data){
    buf[0] = address;
    buf[1] = data;
    wiringPiSPIDataRW(SPI_CHANNEL, buf, 2);
}