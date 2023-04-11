#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>

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

void Init(void){
    // set decode mode: 0x09FF
    sendData(0x09,0xFF); // thiết lập chế độ giải mã: tất cả các bit đều được giải mã
    // set intensity: 0x0A09
    sendData(0x0A, 9); // thiết lập độ sáng: mức độ sáng được thiết lập là 9/15
    // scan limit: 0x0B07
    sendData(0x0B, 7); // thiết lập giới hạn quét: mỗi số đều được quét
    // no shutdown, turn off display test
    sendData(0x0C, 1); // tắt chế độ kiểm tra và khởi động mạch
    sendData(0x0F, 0); // khởi động mạch
}

void display_number(uint32_t num){
    // count the number of digits (đếm số lượng chữ số trong num)
    uint8_t count=1;
    uint32_t n = num;
    while(n/10){ //VD: số 123, ta chia 123 cho 10 = 12 dư 3 (tách được số 3),...
        count++;
        n = n/10; 
    }
    // set scanlimit
    sendData(0x0B, count-1);
    /*
    Hàm sendData(0x0B, count-1) được sử dụng để gửi dữ liệu qua giao tiếp SPI đến IC Max7219, để cài đặt số lượng các LED 7 thanh được kích hoạt. 
    Tham số đầu tiên 0x0B là địa chỉ thanh ghi (register) của Max7219 để cấu hình số lượng LED 7 thanh, 
    và tham số thứ hai count-1 là giá trị để cấu hình số lượng LED 7 thanh. 
    Vì mỗi LED 7 thanh được kích hoạt sẽ tốn thêm bộ nhớ và tài nguyên hệ thống, 
    nên chỉ kích hoạt đúng số lượng cần thiết là tối ưu.

    Ở đây, count-1 là số lượng LED 7 thanh cần kích hoạt để hiển thị giá trị của biến num trên module LED 7 thanh. 
    Vì count là số lượng chữ số của num, nên count-1 là số lượng LED 7 thanh cần kích hoạt để hiển thị giá trị của num. 
    Ví dụ, nếu num có giá trị là 123 thì count là 3 và count-1 là 2, nghĩa là cần kích hoạt 2 LED 7 thanh để hiển thị số 123.
    
    Nếu giá trị của count-1 là:

    0: Tức là số num chỉ có 1 chữ số. Led 7 thanh sẽ hiển thị số đó.
    1: Tức là số num có 2 chữ số. Led 7 thanh sẽ hiển thị cả 2 chữ số đó.
    2: Tức là số num có 3 chữ số. Led 7 thanh sẽ hiển thị cả 3 chữ số đó.
    3: Tức là số num có 4 chữ số. Led 7 thanh sẽ hiển thị cả 4 chữ số đó.
    4: Tức là số num có 5 chữ số. Led 7 thanh sẽ hiển thị cả 5 chữ số đó.
    5: Tức là số num có 6 chữ số. Led 7 thanh sẽ hiển thị cả 6 chữ số đó.
    Trong trường hợp số chữ số của num lớn hơn 6 thì chỉ có 6 chữ số đầu tiên sẽ được hiển thị trên led 7 thanh, còn những chữ số còn lại sẽ không được hiển thị.

    */
    // dislay number
    for(int i=0; i<count;i++){
        sendData(i+1,num%10);
        num = num/10;
    }
    /*
    Lưu ý rằng, ta phải gửi các chữ số lên các LED theo thứ tự từ phải sang trái, 
    tức là ta phải bắt đầu gửi chữ số cuối cùng của num lên LED cuối cùng, rồi tiếp tục gửi chữ số trước đó lên LED trước đó và cứ tiếp tục như vậy. 
    Do đó, ta sử dụng biến i+1 trong hàm sendData để đảm bảo rằng ta gửi chữ số thứ i của num lên LED thứ i+1.
    */
}


int main(void){
    // setup SPI interface
    wiringPiSPISetup(spi0, 10000000);
    // set operational mode for max7219
    Init();
    for(int i=0; i<1000;i++){
        display_number(i);
        delay(200);
    }
    
    return 0;
}