//Ayman Osman Balance Controller


#include "Wire.h"
#include <MPU6050_light.h>
#include <AccelStepper.h>
#include <freertos/task.h>
#include <ctype.h>
#include <Arduino.h>

#define STEP_PIN_L 26
#define DIR_PIN_L  27
#define STEP_PIN_R 14
#define DIR_PIN_R 33
#define stepsPerRevolution 200

TaskHandle_t Task1;
MPU6050 mpu(Wire);
AccelStepper stepperL(AccelStepper::DRIVER,STEP_PIN_L, DIR_PIN_L);
AccelStepper stepperR(AccelStepper::DRIVER,STEP_PIN_R, DIR_PIN_R);



double Kp=1000, Ki=20, Kd=200; // PID VALUES

volatile int control=1;
volatile double setpoint = 0,input = 0, output = 0, prevInput = 0, integral = 0, derivative = 0, del = 0, dt = 0, error, proportional, offset = 0;

void Task1code( void * pvParameters ){ // Core 0 code
  // Code for control
  Serial.begin(9600);
  Wire.begin();

   
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  

  mpu.calcOffsets(); // gyro and accelero



  while (true) {

    mpu.update();             //Get new gyro angle
    input = mpu.getAngleY();

    if(control == 0){
      setpoint = 0;
    }
    if(control == 1){
      setpoint = 3;
    }
    if(control == 2){
      setpoint = -3;
    }
    if(control == 3){
      setpoint = 0;
    }
    if(control == 4){
      setpoint = 0;
    }

    
    error = setpoint - input;   //PID Control block
    proportional = Kp * error;
    integral += Ki * error;
    integral = constrain(integral, -3000, 3000);
    derivative = Kd * (input - prevInput);
    derivative = constrain(derivative,-25000,25000);


    
    output = proportional + integral + derivative;
    
    
    
    prevInput = input;


//    For testing
//  Serial.print("Input:");
//    Serial.print("\t");
//    Serial.print(input);
//    
//    Serial.print("\tOutput:");
//    Serial.print("\t");
//    Serial.print(output);
//    
//    Serial.print("\tProp:");
//    Serial.print("\t");
//    Serial.print(proportional);
//    
//    Serial.print("\tInt:");
//    Serial.print("\t");
//    Serial.print(integral);
//    
//    Serial.print("\tDeriv:");
//    Serial.print("\t");
//    Serial.println(derivative);
//       


   vTaskDelay(5);
  }
}



void setup() {
  //Serial.begin(9600);
  
  stepperL.setMaxSpeed(32000);     // Maximum speed in steps per second
  stepperL.setAcceleration(8000);  // Acceleration in steps per second^2
  
  stepperR.setMaxSpeed(15000);     // Maximum speed in steps per second
  stepperR.setAcceleration(8000);  // Acceleration in steps per second^2





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


  if(abs(output) < 300){
    output = 0;
  }
  
  
  
  if(abs(input) > 20){
      output = 0;
      offset = 0;
   }

  stepperL.setSpeed(-output + offset);
  stepperR.setSpeed(output + offset);

  stepperL.runSpeed();
  stepperR.runSpeed();
  


}
