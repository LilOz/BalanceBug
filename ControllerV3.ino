/* Get tilt angles on X and Y, and rotation angle on Z
 * Angles are given in degrees
 * 
 * License: MIT
 */

#include "Wire.h"
#include <MPU6050_light.h>
#include <AccelStepper.h>






#define STEP_PIN_L 26
#define DIR_PIN_L  27


#define STEP_PIN_R 14
#define DIR_PIN_R 12

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
double Kp=1000, Ki=100, Kd=10;

int stepL = 0;
int stepR = 0;

void Task1code( void * pvParameters ){
  // Code for control
  Serial.begin(9600);
  Wire.begin();
  
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
  //Serial.println(F("Calculating offsets, do not move MPU6050"));
  //delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
  //Serial.println("Done!\n");


  while (true) {
    mpu.update();
    //analogWrite(25, mpu.getAngleY()+128);
    //Serial.println(mpu.getAngleY()+128);

    input = mpu.getAngleY();

    //dt = (micros() - prevTime); dt = 0.002
    error = setpoint - input;
    proportional = Kp * error;
    integral += Ki * error;
    integral = constrain(integral, -3000, 3000);
    derivative = Kd * (input - prevInput);

    if(abs(input) > 30){
      output = 0;
    }
    else{
      output = proportional + integral + derivative;
    }
    
    
    prevInput = input;
    //prevTime = micros();
    //Serial.print("Input:");
    //Serial.print("\t");
    //Serial.print(input);
    
    //Serial.print("\tOutput:");
    //Serial.print("\t");
    //Serial.print(output);
    
    //Serial.print("\tProp:");
    //Serial.print("\t");
    //Serial.print(proportional);
    
    //Serial.print("\tInt:");
    //Serial.print("\t");
    //Serial.print(integral);
    
    //Serial.print("\tDeriv:");
    //Serial.print("\t");
    //Serial.println(derivative);
       

    
    delay(1);

   
  }
}



void setup() {
  //Serial.begin(9600);
  
  stepperL.setMaxSpeed(32000);     // Maximum speed in steps per second
  stepperL.setAcceleration(8000);  // Acceleration in steps per second^2
  
  stepperR.setMaxSpeed(15000);     // Maximum speed in steps per second
  stepperR.setAcceleration(8000);  // Acceleration in steps per second^2

  pinMode(33, OUTPUT);
  pinMode(32, OUTPUT);

  digitalWrite(33, HIGH);
  digitalWrite(32, HIGH);

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


  if(abs(output) < 150){
    output = 0;
  }


  stepperL.setSpeed(-output);
  stepperR.setSpeed(output);

  stepperL.runSpeed();
  stepperR.runSpeed();



}
