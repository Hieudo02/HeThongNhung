#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdint.h>     
#include <math.h>       
#include <stdlib.h>     
#include <wiringPiSPI.h>

#define Sample_rate     25  
#define Config          26  
#define Gyro_config     27  
#define Acc_config      28
#define Interrupt       56
#define PWR_Managment   107

#define Acc_X           59 
#define Acc_Y           61 
#define Acc_Z           63 

#define spi0   0
uint8_t buf[2];

void sendData(uint8_t address, uint8_t data){
    buf[0] = address; 
    buf[1] = data; 
    wiringPiSPIDataRW(spi0, buf, 2);
}

void Init_MAX7219(void){
    sendData(0x09,0x00);
    sendData(0x0A, 9);
    sendData(0x0B, 7);
    sendData(0x0C, 1);
    sendData(0x0F, 0);
    for (int i = 1; i <= 8; i++) {
        sendData(i, 0);
    }
}
int mpu;

void Init_6050(void){
    wiringPiI2CWriteReg8(mpu, Sample_rate, 15);
    wiringPiI2CWriteReg8(mpu, Config, 0);
    wiringPiI2CWriteReg8(mpu, Gyro_config, 0x08);
    wiringPiI2CWriteReg8(mpu, Acc_config, 0x10);
    wiringPiI2CWriteReg8(mpu, Interrupt, 1);
    wiringPiI2CWriteReg8(mpu, PWR_Managment, 0x01);
}

int16_t readSensor(unsigned char sensor){ 
    int16_t high, low, data;
    high = wiringPiI2CReadReg8(mpu, sensor);
    low = wiringPiI2CReadReg8(mpu, sensor + 1);
    data = (high << 8) | low;
   return data;
}

unsigned char Snake[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

int transferHex(unsigned char hex){
    int num;
    switch (hex){
        case 0x80:
            num = 0;
            break;
        case 0x40:
            num = 1;
            break;
        case 0x20:
            num = 2;
            break;
        case 0x10:
            num = 3;
            break;
        case 0x08:
            num = 4;
            break;
        case 0x04:
            num = 5;
            break;
        case 0x02:
            num = 6;
            break;
        case 0x01:
            num = 7;
            break;
    }
    return num;
}
int Reverse_transferHex(int num){
    unsigned char hex;
    switch (num){
        case 0:
            hex = 0x80;
            break;
        case 1:
            hex = 0x40;
            break;
        case 2:
            hex = 0x20;
            break;
        case 3:
            hex = 0x10;
            break;
        case 4:
            hex = 0x08;
            break;
        case 5:
            hex = 0x04;
            break;
        case 6:
            hex = 0x02;
            break;
        case 7:
            hex = 0x01;
            break;
    }
    return hex;
}

bool flag = FALSE;

int* snakeLtoR(int ledState[]){
    int LR_address = ledState[0]; 
    int LR_data = transferHex(ledState[1]);
    static int ledState_LtoR[2];
    for(int i = LR_data; i <= 7; i++){
        sendData(LR_address, Snake[i]);
        delay(100);

        if(!flag){
            ledState_LtoR[0] = LR_address; //1
            ledState_LtoR[1] = Reverse_transferHex(i);
            break;
        }

        ledState_LtoR[0] = LR_address; //1
        ledState_LtoR[1] = Reverse_transferHex(i); //7 - R: 0x01
    }
    return ledState_LtoR;
}

int* snakeRtoL(int ledState[]){
    int RL_address = ledState[0]; // 8
    int RL_data = transferHex(ledState[1]); // 0x01 - 7 
    static int ledState_RtoL[2];
    for(int j = RL_data; j >= 0; j--){
        sendData(RL_address, Snake[j]);
        delay(100);
        //sendData(RL_address, 0);

        if(!flag){
            ledState_RtoL[0] = RL_address; // 8
            ledState_RtoL[1] = Reverse_transferHex(j);
            break;
        }

        ledState_RtoL[0] = RL_address; // 8
        ledState_RtoL[1] = Reverse_transferHex(j); //0 - 0x80 
    }
    return ledState_RtoL;
}

int* snakeDown(int ledState[]){
    int D_address = ledState[0]; //1
    int D_data = transferHex(ledState[1]); //0x80 - 0
    static int ledState_Down[2];
    for(int i = D_address; i <= 8; i++){
        sendData(i, Snake[D_data]); //snake[0] = 0x80
        delay(100);
        sendData(i, 0);

        if(!flag){
            ledState_Down[0] = i; //8
            ledState_Down[1] = Reverse_transferHex(D_data);
            break;
        }

        ledState_Down[0] = i; //8
        ledState_Down[1] = Reverse_transferHex(D_data); //0 - 0x80 
    }
    return ledState_Down;
}

int* snakeUp(int ledState[]){
    int U_address = ledState[0]; // 8
    int U_data = transferHex(ledState[1]); // 0x80 - 0
    static int ledState_Up[2]; 
    for(int i = U_address; i >= 1; i--){
        sendData(i, Snake[U_data]); 
        delay(100);
        sendData(i, 0);

        if(!flag){
            ledState_Up[0] = i; // 1
            ledState_Up[1] = Reverse_transferHex(U_data);
            break;
        }

        ledState_Up[0] = i; // 1
        ledState_Up[1] = Reverse_transferHex(U_data); //0 - 0x80
    }
    return ledState_Up;
}

int main(void){
    wiringPiSetup();
    wiringPiSPISetup(spi0, 10000000);
    Init_MAX7219();
    mpu = wiringPiI2CSetup(0x68); //mức 0
	if(wiringPiI2CReadReg8(mpu, 0x75) != 0x68) {
		printf("Connection failed!\n");
		exit(1);
	}
    Init_6050();
    delay(100);

    int POINT[2] = {1, 0x80}; 
    while (1)
    {
        float Ax = (float)readSensor(Acc_X)/4096.0; 
        float Ay = (float)readSensor(Acc_Y)/4096.0;
        float Az = (float)readSensor(Acc_Z)/4096.0;

        float pitch = atan2(Ax, sqrt(pow(Ay,2) + pow(Az,2))) * 180 / 3.14;
        float roll = atan2(Ay, sqrt(pow(Ax,2) + pow(Az,2))) * 180 / 3.14;

        //printf("Góc nghiêng theo trục X, Y lần lượt: %.1f %.1f\n", roll, pitch);

        delay(100);

        // if(roll > 10.0){
        //     int* result = snakeRtoL(POINT);
        //     POINT[0] = result[0];
        //     POINT[1] = result[1];

        // }
        // else if(roll < -10.0){
        //     int* result = snakeLtoR(POINT);
        //     POINT[0] = result[0];
        //     POINT[1] = result[1];
        // }
        // else if(pitch > 10.0){
        //     int* result = snakeUp(POINT);
        //     POINT[0] = result[0];
        //     POINT[1] = result[1];
        // }
        // else if(pitch < -10.0){
        //     int* result = snakeDown(POINT);
        //     POINT[0] = result[0];
        //     POINT[1] = result[1];
        // }
        // else{
        //     sendData(POINT[0], POINT[1]);
        // }
        switch (TRUE) {
            case (roll > 10.0):
                flag = TRUE;
                int* result = snakeRtoL(POINT);
                POINT[0] = result[0];
                POINT[1] = result[1];
                break;
            case (role < -10.0):
                flag = TRUE;
                int* result = snakeLtoR(POINT);
                POINT[0] = result[0];
                POINT[1] = result[1];
                break;
            case (pitch > 10.0):
                flag = TRUE;
                int* result = snakeUp(POINT);
                POINT[0] = result[0];
                POINT[1] = result[1];
            case (pitch < -10.0):
                flag = TRUE;
                int* result = snakeDown(POINT);
                POINT[0] = result[0];
                POINT[1] = result[1];
            default:
                sendData(POINT[0], POINT[1]);
        }
    }
    return 0;
}
