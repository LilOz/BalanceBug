
#define stprPin 2
#define dirrPin 3
#define stplPin 4
#define dirlPin 5
#define ms1 6
#define ms2 7



void setup() {
  Serial.begin(9600);
  pinMode(LeftSensor, INPUT);
  pinMode(RightSensor, INPUT);
  pinMode(stprPin,   OUTPUT);
  pinMode(dirrPin, OUTPUT);
  pinMode(stplPin, OUTPUT);
  pinMode(dirlPin, OUTPUT);
  pinMode(ms1,OUTPUT);
  pinMode(ms2,OUTPUT);
}

void Move(int steps) {

  int direction = (steps > 0) ? 1 : 0;
  digitalWrite(dirrPin, direction);
  digitalWrite(dirlPin, direction);
  steps=abs(steps);
  for (int step = 0; step < steps; step++) {
    digitalWrite(stprPin, HIGH);
    digitalWrite(stplPin, HIGH);
    delay(3);
    digitalWrite(stprPin, LOW);
    digitalWrite(stplPin, LOW);
    delay(1);
  }
}
// right is +ive
void Rotate(int steps) {
  int direction = (steps > 0) ? 1 : 0;
  digitalWrite(dirrPin, direction);
  digitalWrite(dirlPin, !direction);
  steps=abs(steps);
  for (int step = 0; step < steps; step++) {
      digitalWrite(stprPin, HIGH);
      digitalWrite(stplPin, HIGH);
      int f = steps < 100 ? 4: 3;
      delay(f);
      digitalWrite(stprPin, LOW);
      digitalWrite(stplPin, LOW);
      delay(1);
  }}
