
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

int gioihan = 25;
int i;
int x=0;
unsigned long thoigian; // biến đo thời gian
int khoangcach;           // biến lưu khoảng cách
int khoangcachtrai,khoangcachphai;
float velocity = (float) 220/3;
float angular_velocity = (float)52/15;
float deltaAngle = 45;
float deltaMove = 5;
float currentAngle = 90;
struct Position currentPosition;
struct Position destinationPosition; 

void dokhoangcach();
void dithang(int duongdi);
void disangtrai();
void disangphai();
void dilui();
void resetdongco();
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

    destinationPosition.x = 0;
    destinationPosition.y = 50;
    currentPosition.x = 0;
    currentPosition.y = 0;

   // Serial.begin(9600);
    myservo.write(90);    
    delay(500); 
}

void loop() 
{   
    findWay();
    //dilui();
    //dithang();
    //avoidObstacle();
    //Serial.println(khoangcach);
}

void avoidObstacle()
{
    khoangcach=0;
    dokhoangcach();

    Serial.print("Khoảng cách: ");
    Serial.print(khoangcach);
    Serial.println("cm");
    
    if(khoangcach>gioihan||khoangcach==0) 
    {
      dokhoangcach();
       if(khoangcach>gioihan||khoangcach==0) 
        {
          dithang();
          delay(500); 
        }   
    }
    else
    {
      
      resetdongco();
      quaycbsangtrai();
      
      khoangcachtrai=khoangcach;
     
      quaycbsangphai();
      khoangcachphai=khoangcach;
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

void dithang()
{  
    resetdongco(); 
    analogWrite(tien1, 255);
    analogWrite(tien2, 255);
    //delay time for moving 1cm t = s/v
    float curArgRad = currentAngle * (PI / 180);
    currentPosition.x += deltaMove * cos(curArgRad);
    currentPosition.y += deltaMove * sin(curArgRad);
    float time = (deltaMove/ velocity) * 1000;
    Serial.print("Go ahead ");
    Serial.print(time);
    Serial.println("ms");
    delay(time); 
    resetdongco(); 
}

void disangtrai()
{
    Serial.println("turn left");
    resetdongco();
    digitalWrite(lui1,HIGH);
    delay(250);
    digitalWrite(lui1,LOW);
}
void disangphai()
{
    Serial.println("turn right");
    resetdongco();
    digitalWrite(lui2,HIGH);
    delay(250);
    digitalWrite(lui2,LOW);
}

void dilui()
{
    Serial.println("step backward");
    resetdongco();
    for(i=0;i<20;i++)
        digitalWrite(lui1,HIGH);
        digitalWrite(lui2,HIGH);
        delay(700);
        
        digitalWrite(lui1,LOW);
        digitalWrite(lui2,LOW);
}

void resetdongco()
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
    thoigian = pulseIn(echo,HIGH);  
    // Tính khoảng cách đến vật.
    khoangcach = int(thoigian/2/29.412);

    Serial.print(" Distances: ");
    Serial.println(khoangcach); 
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
    //myservo.write(angle);              // tell servo to go to position in variable 'pos'
    Serial.print("Rote servo  angle: ");
    Serial.print(angle); // tell servo to go to position in variable
    delay(1000);
    dokhoangcach();
}

void resetservo()
{
    myservo.write(90);              // tell servo to go to position in variable 'pos'
    delay(1000);
}

void turn(float angle)
{
    resetdongco(); 
    float angleOnWheel = (angle/360)*4;
    float roundOnWheel = angle/360;
    float time = (roundOnWheel / angular_velocity) * 1000;
    if(angle < 90)
    {
        Serial.print("turn right ");
        Serial.println(angle);
        resetdongco();
        analogWrite(tien1,255);
        delay(time);
        
    }
    else if (angle > 90)
    {
        Serial.print("turn left ");
        Serial.println(angle);
        resetdongco();
        analogWrite(tien2,255);
        delay(time);
        
    }
    resetdongco();
}

void findWay()
{
    if(abs(currentPosition.x - destinationPosition.x) < 1 && abs(currentPosition.y - destinationPosition.y) < 1)
    {
        Serial.println("Here");
        return;
    }

    //resetservo();
    int angle = 0;
    float distance = 1000;
    int samples = 180 / deltaAngle + 1; 
    int arrayDistance[samples]; 
    int arrayAngle[samples]; 

    for (i = 0; i < samples; i++)
    {
        arrayDistance[i] = 1000;
        arrayAngle[i] = 0;        
    }    

    int j = 0;
    while (angle <=  180)
    {
        rotateServo(angle);
        arrayDistance[j] = khoangcach;
        arrayAngle[j] = angle;
        angle += deltaAngle;
        j++;
    }

    resetservo();
    int index = 0;
    for (i = 0; i < samples; i++)
    {
        struct Position tmpPos;
        float curArgRad = arrayAngle[i] * (PI / 180);
        tmpPos.x = currentPosition.x +  deltaMove* cos(curArgRad);
        tmpPos.y = currentPosition.y +  deltaMove* sin(curArgRad);
        float minDist = calculateDistance(tmpPos, destinationPosition);
        if (minDist < distance && arrayDistance[i] > gioihan && arrayDistance[i] < 1000)
        {
            Serial.print("arrayDistance ");
            Serial.print(i);
            Serial.print("   ");
            Serial.println(arrayDistance[i]);
            distance = minDist;
            index = i;
        }
    }
    
    Serial.print("Distance:");
    Serial.println(distance);
    Serial.print("Current pos x:");
    Serial.println(currentPosition.x);
    Serial.print("Current pos y:");
    Serial.println(currentPosition.y);
    if(distance >= 1000 || (arrayAngle[index] > 180 || arrayAngle[index] < 0))
    {
        turnBackward();
    }
    else
    {
        angle = arrayAngle[index];
        currentAngle = angle;
        turn(angle);
        dithang();
    }
    delay(1000);
}

void turnBackward()
{
    Serial.println("Turn backward");
    resetdongco(); 
    analogWrite(lui1, 255);
    analogWrite(lui2, 255);
    //delay time for moving 1cm t = s/v
    float time = (deltaMove / velocity) * 1000;
    float curArgRad = currentAngle * (PI / 180);
    currentPosition.x -= deltaMove * cos(curArgRad);
    currentPosition.y -= deltaMove * sin(curArgRad);
    delay(time);
    resetdongco(); 
}

float calculateDistance(struct Position p1, struct Position p2) 
{
    return sqrt(pow((p2.x - p1.x),2) + pow((p2.y - p1.y),2));
}
