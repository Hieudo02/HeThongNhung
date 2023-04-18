#define DECODE_MODE     0x09
#define INTENSITY       0x0A
#define SCAN_LIMIT      0x0B
#define SHUTDOWN        0x0C
#define DISPLAY_TEST    0x0F

#define SPI_CHANNEL     0
#define SPI_SPEED       10000000

void Init_MAX7219(void);

void sendData(unsigned char address, unsigned char data);
