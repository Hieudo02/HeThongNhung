//sudo i2cdetect -y 1
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdint.h>     // int16_t
#include <math.h>       // arctan (gcc ... -lwiringPi -lm)
#include <stdlib.h>     //exit(1)
#include <wiringPiSPI.h>

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

#define spi0   0
uint8_t buf[2]; // Tạo mảng 2 phần tử để lưu dữ liệu gửi đi qua giao tiếp SPI

void sendData(uint8_t address, uint8_t data){
    buf[0] = address; //địa chỉ thanh ghi cần gửi
    buf[1] = data; //dữ liệu cần gửi
    wiringPiSPIDataRW(spi0, buf, 2); //gửi mảng 'buf' chứa địa chỉ và dữ liệu qua giao tiếp SPI trên kênh 'spi0'
    /*
    wiringPiSPIDataRW(a, b, c): dùng để gửi và nhận dữ liệu qua kênh SPI
        a: int channel 
        -> kênh SPI muốn gửi và nhận dữ liệu
            trong bài có sử dụng '#define spi0 0', đó là kênh SPI thứ nhất của raspberry 
        b: unsigned char *data
        -> con trỏ đến mảng dữ liệu cần gửi và nhận, mảng này phải được cấp phát trước và có độ dài đủ để chứa các byte dữ liệu cần truyền qua giao tiếp SPI
            trong code, biến 'buf' là 1 mảng 'uint8_t' với độ dài là 2
        c: int len
        -> độ dài (số byte) của dữ liệu cần gửi or nhận
    */
}
void Init_MAX7219(void){
    // set decode mode: 0x09FF
    sendData(0x09,0x00); // thiết lập chế độ giải mã: tắt chế độ giải mã, tức là chế độ hiển thị tùy chỉnh, để có thể kiểm soát từng LED một.
    // set intensity: 0x0A09
    sendData(0x0A, 9); // thiết lập độ sáng: mức độ sáng được thiết lập là 9/15
    // scan limit: 0x0B07
    sendData(0x0B, 7); // thiết lập giới hạn quét: 8 (tương đương 8x8)
    // no shutdown, turn off display test
    sendData(0x0C, 1); // tắt chế độ kiểm tra và khởi động mạch
    // displaytest
    sendData(0x0F, 0); // khởi động mạch

    //Tắt tất cả led
    for (int i = 1; i <= 8; i++) {
        sendData(i, 0);
    }
}
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

unsigned char arrowRight[8] = {
    0x08,
    0x0C,
    0x0E,
    0xFF,
    0xFF,
    0x0E,
    0x0C,
    0x08
};
unsigned char arrowLeft[8] = {
    0x10,
    0x30,
    0x70,
    0xFF,
    0xFF,
    0x70,
    0x30,
    0x10
};
unsigned char arrowDown[8] = {
    0x18,
    0x18,
    0x18,
    0x18,
    0xFF,
    0x7E,
    0x3C,
    0x18
};
unsigned char arrowUp[8] = {
    0x18,
    0x3C,
    0x7E,
    0xFF,
    0x18,
    0x18,
    0x18,
    0x18
};
unsigned char Still[8] = {
    0x00,
    0x00,
    0x00,
    0x18,
    0x18,
    0x00,
    0x00,
    0x00
};

int main(void){
    wiringPiSetup();

    // setup SPI interface
    wiringPiSPISetup(spi0, 10000000);
    // set operational mode for max7219
    Init_MAX7219();

    // Setup giao tiep I2C
    mpu = wiringPiI2CSetup(0x68); //mức 0

    //Check I2C connection
	if(wiringPiI2CReadReg8(mpu, 0x75) != 0x68) {
		printf("Connection failed!\n");
		exit(1);
	}

    delay(100); //100ms
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
        // printf("%s %.1f %.1f\n", 
        // (roll > 10.0) ? "trai" :
        // (roll < -10.0) ? "phai" :
        // (pitch > 10.0) ? "len" :
        // (pitch < -10.0) ? "xuong" :
        // "can bang", roll, pitch
        // );
    }
    return 0;
}