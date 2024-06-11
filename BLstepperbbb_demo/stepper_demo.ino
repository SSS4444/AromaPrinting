/*#include <SoftwareSerial.h>
#include <Servo.h>
#include <Stepper28BYJ48.h>
#include <Arduino.h>
const int stepsPerRevolution = 683;  //28BYJ48电机旋转一周需要的步数


// 电机接在引脚8 ~ 11: 电机线依次为蓝，粉，黄，橙
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

int stepCount = 0;         // number of steps the motor has taken

void setup() {
  // initialize the serial port:
  Serial.begin(9600);
  
  //设置电机转速r/min
  myStepper.setSpeed(20);
}

void loop() {
  // 正转60°
  myStepper.step(stepsPerRevolution);
  Serial.print("steps:" );
  Serial.println(stepsPerRevolution);
  delay(1000);
  // 反转60°
  myStepper.step(-stepsPerRevolution);
  Serial.print("steps:" );
  Serial.println(stepsPerRevolution);
  delay(1000);
}
*/


/*
//蓝牙+电机
#include <SoftwareSerial.h>
#include <Servo.h>
#include <Stepper28BYJ48.h>
#include <Arduino.h>
const int stepsPerRevolution = 2048;  // 28BYJ48电机旋转一周需要的步数

// 电机接在引脚8 ~ 11: 电机线依次为蓝，粉，黄，橙
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

SoftwareSerial BTserial(2, 3); // 蓝牙模块的RX、TX分别连接到Arduino的数字3,2号引脚

void setup() {
  Serial.begin(9600);
  BTserial.begin(9600); // 初始化蓝牙模块的软件串口
  myStepper.setSpeed(20); // 设置电机转速
}

void loop() {
  // 检查蓝牙模块是否有数据可读
  if (BTserial.available() > 0) {
    char receivedCommand = BTserial.read(); // 读取一个字符

    if (receivedCommand == '1') { // 如果接收到的字符是'1'
      Serial.println("Received command to rotate 60 degrees.");
      myStepper.step(stepsPerRevolution / 6); // 转动电机60度，假设每步是360度/683
      delay(1000);
      Serial.print("steps: ");
      Serial.println(stepsPerRevolution / 6);
      int a = stepsPerRevolution / 6;
      myStepper.step(-a); // 转动电机60度，假设每步是360度/683
      delay(1000);
      Serial.print("steps: ");
      Serial.println("归位");
    }
    else if (receivedCommand == '2') { // 如果接收到的字符是'2'
      Serial.println("Received command to rotate 120 degrees.");
      myStepper.step(stepsPerRevolution / 3); // 转动电机120度，假设每步是360度/683
      delay(1000);
      Serial.print("steps: ");
      Serial.println(stepsPerRevolution / 3);
      int a = stepsPerRevolution / 3;
      myStepper.step(-a); // 转动电机60度，假设每步是360度/683
      delay(1000);
      Serial.print("steps: ");
      Serial.println("归位");
    }
    else if (receivedCommand == '3') { // 如果接收到的字符是'3'
      Serial.println("Received command to rotate 180 degrees.");
      myStepper.step(stepsPerRevolution / 2); // 转动电机120度，假设每步是360度/683
      delay(1000);
      Serial.print("steps: ");
      Serial.println(stepsPerRevolution / 2);
      int a = stepsPerRevolution / 2;
      myStepper.step(-a); // 转动电机60度，假设每步是360度/683
      delay(1000);
      Serial.print("steps: ");
      Serial.println("归位");
    }
  }
}

*/

//带上蠕动泵
#include <Servo.h>
#include <Stepper28BYJ48.h>
#include <SoftwareSerial.h>
#include <Arduino.h>

const int stepsPerRevolution = 2048;  // 28BYJ48电机旋转一周需要的步数

// 电机接在引脚8 ~ 11
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

SoftwareSerial BTserial(2, 3); // 蓝牙模块的TX、RX分别连接到Arduino的数字2、3号引脚

const int relayPins[] = {4, 5, 6}; // 假设有三个继电器，分别连接到引脚4, 5, 6，后期可以直接加引脚
const int numPumps = sizeof(relayPins) / sizeof(relayPins[0]); // 蠕动泵的数量
int relayDuration=0;//泵工作时间数
// 定义变量来存储电机转动的角度和当前选择的蠕动泵
int angleToRotate = 0;
int selectedPump = 0;

void setup() {
  Serial.begin(9600);
  BTserial.begin(9600);
  myStepper.setSpeed(20);
  
  // 初始化继电器引脚
  for (int i = 0; i < numPumps; i++) {
    pinMode(relayPins[i], OUTPUT);
  }
}

void loop() {
  if (BTserial.available() > 0) {
    char receivedCommand = BTserial.read();
    
    // 第一个信号：根据接收到的字符决定电机转动的角度和选择的蠕动泵
    if (receivedCommand >= '1' && receivedCommand <= '3') {
      angleToRotate = stepsPerRevolution / (4 / (receivedCommand - '0')); // 1 -> 90°, 2 -> 180°, 3 -> 270°
      selectedPump = receivedCommand - '1' + 1; // 将字符转换为1到3的数字
    }
    // 第二个信号：控制继电器的工作时间
    else if (receivedCommand == 'A' || receivedCommand == 'B') {
      relayDuration = receivedCommand == 'A' ? 1000 : 2000; // 'A'对应1秒，'B'对应2秒
      
    }
  }
  
  // 如果确定了转动角度，执行电机转动
  if (angleToRotate != 0) {
    Serial.print("Rotating to ");
    Serial.print(angleToRotate);
    Serial.println(" steps.");
    myStepper.step(angleToRotate);
    delay(1000);
    
  }
  delay(5000);//等一下泵泵
  controlPump(selectedPump, relayDuration);//泵工作ing
  
  //电机复位
  if (angleToRotate != 0) {
  Serial.print("Rotating to ");
  Serial.print("归位");
  Serial.println(" steps.");
  myStepper.step(-angleToRotate);
  delay(1000);
  angleToRotate = 0; // 重置角度变量，等待下一次命令 
  }
}

// 控制指定蠕动泵的函数
void controlPump(int pumpNumber, int duration) {
  if (pumpNumber > 0 && pumpNumber <= numPumps) {
    Serial.print("Pump ");
    Serial.print(pumpNumber);
    Serial.print(" dispensing for ");
    Serial.print(duration / 1000);
    Serial.println(" seconds.");
    
    digitalWrite(relayPins[pumpNumber - 1], HIGH); // 打开继电器
    delay(duration);
    digitalWrite(relayPins[pumpNumber - 1], LOW); // 关闭继电器
  }
}

