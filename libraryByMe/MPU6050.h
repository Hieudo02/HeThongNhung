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

void Init_6050(void);

// int16_t readSensor(unsigned char sensor); -> ham tra ve gia tri nen ko khai bao

void getRollPitch(float* roll, float* pitch);