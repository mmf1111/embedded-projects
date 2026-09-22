// 电机引脚定义
const int leftMotorPin1 = 10;   // 左轮引脚1
const int leftMotorPin2 = 9;    // 左轮引脚2
const int rightMotorPin1 = 8;   // 右轮引脚1
const int rightMotorPin2 = 4;   // 右轮引脚2

// 传感器引脚定义（请根据实际连接修改）
const int leftSensorPin = A0;   // 左传感器引脚
const int rightSensorPin = A1;  // 右传感器引脚

// 变量定义
int leftSensorValue = 0;        // 左传感器值
int rightSensorValue = 0;       // 右传感器值

// 计时变量
unsigned long lostLineStartTime = 0;  // 开始丢失线路的时间
bool isTrackingLost = false;          // 是否丢失跟踪标志
const unsigned long LOST_TIME_THRESHOLD = 100;  // 丢失阈值0.1秒（100毫秒）

void setup() {
  // 设置电机引脚为输出
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  
  // 设置传感器引脚为输入
  pinMode(leftSensorPin, INPUT);
  pinMode(rightSensorPin, INPUT);
  
  // 初始化串口通信（用于调试）
  Serial.begin(9600);
}

void loop() {
  // 读取传感器值
  leftSensorValue = digitalRead(leftSensorPin);
  rightSensorValue = digitalRead(rightSensorPin);
  
  // 调试输出
  Serial.print("Left: ");
  Serial.print(leftSensorValue);
  Serial.print(" Right: ");
  Serial.print(rightSensorValue);
  
  // 检查是否两个传感器都检测不到黑线（都在白线上）
  if (leftSensorValue == HIGH && rightSensorValue == HIGH) {
    // 如果是第一次检测到丢失线路，记录开始时间
    if (!isTrackingLost) {
      lostLineStartTime = millis();
      isTrackingLost = true;
      Serial.print(" - Lost track, starting timer: ");
      Serial.println(lostLineStartTime);
    } else {
      // 已经处于丢失状态，检查是否超过阈值
      unsigned long currentTime = millis();
      unsigned long lostDuration = currentTime - lostLineStartTime;
      
      Serial.print(" - Lost duration: ");
      Serial.print(lostDuration);
      Serial.println(" ms");
      
      // 如果丢失时间超过0.1秒，立即停止
      if (lostDuration >= LOST_TIME_THRESHOLD) {
        stopCar();
        Serial.println("EMERGENCY STOP - Track lost for too long!");
        
        // 保持停止状态，直到重新检测到线路
        while (digitalRead(leftSensorPin) == HIGH && digitalRead(rightSensorPin) == HIGH) {
          delay(10); // 短暂延迟，避免过于频繁检测
        }
        
        // 重置丢失标志
        isTrackingLost = false;
        Serial.println("Track found, resuming operation");
        return; // 退出本次循环，重新开始检测
      } else {
        // 还在阈值内，继续直行寻找线路
        moveForward();
        Serial.println(" - Searching for track...");
      }
    }
  } else {
    // 至少有一个传感器检测到黑线，重置丢失标志
    if (isTrackingLost) {
      isTrackingLost = false;
      Serial.println(" - Track found, resetting timer");
    }
    
    // 正常循迹逻辑
    // 情况1：两个传感器都检测到黑线（都低电平）- 直行
    if (leftSensorValue == LOW && rightSensorValue == LOW) {
      moveForward();
      Serial.println(" - Moving Forward (both on black)");
    }
    // 情况2：左传感器检测到黑线，右传感器在白线上 - 左转
    else if (leftSensorValue == LOW && rightSensorValue == HIGH) {
      turnLeft();
      Serial.println(" - Turning Left");
    }
    // 情况3：右传感器检测到黑线，左传感器在白线上 - 右转
    else if (leftSensorValue == HIGH && rightSensorValue == LOW) {
      turnRight();
      Serial.println(" - Turning Right");
    }
  }
  
  delay(50); // 短暂延迟，避免过于敏感
}

// 直行函数
void moveForward() {
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
}

// 左转函数
void turnLeft() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
}

// 右转函数
void turnRight() {
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
}

// 停止函数
void stopCar() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
}

// 后退函数（备用）
void moveBackward() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
}
