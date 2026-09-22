// 电机控制引脚定义
const int LEFT_MOTOR_PIN1 = 10;
const int LEFT_MOTOR_PIN2 = 9;
const int RIGHT_MOTOR_PIN1 = 8;
const int RIGHT_MOTOR_PIN2 = 4;

// 红外传感器引脚定义
const int SENSOR_LEFT = A1;    // 左侧传感器
const int SENSOR_FRONT = A0;   // 前方传感器  
const int SENSOR_RIGHT = A2;   // 右侧传感器

// 传感器阈值
const int OBSTACLE_THRESHOLD = 500;

// 状态变量
bool lastLeftObstacle = false;
unsigned long lastActionTime = 0;
int stuckCounter = 0; // 卡死计数器

// 小车物理参数
const int CAR_LENGTH = 140; // 小车长度mm
const int TURN_SAFETY_MARGIN = 50; // 安全余量

void setup() {
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("红外避障小车启动 - 防卡死修正版");
  stopMotors();
  delay(2000);
  lastActionTime = millis();
}

void loop() {
  // 读取传感器数值
  int leftValue = analogRead(SENSOR_LEFT);
  int frontValue = analogRead(SENSOR_FRONT);
  int rightValue = analogRead(SENSOR_RIGHT);
  
  // 障碍物判断
  bool leftObstacle = leftValue < OBSTACLE_THRESHOLD;
  bool frontObstacle = frontValue < OBSTACLE_THRESHOLD;
  bool rightObstacle = rightValue < OBSTACLE_THRESHOLD;
  
  // 调试输出
  Serial.print("传感器:L");
  Serial.print(leftValue);
  Serial.print(" F");
  Serial.print(frontValue);
  Serial.print(" R");
  Serial.print(rightValue);
  Serial.print(" 障碍物:");
  if (leftObstacle) Serial.print("左");
  if (frontObstacle) Serial.print("前");
  if (rightObstacle) Serial.print("右");
  if (!leftObstacle && !frontObstacle && !rightObstacle) Serial.print("无");
  
  // 检测是否卡死（长时间没有前进动作）
  if (millis() - lastActionTime > 3000) { // 3秒没有有效前进
    stuckCounter++;
    Serial.print(" 卡死计数:");
    Serial.print(stuckCounter);
    
    if (stuckCounter >= 2) { // 连续卡死2次，执行脱困程序
      executeEscapeRoutine();
      stuckCounter = 0;
      lastActionTime = millis();
      return;
    }
  }
  
  // 改进的避障逻辑
  if (frontObstacle) {
    // 前方有障碍物 - 根据左右传感器情况智能转向
    Serial.println(" -> 前方有障碍物");
    stopMotors();
    delay(200);
    
    if (!rightObstacle) {
      // 右边有路，大角度右转（解决卡死问题）
      Serial.println("    动作: 大角度右转90度");
      turnRight();
      delay(400);  // 90度转弯
      stopMotors();
      delay(100);
    } else if (!leftObstacle) {
      // 左边有路，左转
      Serial.println("    动作: 左转90度");
      turnLeft();
      delay(400);  // 90度转弯
      stopMotors();
      delay(100);
    } else {
      // 左右都有障碍，后退并右转（解决尾部碰撞）
      Serial.println("    动作: 后退转弯避开尾部碰撞");
      moveBackward();
      delay(300);  // 后退时间根据小车速度调整
      turnRight();
      delay(300);
      stopMotors();
      delay(100);
    }
  } 
  else if (rightObstacle && !frontObstacle) {
    // 右边有障碍但前方畅通 - 轻微左转保持距离
    Serial.println(" -> 右边有障碍，轻微左转保持距离");
    slightLeft();
    lastActionTime = millis(); // 更新前进时间
  }
  else {
    // 前方和右边都无障碍 - 左墙跟随逻辑
    if (leftObstacle) {
      if (leftObstacle && !lastLeftObstacle) {
        Serial.println(" -> 新检测到左侧墙，安全转向程序");
        
        // 安全转向：先远离墙壁再转向（避免尾部碰撞）
        Serial.println("    动作: 先右移再转向");
        
        // 1. 轻微右移远离墙壁
        slightRight();
        delay(200);
        stopMotors();
        delay(100);
        
        // 2. 前进并轻微右转
        slightRight();
        delay(300);
        
      } else {
        // 持续检测到左墙，保持安全距离
        Serial.println(" -> 沿墙行驶，保持安全距离");
        slightRight(); // 轻微右转保持距离
        delay(100);
      }
      moveForward(); // 然后直行
    } else {
      // 左侧无墙，轻微左转找墙
      Serial.println(" -> 轻微左转找墙");
      slightLeft();
    }
    lastActionTime = millis(); // 更新前进时间
  }
  
  // 更新前一次传感器状态
  lastLeftObstacle = leftObstacle;
  
  delay(100);
}

// 脱困程序（解决卡死问题）
void executeEscapeRoutine() {
  Serial.println("执行脱困程序！");
  
  // 1. 先后退安全距离
  Serial.println("后退安全距离");
  moveBackward();
  delay(500);
  stopMotors();
  delay(200);
  
  // 2. 右转180度尝试新方向
  Serial.println("右转180度");
  turnRight();
  delay(800);
  stopMotors();
  delay(200);
  
  // 3. 前进一段距离
  Serial.println("前进脱困");
  moveForward();
  delay(1000);
  stopMotors();
  delay(200);
}

// 新增：轻微右转函数
void slightRight() {
  static unsigned long lastTime = 0;
  static bool rightMotorState = false;
  
  unsigned long currentTime = millis();
  
  if (currentTime - lastTime > 300) {
    rightMotorState = !rightMotorState;
    lastTime = currentTime;
  }
  
  // 左轮始终前进
  digitalWrite(LEFT_MOTOR_PIN1, HIGH);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  
  // 右轮间歇性前进（实现轻微右转）
  if (rightMotorState) {
    digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  } else {
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  }
}

// 原有电机控制函数保持不变
void moveForward() {
  digitalWrite(LEFT_MOTOR_PIN1, HIGH);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
}

void moveBackward() {
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, HIGH);
}

void turnLeft() {
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
}

void turnRight() {
  digitalWrite(LEFT_MOTOR_PIN1, HIGH);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, HIGH);
}

void slightLeft() {
  static unsigned long lastTime = 0;
  static bool leftMotorState = false;
  
  unsigned long currentTime = millis();
  
  if (currentTime - lastTime > 300) {
    leftMotorState = !leftMotorState;
    lastTime = currentTime;
  }
  
  digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  
  if (leftMotorState) {
    digitalWrite(LEFT_MOTOR_PIN1, HIGH);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
  } else {
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
  }
}

void stopMotors() {
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
}
