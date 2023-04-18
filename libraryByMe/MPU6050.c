//I2C
#include <wiringPiI2C.h>
#include "MPU6050.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int mpu;

void Init_6050(void){
    // Setup giao tiep I2C
    mpu = wiringPiI2CSetup(0x68); //mức 0

    //Check I2C connection
	if(wiringPiI2CReadReg8(mpu, 0x75) != 0x68) {
		printf("Connection failed!\n");
		exit(1);
	}

    // register 25->28, 56, 107

    // Sample_rate: 500Hz
    wiringPiI2CWriteReg8(mpu, Sample_rate, 15);
    // Không sử dụng nguồn xung ngoài, tắt DLFP
    wiringPiI2CWriteReg8(mpu, Config, 0);
    // gyro FS: +- 500 o/s
    wiringPiI2CWriteReg8(mpu, Gyro_config, 0x08);
    // acc FS: +- 8g
    wiringPiI2CWriteReg8(mpu, Acc_config, 0x10);
    // Mở interrupt của data ready
    wiringPiI2CWriteReg8(mpu, Interrupt, 1);
    // Chọn nguồn xung Gyro X
    wiringPiI2CWriteReg8(mpu, PWR_Managment, 0x01);
}

int16_t readSensor(unsigned char sensor){ //parameter là address của sensor muốn đọc
    // Tạo biến chứa mức cao, thấp, và ghép lại 2 giá trị
    int16_t high, low, data;
    high = wiringPiI2CReadReg8(mpu, sensor);
    low = wiringPiI2CReadReg8(mpu, sensor + 1);
    // data là gộp 2 thanh ghi lại vs nhau (thành thanh ghi 16 bits)
    data = (high << 8) | low; // '|' là phép OR
    /* 
    data đang là giá trị raw, chứ không phải giá trị của gia tốc 
    Muốn ra giá trị gia tốc phải lấy data chia cho độ nhạy tương ứng (LSB Sensitivity)
    Set bên trên là +-8g tương ứng với 4096 LSB/g 
    */

   return data;
}

/*  USING:
        float roll, pitch;
        getRollPitch(&roll, &pitch);
*/

void getRollPitch(float* roll, float* pitch){
        // Ax, Ay, Az là giá trị gia tốc theo trục X, Y, Z
        float Ax = (float)readSensor(Acc_X)/4096.0; 
        float Ay = (float)readSensor(Acc_Y)/4096.0;
        float Az = (float)readSensor(Acc_Z)/4096.0;

        //Tính góc nghiêng theo trục X (Roll), Y (Pitch)
        //atan2 trả về radian => nhân 180 và chia pi để chuyển về độ
        *pitch = atan2(Ax, sqrt(pow(Ay,2) + pow(Az,2))) * 180 / 3.14;
        *roll = atan2(Ay, sqrt(pow(Ax,2) + pow(Az,2))) * 180 / 3.14;
}

