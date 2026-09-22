// 定义电机控制引脚
const int leftMotorPin1 = 10;   // 左轮引脚1
const int leftMotorPin2 = 9;  // 左轮引脚2
const int rightMotorPin1 = 8;  // 右轮引脚1
const int rightMotorPin2 = 4;  // 右轮引脚2

// 定义蜂鸣器引脚
const int buzzerPin = 2;

// 《夜的钢琴曲5》歌曲参数
int melody[] = {
  392, 440, 494, 523, 587, 659, 698, 784,
  880, 784, 698, 659, 587, 523, 494, 440,
  392, 440, 494, 523, 587, 659, 698, 784,
  880, 784, 698, 659, 587, 523, 494, 440,
  392, 330, 392, 440, 494, 523, 587, 659,
  698, 659, 587, 523, 494, 440, 392, 330,
  294, 330, 392, 440, 494, 523, 587, 659,
  698, 659, 587, 523, 494, 440, 392, 330
};

int noteDurations[] = {
  4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4
};

int currentNote = 0;
unsigned long previousNoteTime = 0;
int noteDuration = 0;
int tempo = 120; // 节奏速度（每分钟拍数）

// 音乐控制变量
bool musicEnabled = true; // 默认开启音乐

void setup() {
  // 设置电机控制引脚为输出模式
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  
  // 设置蜂鸣器引脚为输出模式
  pinMode(buzzerPin, OUTPUT);
  
  // 初始化串口通信，波特率设置为115200
  Serial.begin(115200);
  
  // 初始状态下停止所有电机
  stopMotors();
  
  Serial.println("蓝牙小车已就绪，等待指令...");
  Serial.println("指令说明:");
  Serial.println("F - 前进");
  Serial.println("B - 后退");
  Serial.println("L - 左转");
  Serial.println("R - 右转");
  Serial.println("S - 停止");
  Serial.println("M - 切换音乐开关");
  Serial.print("当前音乐状态: ");
  Serial.println(musicEnabled ? "开启" : "关闭");
}

void loop() {
  // 如果音乐开启，播放《夜的钢琴曲5》（非阻塞方式）
  if (musicEnabled) {
    playNightPiano5();
  } else {
    // 如果音乐关闭，确保没有声音输出
    noTone(buzzerPin);
  }
  
  // 检查是否有可用的蓝牙数据
  if (Serial.available() > 0) {
    char command = Serial.read(); // 读取接收到的命令
    
    // 根据接收到的命令执行相应动作
    switch(command) {
      case 'F': // 前进
        forward();
        Serial.println("前进");
        break;
      case 'B': // 后退
        backward();
        Serial.println("后退");
        break;
      case 'L': // 左转
        left();
        Serial.println("左转");
        break;
      case 'R': // 右转
        right();
        Serial.println("右转");
        break;
      case 'S': // 停止
        stopMotors();
        Serial.println("停止");
        break;
      case 'M': // 切换音乐开关
        toggleMusic();
        break;
      default:
        // 未知命令
        break;
    }
  }
}

// 播放《夜的钢琴曲5》函数（非阻塞方式）
void playNightPiano5() {
  unsigned long currentTime = millis();
  
  if (currentTime - previousNoteTime >= noteDuration) {
    // 停止当前音符
    noTone(buzzerPin);
    
    // 播放下一个音符
    if (currentNote < sizeof(melody) / sizeof(melody[0])) {
      // 计算音符持续时间（基于节拍）
      noteDuration = (60000 / tempo) / noteDurations[currentNote];
      
      // 播放音符
      tone(buzzerPin, melody[currentNote], noteDuration * 0.9); // 播放90%的时间，留10%作为间隔
      
      // 移动到下一个音符
      currentNote++;
    } else {
      // 回到歌曲开头
      currentNote = 0;
    }
    
    // 更新上次播放时间
    previousNoteTime = currentTime;
  }
}

// 切换音乐开关函数
void toggleMusic() {
  musicEnabled = !musicEnabled;
  Serial.print("音乐状态: ");
  Serial.println(musicEnabled ? "开启" : "关闭");
  
  // 如果关闭音乐，确保停止所有声音
  if (!musicEnabled) {
    noTone(buzzerPin);
    currentNote = 0; // 重置到歌曲开头
  }
}

// 前进函数
void forward() {
  // 左轮前进
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  // 右轮前进
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
}

// 后退函数
void backward() {
  // 左轮后退
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
  // 右轮后退
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
}

// 左转函数
void left() {
  // 左轮后退
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
  // 右轮前进
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
}

// 右转函数
void right() {
  // 左轮前进
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  // 右轮后退
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
}

// 停止电机函数
void stopMotors() {
  // 停止左轮
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);
  // 停止右轮
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
}