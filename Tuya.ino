
#include "Arduino.h"
#include "wifi.h"                 //引用TuyaWiFi
#include "mcu_api.h"              //引用Tuya
#include <U8x8lib.h>              //引用U8x8库
#include <SPI.h>                  //引用SPI
#include <ClosedCube_SHT31D.h>    //引用SHT31库
#define SHT30_addr 0x44           // SHT30地址

//屏幕显示定义
U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//SHT31库定义
ClosedCube_SHT31D sht30;
//管脚定义
#define BUTTON  12    // 按键的管脚定义
#define LED  7        // LED灯管脚定义
#define BAT_PIN  A6   // 电池电压检测管脚定义
#define BV  6         // 电池检测控制管脚定义
//Tuya模组通讯管脚定义
SoftwareSerial mySerial(0, 1); // RX, TX

//温湿度电池参数注册
float Sht30_Temp,Sht30_Hum,Tuya_Temp,BatV;
float Bat = 30;
float Voltage = 3.7;
//WiFi状态注册
int wifi_connectstate = 1;
//Millis定时参数注册
char Bat_time = 0;
char SHT30_time = 0 ;
int B_time = 299;
int SHT_time = 99;
uint32_t display_time = 0;

//SHT30读取温湿度程序注册
void SHT30ReadData(SHT31D result); 

//程序初始化
void setup() {
  wifi_protocol_init();          //WiFi协议初始化
  u8x8.begin();                  //U8x8初始化
  u8x8.setFlipMode(1);           //屏幕方向设置反向显示
  sht30.begin(SHT30_addr);       //SHT30初始化
  pinMode(BUTTON, INPUT_PULLUP); // 重置Wi-Fi按键初始化
  pinMode(LED, OUTPUT);          // Wi-Fi状态指示灯初始化
  pinMode(LED, HIGH);         
  pinMode(BV, OUTPUT);           // 电池测量初始化
  mySerial.begin(9600);          // 软件串口初始化
  mySerial.println("myserial init successful!");
}

//主体函数
void loop() {
  wifi_uart_service();   
  myserialEvent();      //串口接收处理 
  key_scan();           //按键配网   
  wifi_stat_led();      //状态指示灯
  getVoltage();         //获取电池电压 
  Get_TempHum();        //获取温湿度
  oled_display();       //屏幕显示
}

// 串口接收处理
void myserialEvent() {
  if (mySerial.available()) {
    unsigned char value = (unsigned char)mySerial.read();
    uart_receive_input(value);
  }
}

//配网按键，按键按下后开始配网
void key_scan(void){
  unsigned char buttonState  = HIGH;
  buttonState = digitalRead(BUTTON);
  if (buttonState == LOW) 
  {
    delay(10);
    buttonState = digitalRead(BUTTON);
    if (buttonState == LOW)
    {
       mcu_set_wifi_mode(SMART_CONFIG);   //智能配网
    }
  }
}

//获取温湿度
void Get_TempHum(){
  if ((millis() - SHT30_time) >= 200){   //0.1秒增加一次计数，由于使用8Mhz晶振所以*2
    SHT30_time = millis();
    SHT_time++;
    if (SHT_time >= 100){   // 10秒打开三极管测量一次温湿度
      SHT_time = 0;
      SHT30ReadData(sht30.readTempAndHumidity(SHT3XD_REPEATABILITY_HIGH, SHT3XD_MODE_POLLING, 25));         //获取温湿度
    }
  }
}
void SHT30ReadData(SHT31D result){
  Sht30_Temp = result.t;
  Tuya_Temp = Sht30_Temp*10;
  Sht30_Hum = result.rh;
}

// 获取电池电压
void getVoltage(){ 
  
  if ((millis() - Bat_time) >= 100){   //0.1秒增加一次计数，由于使用8Mhz晶振所以*2
    Bat_time = millis();
    B_time++;
    if (B_time >= 300){   // 30秒打开三极管测量一次电压
      B_time = 0;
      digitalWrite(BV, HIGH);
      delay(50); 
      BatV = analogRead(BAT_PIN);// read supply voltage via voltage divider
      Voltage = (BatV/153);// 511.5 = 1023 * R13 / (R12 + R13)
      Bat = map(Voltage*100,340,420,0,100);
      delay(2000);
      digitalWrite(BV, LOW);
    }
  }
  
}

//WiFi状态指示灯
void wifi_stat_led(void)
{
  switch(mcu_get_wifi_work_state())  //获取wifi工作状态
  {
    //开始Smart配网，灯十长一短闪烁
    case SMART_CONFIG_STATE:  
      wifi_connectstate = 1;
      digitalWrite(LED,LOW);       
      delay(1000);
      digitalWrite(LED,HIGH);       
      delay(100);
      break;
    //WiFi未连接灯熄灭
    case WIFI_NOT_CONNECTED:  
      wifi_connectstate = 1;
      digitalWrite(LED,HIGH);       
      break;  
    //WiFi已连接，灯闪烁
    case WIFI_CONNECTED:  
      wifi_connectstate = 1;
      digitalWrite(LED,LOW);
      delay(200);
      digitalWrite(LED,HIGH);       
      delay(200);
      break;
    //配网成功，连接上云。灯灭，屏幕显示WiFi图标
    case WIFI_CONN_CLOUD:  
      wifi_connectstate = 2;
      digitalWrite(LED,LOW);
      break;
    //其他情况，灯灭
    default:
      digitalWrite(LED,HIGH);
      wifi_connectstate = 1;
      break;
  }
}
