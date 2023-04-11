//sudo i2cdetect -y 1
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdint.h>     // int16_t
#include <math.h>       // arctan (gcc ... -lwiringPi -lm)
#include <stdlib.h>     //exit(1)
#include <wiringPiSPI.h>
#include "I2C_MPU6050.h"
#include "SPI_MAX7219.h"

unsigned char arrowRight[8] = {0x08, 0x0C, 0x0E, 0xFF, 0xFF, 0x0E, 0x0C, 0x08};
unsigned char arrowLeft[8] = {0x10, 0x30, 0x70, 0xFF, 0xFF, 0x70, 0x30, 0x10};
unsigned char arrowDown[8] = {0x18, 0x18, 0x18, 0x18, 0xFF, 0x7E, 0x3C, 0x18};
unsigned char arrowUp[8] = {0x18, 0x3C, 0x7E, 0xFF, 0x18, 0x18, 0x18, 0x18};
unsigned char Still[8] = {0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00};

int main(void){
    wiringPiSetup();
    // setup SPI interface
    wiringPiSPISetup(spi0, 10000000);
    // set operational mode for max7219
    Init_MAX7219();
    // Thiết lập MPU6050
    Init_6050();

    // Setup giao tiep I2C
    int mpu = wiringPiI2CSetup(0x68); //mức 0 //mpu trong thư viện I2C_MPU6050.h

    //Check I2C connection
	if(wiringPiI2CReadReg8(mpu, 0x75) != 0x68) {
		printf("Connection failed!\n");
		exit(1);
	}

    delay(100); //100ms

    while (1)
    {
        // Đọc giá trị đo    
        // Ax, Ay, Az là giá trị gia tốc theo trục X, Y, Z
        float Ax = (float)readSensor(Acc_X)/4096.0; 
        float Ay = (float)readSensor(Acc_Y)/4096.0;
        float Az = (float)readSensor(Acc_Z)/4096.0;

        //Tính góc nghiêng theo trục X (Roll), Y (Pitch)
        //atan2 trả về radian => nhân 180 và chia pi để chuyển về độ
        float pitch = atan2(Ax, sqrt(pow(Ay,2) + pow(Az,2))) * 180 / 3.14;
        float roll = atan2(Ay, sqrt(pow(Ax,2) + pow(Az,2))) * 180 / 3.14;

        // printf("Góc nghiêng theo trục X, Y lần lượt: %.1f %.1f\n", pitch, roll);

        delay(100);
        for(int i = 0;  i < 8; i++){
            sendData(i+1, 
            (roll > 10.0) ? arrowLeft[i] :
            (roll < -10.0) ? arrowRight[i] :
            (pitch > 10.0) ? arrowUp[i] :
            (pitch < -10.0) ? arrowDown[i] :
            Still[i]
            );
        }
    }
    return 0;
}