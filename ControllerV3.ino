/* Get tilt angles on X and Y, and rotation angle on Z
 * Angles are given in degrees
 * 
 * License: MIT
 */

#include "Wire.h"
#include <MPU6050_light.h>
#include <AccelStepper.h>






#define STEP_PIN_L 26
#define DIR_PIN_L 27


#define STEP_PIN_R 33//14
#define DIR_PIN_R 25//12

AccelStepper stepperL(AccelStepper::DRIVER,STEP_PIN_L, DIR_PIN_L);
AccelStepper stepperR(AccelStepper::DRIVER,STEP_PIN_R, DIR_PIN_R);

#define stepsPerRevolution 200


TaskHandle_t Task1;
volatile double input = 0;

MPU6050 mpu(Wire);
unsigned long timer = 0;
volatile double setpoint = 0, output = 0, prevInput = 0, integral = 0, derivative = 0, del = 0, dt = 0, error, proportional;
volatile unsigned long prevTime = 0, prevMicros = 0;

unsigned long highTime, lowTime, cycleTime;
float angle;
double Kp=100, Ki=5, Kd=0.0;

int stepL = 0;
int stepR = 0;

void Task1code( void * pvParameters ){
  // Code for task 1
  Serial.begin(9600);
  Wire.begin();
  
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");


  while (true) {
    mpu.update();
    //analogWrite(25, mpu.getAngleY()+128);
    //Serial.println(mpu.getAngleY()+128);

    input = mpu.getAngleY();

    dt = (micros() - prevTime);
    error = setpoint - input;
    proportional = Kp * error;
    integral += Ki * error * dt;
    integral = constrain(integral, -100, 100);
    derivative = Kd * (input - prevInput) * 1000000 / dt;
    output = proportional + integral + derivative;
    
    prevInput = input;
    prevTime = micros();

    
    delay(1);

   
  }
}



void setup() {
  //Serial.begin(9600);
  
  stepperL.setMaxSpeed(1000);     // Maximum speed in steps per second
  stepperL.setAcceleration(200);  // Acceleration in steps per second^2
  
  stepperR.setMaxSpeed(1000);     // Maximum speed in steps per second
  stepperR.setAcceleration(200);  // Acceleration in steps per second^2

 xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */   

}

void loop() {
  //highTime = pulseIn(33, HIGH);
  //lowTime = pulseIn(33, LOW);
  //cycleTime = highTime + lowTime;
  //if(cycleTime != 0){
    //input = ((highTime*255) / cycleTime)-128;
  //}
  //else{
    //Serial.println("ERROR");
  //}

  if(abs(output) < 200){
    output = 0;
  }


  stepperL.setSpeed(-output);
  stepperR.setSpeed(output);

  stepperL.runSpeed();
  stepperR.runSpeed();
  //Serial.println(xPortGetCoreID());

  //Serial.print("Input: ");
  //Serial.print(input);
  //Serial.print("\t Output: ");
  //Serial.println(output);



  //if(input < 35 && input > -35){

  //}
}
