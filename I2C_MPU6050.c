#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdint.h>     // int16_t
#include <math.h>       // arctan (gcc ... -lwiringPi -lm)

#define Sample_rate     25  // 0x19 (de co so 16 or 10 deu dc)
#define Config          26  // 0x1A
#define Gyro_config     27  // 0x1B
#define Acc_config      28
#define Interrupt       56
#define PWR_Managment   107

//3 trục x, y, z
#define Acc_X           59 // 59-high, 60-low
#define Acc_Y           61 // 61-high, 62-low
#define Acc_Z           63 // 63-high, 64-low

int mpu;

void Init_6050(void){
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
    high = wiringPiI2CReadReg8(mpu, sensor + 1);
    // data là gộp 2 thanh ghi lại vs nhau (thành thanh ghi 16 bits)
    data = (high << 8) | low; // '|' là phép OR
    /* 
    data đang là giá trị raw, chứ không phải giá trị của gia tốc 
    Muốn ra giá trị gia tốc phải lấy data chia cho độ nhạy tương ứng (LSB Sensitivity)
    Set bên trên là +-8g tương ứng với 4096 LSB/g 
    */

   return data;
}

int main(void){
    // Setup giao tiếp I2C
    mpu = wiringPiI2CSetup(0x68); //mức 0

    //Check I2C connection
	if(wiringPiI2CReadReg8(mpu, 0x75) != 0x68) {
		printf("Connection failed!\n");
		exit(1);
	}

    // Thiết lập chế độ đo cho MPU6050
    Init_6050();

    // Đọc giá trị đo
    while (1)
    {
        // Ax, Ay, Az là giá trị gia tốc theo trục X, Y, Z
        float Ax = (float)readSensor(Acc_X)/4096.0; 
        float Ay = (float)readSensor(Acc_Y)/4096.0;
        float Az = (float)readSensor(Acc_Z)/4096.0;

        //Tính góc nghiêng theo trục X (Roll), Y (Pitch)
        //atan2 trả về radian => nhân 180 và chia pi để chuyển về độ
        float pitch = atan2(Ax, sqrt(pow(Ay,2) + pow(Az,2))) * 180 / M_PI;
        float roll = atan2(Ay, sqrt(pow(Ax,2) + pow(Az,2))) * 180 / M_PI;

        delay(100);
        for(int i = 0;  i < 8; i++){
            printf("%s %.1f %.1f\n", 
            (roll > 10.0) ? "trai" :
            (roll < -10.0) ? "phai" :
            (pitch > 10.0) ? "len" :
            (pitch < -10.0) ? "xuong" :
            "can bang", roll, pitch
            );
        }
    }

    return 0;
}