/**** Giới thiệu code ***************
 *  Code sử dụng biến giới hạn là 25 cm để xác định khoảng cách cần dừng.
 *  Khi ta muốn thay đổi khoảng cách, ta thay đổi tham số "gioihan" đang để mặc định là 25. ( đơn vị cm)
  *  Xe ban đầu được đặt trạng thái UP, mục tiêu cuối cùng là trạng thái UP
 *  Khi gặp vật cản, xe tính khoảng cách hai bên trái phải, chọn bên có khoảng cách ngắn hơn, cập nhật trạng thái mới LEFT/RIGHT
 *  Khi ở trạng thái LEFT/RIGHT, xe sẽ dò phái trước và bên phải/trái, nếu bên phải/trái đi được thì rẽ sang hướng đó, cập nhật lại trạng thái mới
 *  Nếu không thì tiếp tục đi thẳng cho đến khi tìm được đường rẽ, nếu không đi thẳng được nữa thì xe quay đầu, dò hướng ngược lại
 *  Việc dò đường lặp đi lặp lại cho đến khi đến đích
 *  Các hàm tính khoảng cách, đi sang trái phải hay quay đầu cần thực hiện tinh chỉnh trên board Khi cả phía trước, trái ,phải đều có chướng ngại vật. xe đi lùi 1 đoạn, sau đó đo lại khoảng cách. ....
 */

#include <Servo.h>
Servo myservo;  // create servo object to control a servo
//int pos = 0;    // variable to store the servo position

enum car_status {
  UP, LEFT, RIGHT
};

/******** khai báo chân input/output**************/
const int trig = 6;     // chân trig của SRF-05.
const int echo = 5;     // chân echo của SRF-05.
int tien1=10;           // chân IN - A của Module L298.
int tien2=12;           // chân IN - C của Module L298. banh phai
int lui1=11;            // chân IN - D của Module L298. banh trai
int lui2=13;            // chân IN - B của Module L298. banh phai
int dongcoservo=9;      // chân Orange của Servo.

/********** Global variable *******************/
car_status my_status = UP;
int gioihanduoi = 25;
int gioihantren = 100;
int i;
int x=0;
unsigned long thoigian; // biến đo thời gian
int khoangcach;           // biến lưu khoảng cách
int khoangcachtrai, khoangcachphai;

/*********** protoype **************************/
void resetdongco();
void resetservo();

void dokhoangcach();

void dithang();
void disangtrai();
void disangphai();
void dilui();
void quaydau();

void quaycbsangphai();
void quaycbsangtrai();

void setup() {
  // put your setup code here, to run once:
    myservo.attach(dongcoservo);  // attaches the servo on pin 9 to the servo object
    pinMode(trig,OUTPUT);   // chân trig sẽ phát tín hiệu
    pinMode(echo,INPUT);    // chân echo sẽ nhận tín hiệu
    pinMode(tien1,OUTPUT);
    pinMode(tien2,OUTPUT);
    pinMode(lui1,OUTPUT);
    pinMode(lui2,OUTPUT);

    resetdongco();
   // Serial.begin(9600);
    resetservo();
    khoangcach = 0;
    delay(500);
}

void loop()
{
    if(my_status == UP) // thay bang while loop
    {
      dokhoangcach();
      if(khoangcach > gioihanduoi || khoangcach == 0)
      {
        dokhoangcach();
        if(khoangcach > gioihanduoi || khoangcach == 0)
        {
          dithang();
        }
      }
      else
      {
        quaycbsangtrai();
        khoangcachtrai=khoangcach;
        quaycbsangphai();
        khoangcachphai=khoangcach;
      
        if(khoangcachphai < 10 && khoangcachtrai < 10) // thu thay bang gioihanduoi
        {
          dilui(); // khong can
        }
        else
        {
          if(khoangcachphai < khoangcachtrai) 
          {
            disangphai();
            my_status = RIGHT;
          }
          else 
          {
            disangtrai();
            my_status = LEFT;
          }
          delay(500);
        }
      }
    }
    if(my_status == LEFT) // while loop ?
    {
      dokhoangcach();
      if(khoangcach > gioihanduoi || khoangcach == 0) 
      {
        dokhoangcach(); // khoang cach phia truoc
        if(khoangcach > gioihanduoi || khoangcach == 0) 
        {
          quaycbsangphai();
          if(khoangcach > gioihanduoi)
          {
            dithang(); // di thang them 1 doan
            delay(1000);
            disangphai();
            my_status = UP;
            delay(500);
          }
        }
        else
        {
          dithang();
          delay(500);
        }
      }
      else
      {
        quaydau();
        my_status = RIGHT;
        delay(500);
      }
    }
    if(my_status == RIGHT)
    {
      dokhoangcach();
      if(khoangcach > gioihanduoi || khoangcach == 0)
      {
        dokhoangcach(); // khoang cach phia truoc
        if(khoangcach > gioihanduoi || khoangcach == 0)
        {
          quaycbsangtrai();
          if(khoangcach > gioihanduoi)
          {
            dithang(); // di thang them 1 doan
            delay(1000);
            disangtrai();
            my_status = UP;
            delay(500);
          }
        }
        else
        {
          dithang();
          delay(500);
        }
      }
      else
      {
        quaydau();
        my_status = LEFT;
        delay(500);
      }
    }
}

void resetdongco()
{
  digitalWrite(tien1,LOW);
  digitalWrite(tien2,LOW);
  digitalWrite(lui1,LOW);
  digitalWrite(lui2,LOW); 
}

void resetservo()
{
   myservo.write(90);
}

/******** chương trình đo khoảng cách SRF04 ***************/
void dokhoangcach()
{
/* Phát xung từ chân trig */
    digitalWrite(trig,0);   // tắt chân trig
    delayMicroseconds(2);
    digitalWrite(trig,1);   // phát xung từ chân trig
    delayMicroseconds(10);   // xung có độ dài 5 microSeconds
    digitalWrite(trig,0);   // tắt chân trig
    
    /* Tính toán thời gian */
    // Đo độ rộng xung HIGH ở chân echo.
    thoigian = pulseIn(echo,HIGH);
    // Tính khoảng cách đến vật.
    khoangcach = int(thoigian/2/29.412);
}

/**************** Di chuyen xe ********************/
void dithang()
{
        digitalWrite(tien1,HIGH);
        digitalWrite(tien2,HIGH);
        //delay(2); 
}

void disangtrai() // quay trai
{
  resetdongco();
  digitalWrite(lui1,HIGH);
  delay(250); // chinh delay sao cho xe quay trai 90 do
  digitalWrite(lui1,LOW);
}

void disangphai() // quay phai
{
  resetdongco();
  digitalWrite(lui2,HIGH);
  delay(250); // chinh delay sao cho xe quay phai 90 do
  digitalWrite(lui2,LOW);
}

void dilui()
{
  resetdongco();
  for(i=0;i<20;i++); // ?
  digitalWrite(lui1,HIGH);
  digitalWrite(lui2,HIGH);
  delay(700); // chinh delay sao cho xe lui hop ly
  digitalWrite(lui1,LOW);
  digitalWrite(lui2,LOW);
}

void quaydau()
{
  dilui();
  if(my_status == LEFT)
  {
    disangtrai();
    delay(500);
    disangtrai();
  }
  else
  {
    disangphai();
    delay(500);
    disangphai();
  }
}

/*********** chương trình quay cảm biến xang trái *********/
void quaycbsangtrai() // do khoan cach ben trai xe
{
    myservo.write(180);              // tell servo to go to position in variable 'pos'
    delay(1000);
    dokhoangcach();
    resetservo();              // tell servo to go to position in variable 'pos'  
}

void quaycbsangphai() // do khoang cach ben phai xe
{
    myservo.write(0);              // tell servo to go to position in variable 'pos'
    delay(1000);
    dokhoangcach();
    resetservo();              // tell servo to go to position in variable 'pos'
}
