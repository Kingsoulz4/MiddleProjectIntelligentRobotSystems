
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

int limitThreshold = 25;
int i;
unsigned long timeCollideObstacle; 
int distanceToObstacle;           
float velocity = (float) 88/3;
float angular_velocity = (float)52/15;
float deltaAngle = 30;
float deltaMove = 5;
int rangeView = 1;
float currentAngle = 90;
float angleCollideObstacle = -1;
struct Position currentPosition;
struct Position destinationPosition; 

void resetWheels();

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
    destinationPosition.y = 1000;
    currentPosition.x = 0;
    currentPosition.y = 0;

   // Serial.begin(9600);
    myservo.write(90);    
    delay(500); 
}

void loop() 
{   
    findWay();
    // turn(45);
    // delay(500);
    // turn(135);
    // delay(500);
    // dithang();
    // delay(500);
    //turnBackward();
    //dithang();
    //dilui();
    //turnBackward();
    //avoidObstacle();
    //Serial.println(khoangcach);
}


void goAhead()
{  
    resetWheels(); 
    digitalWrite(tien1, HIGH);
    digitalWrite(tien2, HIGH);
    //delay time for moving 1cm t = s/v
    float curArgRad = currentAngle * (PI / 180);
    currentPosition.x += deltaMove * cos(curArgRad);
    currentPosition.y += deltaMove * sin(curArgRad);
    float time = (deltaMove/ velocity) * 1000;
    Serial.print("Go ahead ");
    Serial.print(time);
    Serial.println("ms");
    delay(time);
    digitalWrite(tien1, LOW);
    delay(time/8);
    digitalWrite(tien2, LOW);
    resetWheels();  
}

void resetWheels()
{
    digitalWrite(tien1,LOW);
    digitalWrite(tien2,LOW);
    digitalWrite(lui1,LOW);
    digitalWrite(lui2,LOW);
}
/******** chương trình đo khoảng cách SRF04 ***************/
void detectObstacle()
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
    distanceToObstacle = int(timeCollideObstacle/2/29.412);

    Serial.print(" Distances: ");
    Serial.println(distanceToObstacle); 
}

void rotateServo(float angle)
{
    myservo.write(angle);              // tell servo to go to position in variable 'pos'
    Serial.print("Rote servo  angle: ");
    Serial.print(angle); // tell servo to go to position in variable
    delay(1000);
    detectObstacle();
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
    float time = (roundOnWheel / angular_velocity) * 1000 * 4;
    if(angle < 90)
    {
        Serial.print("turn right ");
        Serial.println(angle);
        resetWheels();
        digitalWrite(tien1,HIGH);
        delay(time/1);
        
    }
    else if (angle > 90)
    {
        Serial.print("turn left ");
        Serial.println(angle);
        resetWheels();
        digitalWrite(tien2,HIGH);
        delay(time / 2.5);
        
    }
    
    resetWheels();
    delay(250);
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
        arrayDistance[j] = distanceToObstacle;
        arrayAngle[j] = angle;
        angle += deltaAngle;
        j++;
    }

    resetservo();
    int index = 0;
    for (i = 0; i < samples; i++)
    {
        struct Position tmpPos;
        float curArgRad = (currentAngle + arrayAngle[i] - 90) * (PI / 180);
        tmpPos.x = currentPosition.x +  deltaMove* cos(curArgRad);
        tmpPos.y = currentPosition.y +  deltaMove* sin(curArgRad);
        float minDist = calculateDistance(tmpPos, destinationPosition);
        if(i - rangeView >= 0 && i + rangeView < samples && minDist < distance)
        {
            int flag = 1;
            for (int k = i - rangeView; k <= i + rangeView; k++)
            {
                float arg = currentAngle + arrayAngle[k] - 90;
                if (arrayDistance[k] < limitThreshold || arrayDistance[k] > 1000 || arg == angleCollideObstacle)
                {
                    flag = 0;
                    break;
                }
            }
            if(flag == 1)
            {
                Serial.print("arrayDistance ");
                Serial.print(i);
                Serial.print("   ");
                Serial.println(arrayDistance[i]);
                distance = minDist;
                index = i;
            }
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
        angleCollideObstacle = currentAngle;
        turnBackward();
    }
    else
    {
        angle = arrayAngle[index];
        currentAngle += angle - 90;
        angleCollideObstacle = -1;
        turn(angle);
        goAhead();
    }
    delay(1000);
}

void turnBackward()
{
    Serial.println("Turn backward");
    resetWheels(); 
    digitalWrite(lui1, HIGH);
    digitalWrite(lui2, HIGH);
    //delay time for moving 1cm t = s/v
    float time = (deltaMove / velocity) * 1000;
    float curArgRad = currentAngle * (PI / 180);
    currentPosition.x -= deltaMove * cos(curArgRad);
    currentPosition.y -= deltaMove * sin(curArgRad);
    delay(time);
    resetWheels(); 
}

float calculateDistance(struct Position p1, struct Position p2) 
{
    return sqrt(pow((p2.x - p1.x),2) + pow((p2.y - p1.y),2));
}
