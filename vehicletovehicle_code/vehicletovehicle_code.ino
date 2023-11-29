#define BLYNK_TEMPLATE_ID "TMPL3WrA7INXz"
#define BLYNK_TEMPLATE_NAME "vehicle2vehicle"
#define BLYNK_AUTH_TOKEN "EZS61l57cT48aXHVG25j6xx_PVjG0sBK"
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include<HTTPClient.h>
BlynkTimer timer;

//#include<SoftwareSerial.h>
#include<TinyGPS++.h>
static const uint32_t GPSBaud = 9600;    //if Baud rate 9600 didn't work in your case then use 4800
TinyGPSPlus gps;
unsigned int move_index = 1;       // fixed location for now 
#include "I2Cdev.h"     //For communicate with MPU6050
#include "MPU6050.h"    //The main library of the MPU6050
MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
float spd;       //Variable  to store the speed
float sats;      //Variable to store no. of satellites response
String bearing;  //Variable to store orientation or direction of GPS
float latitude;
float longitude;
int trig =18;
int echo =19;
float duration;
long distance;

int accidentsensor = 15;
int accidentval;
int in1 = 25;
int in2 = 26;
int in3 = 27;
int in4 = 14;
int buzzer = 5;
int led =23;
int alcohol =32;
int alcoholval;
int flag;
int flag1;
int flag2;
int flag3;
int flag4;
int flag5;
int flag6;
BLYNK_WRITE(V2)
  {
    int forward = param.asInt();
    if(forward == HIGH)
    {
     fw(); 
    }
    else
    {
      
      stp();
    }
  }
  BLYNK_WRITE(V3)
  {
    int backward = param.asInt();
    if(backward == HIGH)
    {
      bw();
    }
    else
    {
      stp();
      }
    
  }
  BLYNK_WRITE(V4)
  {
    int left = param.asInt();
    if(left == HIGH){
      lft();
    }
    else{
      stp();
    }
  }
  BLYNK_WRITE(V5)
  {
    int right = param.asInt();
    if(right == HIGH){
      
      rgt();
    }
    else{
      stp();
    }
  }

// Maker Webhooks IFTTT
const char* serverName = "http://maker.ifttt.com/trigger/vehicle/with/key/bpMh-zPFE_0gG2i4ySwaxC";

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "vehicle";
char pass[] = "12345678"; 



void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
  Wire.begin();
  mpu.initialize();
  pinMode(echo, INPUT);
  pinMode(trig,OUTPUT);
  pinMode(accidentsensor, INPUT_PULLUP);
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(led,OUTPUT);
  pinMode(alcohol,INPUT);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000, obstacle);
  timer.setInterval(2000,mems);
}
  

  void mems()
  {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  int xaxis = map(ax, -17000, 17000, 300, 400 ); //Send X axis data
  int yaxis = map(ay, -17000, 17000, 100, 200);  //Send Y axis data
 
   Serial.println("x axis value is: "+ String(xaxis));
  Serial.println("y axis value is: "+ String(yaxis));
  if(xaxis > 380)
  {
    if(flag==0)
    {
      up("HIT FROM BACK");
      flag=1;
      }
    Serial.println("bw");
    Blynk.virtualWrite(V0,"HIT FROM BACK");
     }
    
    else if(xaxis < 330)
    {
      if(flag1==0)
    {
      up("CRASHED ON BARICADE");
      
      flag1=1;
    }
    Blynk.virtualWrite(V0,"CRASHED ON BARICADE");
    Serial.println("fw");
     }
    else if(yaxis > 170)
     {
      if(flag2==0)
    {
    up("VEHCILE FALLEN ON RIGHT");
        flag2=1;
        
    }
    Blynk.virtualWrite(V0,"VEHCILE FALLEN ON RIGHT");
      Serial.println("rgt");
      }
      else if(yaxis < 110)
      {
        if(flag3==0)
    {
      up("VEHCILE FALLEN ON LEFT");
      flag3=1;
    }
    Blynk.virtualWrite(V0,"VEHCILE FALLEN ON LEFT");
        Serial.println("lft");
      }
    if(xaxis > 330 && xaxis < 380 && yaxis > 110 && yaxis < 170){
      
      Serial.println("stp");
      Blynk.virtualWrite(V0,"NORMAL");
    
    }
  }
void obstacle()
  {
    accidentval=digitalRead(accidentsensor);
    alcoholval=digitalRead(alcohol);
    Serial.println("accident:"+String(accidentval));
    digitalWrite(trig,LOW);
    delayMicroseconds(2);
    digitalWrite(trig,HIGH);
    delayMicroseconds(10);
    digitalWrite(trig,LOW);
    duration=pulseIn(echo,HIGH);
    distance =duration*0.034/2;
    Serial.println(distance);
    Blynk.virtualWrite(V1,distance);
    if(distance<20)
    {
      stp();
      digitalWrite(buzzer,HIGH);
      digitalWrite(led,HIGH);
    }
    else if(accidentval ==0)
    {
      if(flag5==0)
      {
        up("Met with an accident\nhttp://maps.google.com/maps?q=loc:"+String(latitude,6)+","+String(longitude,6));
        flag5=1;
      }
       
      stp();
      digitalWrite(buzzer,HIGH);
      digitalWrite(led,HIGH);
      

    }
    else if(alcoholval ==0)
    {
      if(flag6 ==0)
      {
        up("Alcohol Detected\nhttp://maps.google.com/maps?q=loc:"+String(latitude,6)+","+String(longitude,6));
        flag6=1;
      }
      digitalWrite(buzzer,HIGH);
    }
    else
    {
      digitalWrite(buzzer,LOW);
      digitalWrite(led,LOW);
      
    }
  
  }
  


  void GPS(){
  if(gps.charsProcessed() < 10)
  {
    Serial2.println("No GPS detected: check wiring.");
    Blynk.virtualWrite(V6, "GPS ERROR");
  }
  }

  void displaygpsInfo(){
    if(gps.location.isValid())
    {
       latitude = (gps.location.lat());
       longitude = (gps.location.lng());

      Serial2.print("LAT:  ");
      Serial2.println(latitude, 6);
      Serial2.print("LONG:  ");
      Serial2.println(longitude, 6);
      
    sats = gps.satellites.value();    //get number of satellites
    bearing = TinyGPSPlus::cardinal(gps.course.value()); // get the direction
    
  }
  Serial2.println();

  }

  
void loop()
{
  while (Serial2.available() > 0)
  {
    if(gps.encode(Serial2.read()))
    displaygpsInfo();
  }
  Blynk.run();
  timer.run();
}

void fw()
{
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      Serial.println("Forward");
}
void bw(){
       digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      Serial.println("Backward");
      
  
}

void lft()
{
 digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      Serial.println("Left"); 
}
void rgt()
{
digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      Serial.println("Right");  
}
void stp()
{
 digitalWrite(in1, HIGH);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, HIGH);
      Serial.println("Stop"); 
}


void up(String dat)
{
  makeIFTTTRequest(dat);
}
// Make an HTTP request to the IFTTT web service
void makeIFTTTRequest(String dString) 
{
  Serial.print("Sending SMS");
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "value1=" + String(dString);           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
}