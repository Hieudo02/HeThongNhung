//sudo i2cdetect -y 1
//gcc KTra_2.c -L. -lMPU6050 -lMAX7219 -o KTra_2 -lwiringPi -lm
#include <stdio.h>
#include <wiringPi.h>
#include <stdint.h>     // int16_t
#include <math.h>       // arctan (gcc ... -lwiringPi -lm); fabs()
#include <stdlib.h>     //exit(1)

#include "MPU6050.h"
#include "MAX7219.h"

void display_float(float num, uint8_t dec){
    int32_t integerPart = num;
    int32_t fractionalPart = (num - integerPart) * pow(10,dec);
    int32_t number = integerPart*pow(10,dec) + fractionalPart;

    // count the number of digits
    uint8_t count=1;
    int32_t n = number;
    while(n/10){
        count++;
        n = n/10;
    }
    //printf("count: %d\n", count);

    // set scanlimit (digit)
    sendData(0x0B, 7); //digit là vị trí của chữ số cần hiển thị (bắt đầu từ 0)

    if (num >= 0 && num < 1){
        sendData(2, 0|0x80);
        for (int i = 0; i < count; i++) {
            sendData(i+1, number % 10);
            number = number / 10;
        }
    }
    else{
        for(int i=0; i<count; i++){
            if(i==dec)
                sendData(i+1,(number%10)|0x80); // turn on dot segment 
            else
                sendData(i+1,number%10);

            number = number/10;
        }
    }
}

void display_float2(float num, uint8_t dec){
    int32_t integerPart = num;
    int32_t fractionalPart = (num - integerPart) * pow(10,dec);
    int32_t number = integerPart*pow(10,dec) + fractionalPart;

    // count the number of digits
    uint8_t count=1;
    int32_t n = number;
    while(n/10){
        count++;
        n = n/10;
    }
    //printf("count: %d\n", count);

    // set scanlimit (digit)
    sendData(0x0B, 7); //digit là vị trí của chữ số cần hiển thị (bắt đầu từ 0)

    if (num >= 0 && num < 1){
        sendData(6, 0|0x80);
        for (int i = 0; i < count; i++) {
            sendData(i+5, number % 10);
            number = number / 10;
        }
    }
    else{
        for(int i=0; i<count; i++){
            if(i==dec)
                sendData(i+5,(number%10)|0x80); // turn on dot segment 
            else
                sendData(i+5,number%10);

            number = number/10;
        }
    }
}

int main(void){
    wiringPiSetup();
    // set operational mode for max7219
    Init_MAX7219();
    delay(100); //100ms
    // Thiết lập chế độ đo cho MPU6050
    Init_6050();

    int ledDisplayCount = 0; //Cach 2
    while (1)
    {
        float roll, pitch;
        getRollPitch(&roll, &pitch);

        printf("Góc nghiêng theo trục X, Y lần lượt: %.1f %.1f\n", roll, pitch);

        // send zeroes to all digits to clear the display
        for(int i = 1; i <= 8; i++) {
            sendData(i, 0x0F);
        }

        /*Cach 1: 
    //     float a = roll;
    //     float b = pitch;

    //     if(fabs(roll) > 45){
    //         // a = 45;
    //         display_float(fabs(b), 1);
    //         display_float2(fabs(a), 1);
            
    //         for(int i = 5; i <= 8; i++){
    //             sendData(i, 0x0F);
    //         }
    //         delay(100);
            
    //         display_float(fabs(b), 1);
    //     }
        
    //     if(fabs(pitch) > 45){
    //         // b = 45;
    //         display_float(fabs(b), 1);
    //         display_float2(fabs(a), 1);
            
    //         for(int i = 1; i <= 4; i++){
    //             sendData(i, 0x0F);
    //         }
    //         delay(100);

    //         display_float2(fabs(a), 1);
    //     }
        
    //     display_float(fabs(b), 1);
    //     display_float2(fabs(a), 1);
    //     delay(100);
    */
        /*Cach 2:
        if (fabs(roll) > 45.0) {
            // Chỉ hiển thị LED nếu đếm chẵn, le thi khong lam gi ca
            if (ledDisplayCount % 2 == 0) {
                display_float2(fabs(roll), 1);
            }
            display_float(fabs(pitch), 1);
            ledDisplayCount++; // Tăng giá trị biến đếm sau mỗi lần lặp
        } 
        else if (fabs(pitch) > 45.0){
            // Chỉ hiển thị LED nếu đếm chẵn
            if (ledDisplayCount % 2 == 0) {
                display_float(fabs(pitch), 1);
            }
            display_float2(fabs(roll), 1);
            ledDisplayCount++; // Tăng giá trị biến đếm sau mỗi lần lặp
        }
        else {
            // Hiển thị LED bình thường nếu không vượt quá ngưỡng 45 độ
            display_float(fabs(pitch), 1);
            display_float2(fabs(roll), 1);
            ledDisplayCount = 0; // Đặt lại giá trị biến đếm nếu không vượt quá ngưỡng 45 độ
        }
        delay(100);
        */
        
    }
    return 0;
}