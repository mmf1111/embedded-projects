// ===== 引脚定义 =====
// 电机控制引脚
const int leftMotorPin1 = 10;
const int leftMotorPin2 = 9;
const int rightMotorPin1 = 8;
const int rightMotorPin2 = 4;

// 蜂鸣器引脚
const int buzzerPin = 2;

// 循迹传感器引脚
const int leftSensorPin = A0;
const int rightSensorPin = A1;

// 避障传感器引脚
const int SENSOR_LEFT = A2;
const int SENSOR_FRONT = A3;
const int SENSOR_RIGHT = A4;

// ===== 模式定义 =====
#define MODE_MUSIC 0
#define MODE_TRACKING 1
#define MODE_AVOIDANCE 2

int currentMode = MODE_MUSIC; // 默认音乐模式

// ===== 音乐模式变量 =====
#define NOTE_5 784
#define NOTE_6 880
#define NOTE_7 988
#define NOTE_1 523
#define NOTE_2 587
#define NOTE_3 659
#define NOTE_4 698
#define NOTE_0 0

int melody[] = {
  NOTE_0, NOTE_0, NOTE_5, NOTE_3, NOTE_2, NOTE_1, NOTE_1, NOTE_2, NOTE_3,
  NOTE_0, NOTE_0, NOTE_2, NOTE_3, NOTE_5, NOTE_0, NOTE_0,
  NOTE_0, NOTE_3, NOTE_1, NOTE_7, NOTE_6, NOTE_7, NOTE_7, NOTE_1, NOTE_2,
  NOTE_0, NOTE_0, NOTE_0, NOTE_0,
  NOTE_0, NOTE_5, NOTE_3, NOTE_2, NOTE_2, NOTE_2, NOTE_3, NOTE_5,
  NOTE_0, NOTE_0, NOTE_2, NOTE_2, NOTE_3, NOTE_3, NOTE_0, NOTE_0,
  NOTE_0, NOTE_3, NOTE_1, NOTE_7, NOTE_1, NOTE_0,
  NOTE_7, NOTE_7, NOTE_1, NOTE_2, NOTE_0,
  NOTE_1, NOTE_5, NOTE_3, NOTE_0, NOTE_5, NOTE_1,
  NOTE_0, NOTE_0, NOTE_0, NOTE_0,
  NOTE_6, NOTE_5, NOTE_5, NOTE_3, NOTE_3, NOTE_5, NOTE_5, NOTE_5,
  NOTE_0, NOTE_0, NOTE_0, NOTE_0,
  NOTE_1, NOTE_6, NOTE_5, NOTE_5, NOTE_3, NOTE_3, NOTE_5, NOTE_5,
  NOTE_5, NOTE_0, NOTE_0, NOTE_3, NOTE_2, NOTE_1,
  NOTE_0, NOTE_1, NOTE_2, NOTE_3, NOTE_5, NOTE_0,
  NOTE_7, NOTE_7, NOTE_1, NOTE_2, NOTE_0,
  NOTE_1, NOTE_6, NOTE_5, NOTE_5, NOTE_3, NOTE_5, NOTE_5, NOTE_5,
  NOTE_0, NOTE_0, NOTE_0, NOTE_3, NOTE_2,
  NOTE_1, NOTE_6, NOTE_5, NOTE_5, NOTE_3, NOTE_2,
  NOTE_0, NOTE_0, NOTE_0, NOTE_0,
  NOTE_1, NOTE_6, NOTE_5, NOTE_5, NOTE_3, NOTE_3, NOTE_5, NOTE_5,
  NOTE_5, NOTE_0, NOTE_0, NOTE_3, NOTE_2,
  NOTE_1, NOTE_6, NOTE_5, NOTE_5, NOTE_3, NOTE_1,
  NOTE_0, NOTE_0, NOTE_0, NOTE_0,
  NOTE_1, NOTE_6, NOTE_5, NOTE_5, NOTE_3, NOTE_3, NOTE_5, NOTE_5,
  NOTE_5, NOTE_0, NOTE_0, NOTE_3, NOTE_2, NOTE_1,
  NOTE_0, NOTE_6, NOTE_6, NOTE_3, NOTE_2, NOTE_3, NOTE_2
};

int noteDurations[] = {
  2, 4, 8, 8, 8, 8, 8, 8, 8,
  2, 4, 8, 8, 4, 2, 4,
  2, 8, 8, 8, 8, 8, 8, 8, 8,
  2, 2, 2, 2,
  2, 8, 8, 8, 8, 8, 8, 8,
  2, 4, 8, 8, 8, 8, 2, 4,
  2, 8, 8, 8, 4, 4,
  8, 8, 8, 4, 4,
  8, 8, 4, 4, 8, 4,
  2, 2, 2, 2,
  8, 8, 8, 8, 8, 8, 8, 8,
  2, 2, 2, 2,
  8, 8, 8, 8, 8, 8, 8, 8,
  4, 2, 4, 8, 8, 4,
  2, 8, 8, 8, 8, 4,
  8, 8, 8, 4, 4,
  8, 8, 8, 8, 8, 8, 8, 8,
  2, 2, 4, 8, 4,
  8, 8, 8, 8, 8, 4,
  2, 2, 2, 2,
  8, 8, 8, 8, 8, 8, 8, 8,
  4, 2, 4, 8, 4,
  8, 8, 8, 8, 8, 4,
  2, 2, 2, 2,
  8, 8, 8, 8, 8, 8, 8, 8,
  4, 2, 4, 8, 8, 4,
  2, 8, 8, 8, 8, 8, 4
};

int currentNote = 0;
unsigned long previousNoteTime = 0;
int noteDuration = 0;
int tempo = 30;
bool musicEnabled = true;

// ===== 循迹模式变量 =====
int leftSensorValue = 0;
int rightSensorValue = 0;
unsigned long lostLineStartTime = 0;
bool isTrackingLost = false;
const unsigned long LOST_TIME_THRESHOLD = 100;

// ===== 避障模式变量 =====
const int OBSTACLE_THRESHOLD = 500;
bool lastLeftObstacle = false;
unsigned long lastActionTime = 0;
int stuckCounter = 0;

// ===== 初始化设置 =====
void setup() {
  // 设置引脚模式
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(leftSensorPin, INPUT);
  pinMode(rightSensorPin, INPUT);
  
  // 初始化串口
  Serial.begin(115200);
  
  // 停止所有电机
  stopMotors();
  
  Serial.println("多功能蓝牙小车已就绪！");
  printModeInfo();
}

// ===== 主循环 =====
void loop() {
  // 检查蓝牙指令
  checkBluetoothCommand();
  
  // 根据当前模式执行相应功能
  switch(currentMode) {
    case MODE_MUSIC:
      musicMode();
      break;
    case MODE_TRACKING:
      trackingMode();
      break;
    case MODE_AVOIDANCE:
      avoidanceMode();
      break;
  }
}

// ===== 蓝牙指令处理 =====
void checkBluetoothCommand() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    switch(command) {
      // 运动控制指令
      case 'F': forward(); Serial.println("前进"); break;
      case 'B': backward(); Serial.println("后退"); break;
      case 'L': left(); Serial.println("左转"); break;
      case 'R': right(); Serial.println("右转"); break;
      case 'S': stopMotors(); Serial.println("停止"); break;
      
      // 模式切换指令
      case '1': setMode(MODE_MUSIC); break;
      case '2': setMode(MODE_TRACKING); break;
      case '3': setMode(MODE_AVOIDANCE); break;
      
      // 音乐控制
      case 'M': toggleMusic(); break;
      
      // 帮助信息
      case 'H': printHelp(); break;
    }
  }
}

// ===== 模式设置函数 =====
void setMode(int newMode) {
  currentMode = newMode;
  stopMotors(); // 切换模式时停止电机
  noTone(buzzerPin); // 停止音乐
  
  printModeInfo();
}

void printModeInfo() {
  Serial.println("=== 当前模式 ===");
  switch(currentMode) {
    case MODE_MUSIC:
      Serial.println("模式1: 蓝牙音乐控制");
      Serial.println("指令: F-前进 B-后退 L-左转 R-右转 S-停止 M-切换音乐");
      break;
    case MODE_TRACKING:
      Serial.println("模式2: 自动循迹");
      Serial.println("小车将自动跟随黑线行驶");
      break;
    case MODE_AVOIDANCE:
      Serial.println("模式3: 自动避障");
      Serial.println("小车将自动避开障碍物");
      break;
  }
  Serial.println("切换模式: 1-音乐 2-循迹 3-避障 H-帮助");
  Serial.println("=================");
}

void printHelp() {
  Serial.println("=== 蓝牙小车指令帮助 ===");
  Serial.println("运动控制: F-前进 B-后退 L-左转 R-右转 S-停止");
  Serial.println("模式切换: 1-音乐模式 2-循迹模式 3-避障模式");
  Serial.println("音乐控制: M-切换音乐开关");
  Serial.println("帮助信息: H-显示此帮助");
  Serial.println("=========================");
}

// ===== 音乐模式 =====
void musicMode() {
  if (musicEnabled) {
    playMusic();
  } else {
    noTone(buzzerPin);
  }
}

void playMusic() {
  unsigned long currentTime = millis();
  
  if (currentTime - previousNoteTime >= noteDuration) {
    noTone(buzzerPin);
    
    if (currentNote < sizeof(melody) / sizeof(melody[0])) {
      noteDuration = (60000 / tempo) / noteDurations[currentNote];
      
      if (melody[currentNote] != NOTE_0) {
        tone(buzzerPin, melody[currentNote], noteDuration * 0.9);
      }
      
      currentNote++;
    } else {
      currentNote = 0;
    }
    
    previousNoteTime = currentTime;
  }
}

void toggleMusic() {
  musicEnabled = !musicEnabled;
  Serial.print("音乐状态: ");
  Serial.println(musicEnabled ? "开启" : "关闭");
  
  if (!musicEnabled) {
    noTone(buzzerPin);
    currentNote = 0;
  }
}

// ===== 循迹模式 =====
void trackingMode() {
  leftSensorValue = digitalRead(leftSensorPin);
  rightSensorValue = digitalRead(rightSensorPin);
  
  if (leftSensorValue == HIGH && rightSensorValue == HIGH) {
    if (!isTrackingLost) {
      lostLineStartTime = millis();
      isTrackingLost = true;
    } else {
      unsigned long currentTime = millis();
      unsigned long lostDuration = currentTime - lostLineStartTime;
      
      if (lostDuration >= LOST_TIME_THRESHOLD) {
        stopMotors();
        
        while (digitalRead(leftSensorPin) == HIGH && digitalRead(rightSensorPin) == HIGH) {
          delay(10);
        }
        
        isTrackingLost = false;
        return;
      } else {
        moveForward();
      }
    }
  } else {
    if (isTrackingLost) {
      isTrackingLost = false;
    }
    
    if (leftSensorValue == LOW && rightSensorValue == LOW) {
      moveForward();
    } else if (leftSensorValue == LOW && rightSensorValue == HIGH) {
      turnLeft();
    } else if (leftSensorValue == HIGH && rightSensorValue == LOW) {
      turnRight();
    }
  }
  
  delay(50);
}

// ===== 避障模式 =====
void avoidanceMode() {
  int leftValue = analogRead(SENSOR_LEFT);
  int frontValue = analogRead(SENSOR_FRONT);
  int rightValue = analogRead(SENSOR_RIGHT);
  
  bool leftObstacle = leftValue < OBSTACLE_THRESHOLD;
  bool frontObstacle = frontValue < OBSTACLE_THRESHOLD;
  bool rightObstacle = rightValue < OBSTACLE_THRESHOLD;
  
  if (millis() - lastActionTime > 3000) {
    stuckCounter++;
    
    if (stuckCounter >= 2) {
      executeEscapeRoutine();
      stuckCounter = 0;
      lastActionTime = millis();
      return;
    }
  }
  
  if (frontObstacle) {
    stopMotors();
    delay(200);
    
    if (!rightObstacle) {
      turnRight();
      delay(400);
      stopMotors();
      delay(100);
    } else if (!leftObstacle) {
      turnLeft();
      delay(400);
      stopMotors();
      delay(100);
    } else {
      moveBackward();
      delay(300);
      turnRight();
      delay(300);
      stopMotors();
      delay(100);
    }
  } else if (rightObstacle && !frontObstacle) {
    slightLeft();
    lastActionTime = millis();
  } else {
    if (leftObstacle) {
      if (leftObstacle && !lastLeftObstacle) {
        slightRight();
        delay(200);
        stopMotors();
        delay(100);
        slightRight();
        delay(300);
      } else {
        slightRight();
        delay(100);
      }
      moveForward();
    } else {
      slightLeft();
    }
    lastActionTime = millis();
  }
  
  lastLeftObstacle = leftObstacle;
  delay(100);
}

void executeEscapeRoutine() {
  moveBackward();
  delay(500);
  stopMotors();
  delay(200);
  
  turnRight();
  delay(800);
  stopMotors();
  delay(200);
  
  moveForward();
  delay(1000);
  stopMotors();
  delay(200);
}

// ===== 电机控制函数 =====
void moveForward() {
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
}

void moveBackward() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
}

void turnLeft() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
}

void turnRight() {
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
}

void slightLeft() {
  static unsigned long lastTime = 0;
  static bool leftMotorState = false;
  
  unsigned 
