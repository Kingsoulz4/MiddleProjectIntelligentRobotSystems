/****** Thong tin ********************
written by Linh Kiện TCP
Linh Kiện TCP – số 70/ngõ 40, Tạ Quang Bửu, Hai Bà Trưng, Hà Nội.
Web: Linhkientcp.vn
Fb : https://www.facebook.com/linhkienTCP/
Bán hàng: 0966.129.629                 Kỹ thuật: 01663.860.326

***************************/


/**** Giới thiệu code ***************
 *  Code sử dụng biến giới hạn là 25 cm để xác định khoảng cách cần dừng.
 *  Khi ta muốn thay đổi khoảng cách, ta thay đổi tham số "gioihan" đang để mặc định là 25. ( đơn vị cm)
 *  Xe ưu tiên rẽ trái trong các trường  hợp phía trước có vật cản. 
 *  Ví dụ: khi khoang cách phía trước <25 , cảm biến đo bên trái trước, nếu không có vật cản thì xe rẽ trái. nếu bên trái cũng có vật cản
 *  lúc đó xe mới kiểm tra bên phải.
 *  Khi cả phía trước, trái ,phải đều có chướng ngại vật. xe đi lùi 1 đoạn, sau đó đo lại khoảng cách. ....
 */

#include <Servo.h>

#define PI 3.1415926535897932384626433832795

struct Position  
{
  float x, y;
};

Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position
/******** khai báo chân input/output**************/

const int trig = 6;     // chân trig của SRF-05.
const int echo = 5;     // chân echo của SRF-05.
int tien1=10;           // chân IN - A của Module L298.
int tien2=12;           // chân IN - C của Module L298.
int lui1=11;            // chân IN - D của Module L298.
int lui2=13;            // chân IN - B của Module L298.
int dongcoservo=9;      // chân Orange của Servo.

int limitThreshold = 15;
int i;
int x=0;
unsigned long timeCollideObstacle; // biến đo thời gian
int distanceToObstacle;           // biến lưu khoảng cách
int khoangcachtrai,khoangcachphai;
float velocity = (float) 220/3;
float angular_velocity = (float)52/15;
float deltaAngle = 45;
float deltaMove = 1;
float currentAngle = 90;
struct Position currentPosition;
struct Position destinationPos; 

void dokhoangcach();
void goAhead(int duongdi);
void disangtrai();
void disangphai();
void dilui();
void resetWheels();
void quaycbsangphai();
void quaycbsangtrai();
void setup() {
    Serial.begin(9600); //Mở cổng Serial ở 9600
  // put your setup code here, to run once:
    myservo.attach(9);  // attaches the servo on pin 9 to the servo object
    pinMode(trig,OUTPUT);   // chân trig sẽ phát tín hiệu
    pinMode(echo,INPUT);    // chân echo sẽ nhận tín hiệu
    
    pinMode(tien1,OUTPUT); 
    pinMode(tien2,OUTPUT); 
    pinMode(lui1,OUTPUT); 
    pinMode(lui2,OUTPUT); 
    digitalWrite(tien1,LOW);
    digitalWrite(tien2,LOW);
    digitalWrite(lui1,LOW);
    digitalWrite(lui1,LOW);
   // Serial.begin(9600);
    myservo.write(90);    
    delay(500);
   
    
    
}

void loop() 
{   
    findWay();
    //avoidObstacle();
    //Serial.println(khoangcach);
}

void avoidObstacle()
{
    distanceToObstacle=0;
    dokhoangcach();

    Serial.print("Khoảng cách: ");
    Serial.print(distanceToObstacle);
    Serial.println("cm");
    
    if(distanceToObstacle>limitThreshold||distanceToObstacle==0) 
    {
      dokhoangcach();
       if(distanceToObstacle>limitThreshold||distanceToObstacle==0) 
        {
          goAhead();
          delay(500); 
        }   
    }
    else
    {
      
      resetWheels();
      quaycbsangtrai();
      
      khoangcachtrai=distanceToObstacle;
     
      quaycbsangphai();
      khoangcachphai=distanceToObstacle;
      if(khoangcachphai<10&&khoangcachtrai<10){
        dilui();
      }
      else
      {
        if(khoangcachphai>khoangcachtrai) 
        {
          disangtrai();
          delay(500);
        }
        if(khoangcachphai<khoangcachtrai) 
        {
          disangphai();
          delay(500);
        }
      }
    }
}

void goAhead()
{ 
    // Serial.println("go ahead");
    // resetdongco();
    // analogWrite(tien1,255);
    // analogWrite(tien2,255);
    //delay(2);
    //unit = 1cm
    resetWheels(); 
    analogWrite(tien1, 255);
    analogWrite(tien2, 255);
    //delay time for moving 1cm t = s/v
    float curArgRad = currentAngle * (PI / 180);
    currentPosition.x -= deltaMove * cos(curArgRad);
    currentPosition.y -= deltaMove * sin(curArgRad);
    float time = (deltaMove/ velocity) * 1000;
    delay(time);
   
}

void disangtrai()
{
  Serial.println("turn left");
  resetWheels();
  digitalWrite(lui1,HIGH);
  delay(250);
  digitalWrite(lui1,LOW);
  
    
}
void disangphai()
{
  Serial.println("turn right");
   resetWheels();
  digitalWrite(lui2,HIGH);
  delay(250);
  digitalWrite(lui2,LOW);
   
}

void dilui()
{
  Serial.println("step backward");
  resetWheels();
  for(i=0;i<20;i++)
    
        digitalWrite(lui1,HIGH);
        digitalWrite(lui2,HIGH);
        delay(700);
     
    digitalWrite(lui1,LOW);
    digitalWrite(lui2,LOW);
}

void resetWheels()
{
    analogWrite(tien1,0);
    analogWrite(tien2,0);
    digitalWrite(lui1,LOW);
    digitalWrite(lui2,LOW);
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
    timeCollideObstacle = pulseIn(echo,HIGH);  
    // Tính khoảng cách đến vật.
    distanceToObstacle = int(thoigian/2/29.412);
    

}

/*********** chương trình quay cảm biến xang trái *********/
void quaycbsangtrai()
{

    myservo.write(180);              // tell servo to go to position in variable 'pos'
    delay(1000);
    dokhoangcach();
    myservo.write(90);              // tell servo to go to position in variable 'pos'  
}
void quaycbsangphai()
{
    myservo.write(0);              // tell servo to go to position in variable 'pos'
    delay(1000);
    dokhoangcach();
    myservo.write(90);              // tell servo to go to position in variable 'pos'
}

void rotateServo(float angle)
{
    myservo.write(angle);              // tell servo to go to position in variable 'pos'
    Serial.print("Rote servo  angle: ");
    Serial.println(angle); // tell servo to go to position in variable
    delay(1000);
    dokhoangcach();
    delay(500);
}

void resetservo()
{
    myservo.write(90);              // tell servo to go to position in variable 'pos'
    delay(1000);
}

void turn(float angle)
{
    resetWheels(); 
    float angleOnWheel = (angle/360)*4;
    float roundOnWheel = angle/360;
    float time = (roundOnWheel / angular_velocity) * 1000;
    if(angle < 90)
    {
        Serial.println("turn right");
        resetWheels();
        digitalWrite(tien1,HIGH);
        delay(time);
        digitalWrite(tien1,LOW);
    }
    else if (angle > 90)
    {
        Serial.println("turn right");
        resetWheels();
        digitalWrite(tien2,HIGH);
        delay(time);
        digitalWrite(tien2,LOW);
    }
}

void findWay()
{
    resetservo();
    int angle = 0;
    float distance = 1000;
    int arrayDistance[180 / deltaMove]; 
    int arrayAngle[180 / deltaMove]; 
    int 
    while (angle <=  180)
    {
        rotateServo(angle);
        arrayDistance[]
        if (khoangcach < distance && khoangcach < gioihan)
        {
            distance = khoangcach;
            currentAngle = angle;  
            break;
        }
        angle += deltaAngle;
    }

    if(distance >= 1000)
    {
        turnBackward();
    }
    else
    {
        turn(angle);
        dithang();
    }
}

void turnBackward()
{
    resetWheels(); 
    analogWrite(lui1, 255);
    analogWrite(lui2, 255);
    //delay time for moving 1cm t = s/v
    float time = (deltaMove / velocity) * 1000;
    float curArgRad = currentAngle * (PI / 180);
    currentPosition.x -= deltaMove * cos(curArgRad);
    currentPosition.y -= deltaMove * sin(curArgRad);
    delay(time);
}

void resetservo()
{
   myservo.write(90);
}
