#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <stdint.h>

#define spi0    0
unsigned char data[2];
uint8_t mssv[8] = {2,0,1,4,6,5,6,7};

void send_data(unsigned char address, unsigned char value){
    data[0]= address;
    data[1]= value;
    wiringPiSPIDataRW(spi0, data, 2);
}

void initMax7219(void){
    // set decode mode: 0x09FF
    send_data(0x09, 0xff);
    // set intensity: 0x0A09
    send_data(0x0A, 9);
    // set scanlimit
    send_data(0x0B, 7);
    // no shutdown, turn off display test
    send_data(0x0C, 1);
    send_data(0x0F, 0);
}

int main (void){
    // setup SPI interface
    wiringPiSPISetup(spi0, 10000000);
    initMax7219();
    for(int i=0;i<8;i++){
        send_data(i+1, mssv[7-i]);
    }
  
    return 0;
}