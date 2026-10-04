const int trigPin = 8;
const int echoPin = 7;
const int buzzerPin = 9;
const int motorPin=10;
// 滑动滤波参数
const int numReadings = 3;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int averageDistance = 0;
int lastDistance = 0;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(motorPin,OUTPUT);
  Serial.begin(9600);

  for (int i = 0; i < numReadings; i++) readings[i] = 0;
}

void loop() {
  //  触发超声波
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  //  读取距离
  long duration = pulseIn(echoPin, HIGH, 15000);
  int currentDistance;
  if (duration==0)
  {
    currentDistance=999;
  }else{currentDistance=duration*0.034/2;}

  //  滑动平均滤波
  total = total - readings[readIndex];
  readings[readIndex] = currentDistance;
  total = total + readings[readIndex];
  readIndex = (readIndex + 1) % numReadings;
  averageDistance = total / numReadings;

  //  计算变化率
  int changeRate = averageDistance - lastDistance;

  Serial.print(currentDistance);
  Serial.print(" ");
  Serial.println(averageDistance);

  //  行人过滤：如果距离瞬间剧烈变化，判定为行人路过，跳过报警
  if (abs(changeRate) > 30) {
    noTone(buzzerPin);
    lastDistance = averageDistance;
    delay(30);
    return;
  }

  //  分级报警
  if (averageDistance <= 50) {
    // 危险区：急促报警 + 强震动
    tone(buzzerPin, 1000);
    analogWrite(motorPin,255);
   
    delay(100);
    noTone(buzzerPin);
    analogWrite(motorPin,0);
    delay(20);
  } 
  else if (averageDistance <= 150) {
    // 警戒区：缓慢报警 + 弱震动
    tone(buzzerPin, 500);
      analogWrite(motorPin,255);

    delay(200);
    noTone(buzzerPin);
    analogWrite(motorPin,0);
    delay(200);
  } 
  else {
    // 安全区
    noTone(buzzerPin);
     digitalWrite(motorPin,LOW);
      analogWrite(motorPin,0);
  }

  lastDistance = averageDistance;
  delay(30);
}
