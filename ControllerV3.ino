//Ayman Osman Balance Controller


#include "Wire.h"
#include <MPU6050_light.h>
#include <AccelStepper.h>
#include <freertos/task.h>
#include <ctype.h>
#include <Arduino.h>
#include <SimpleKalmanFilter.h>




#define STEP_PIN_L 26
#define DIR_PIN_L  27
#define STEP_PIN_R 14
#define DIR_PIN_R 33
#define stepsPerRevolution 200*16


TaskHandle_t Task1;
MPU6050 mpu(Wire);
AccelStepper stepperL(AccelStepper::DRIVER,STEP_PIN_L, DIR_PIN_L);
AccelStepper stepperR(AccelStepper::DRIVER,STEP_PIN_R, DIR_PIN_R);

//Tuning mark 2 chasis
//double Kp = 400, Ki=0, Kd = 0; // Not enough P strength
//double Kp = 700, Ki=0, Kd = 0; // Not enough P strength, a lot of jitter increase deadzone
//double Kp = 700, Ki=0, Kd = 0; // Not enough P strength
//double Kp = 1000, Ki=0, Kd = 0; // Oscillations decrease P
//double Kp = 800, Ki=0, Kd = 0; // Oscillations decrease P

//double Kp = 700, Ki=0.5, Kd = 150; // Oscillations decrease P

//double Kp = 600, Ki=0.5, Kd = 75, K = 1; // test
//double Kp = 850, Ki = 7, Kd = 300, K = 1; // mark 2 pid
double Kp = 900, Ki = 0.1, Kd = 10, K = 1; // test


volatile int control=0;
volatile double setpointD = 0, setpoint = 0,input = 0, output = 0, integral = 0, derivative = 0,errorD = 0, dt = 0, error = 0, proportional, offset = 0;
volatile double prevInput1, prevInput2, prevInput3, prevInput4, prevInput5, prevInput6, prevInput7, prevInput8, prevInput9, prevInput10; 

void Task1code( void * pvParameters ){ // Core 0 code
  // Code for control
  Serial.begin(9600);
  Wire.begin();
  SimpleKalmanFilter simpleKalmanFilter(3, 3, 0.4);




  char controlValue;
  unsigned long timer=0;
  int count = 0;
   
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  

  mpu.calcOffsets(); // gyro and accelero

  int WINDOW_SIZE = 10;
  int INDEX = 0;
  double VALUE = 0;
  double SUM = 0;
  double READINGS[WINDOW_SIZE];
  double AVERAGED = 0;

  double estimated_angle;

  double controller_input;

  double dist_travelled = 0;

  estimated_angle = simpleKalmanFilter.updateEstimate(0);

  



  while (true) {
    SUM = SUM - READINGS[INDEX];       // Remove the oldest entry from the sum
    mpu.update();             //Get new gyro angle
    input = mpu.getAngleY();
                                       //moving average code from https://maker.pro/arduino/tutorial/how-to-clean-up-noisy-sensor-data-with-a-moving-average-filter
    VALUE = input;                     // Read the next sensor value
    READINGS[INDEX] = VALUE;           // Add the newest reading to the window
    SUM = SUM + VALUE;                 // Add the newest reading to the sum
    INDEX = (INDEX+1) % WINDOW_SIZE;   // Increment the index, and wrap to 0 if it exceeds the window size

    AVERAGED = SUM / WINDOW_SIZE;

  
    estimated_angle = simpleKalmanFilter.updateEstimate(AVERAGED); //kalman filter

    controller_input = AVERAGED;


    if(control == 0){
      setpoint = 0;
    }
    
    if(control == 1){
        if(millis() - timer > 1000){
          timer = millis();
          if(setpoint != 0){
            setpoint = 0;
          }
          else{
            setpoint = 3;
          }
      }
    }
    
    if(control == 2){
        if(millis() - timer > 1000){
          timer = millis();
          if(setpoint != 0){
            setpoint = 0;
          }
          else{
            setpoint = -3;
          }
      }
    }

    
    if(control == 3){
      setpoint = 0;
        if(millis() - timer > 100){
          timer = millis();
          if(count > 1){
            if(abs(input < 2)){
              offset = 2500;
            }
            else{
              offset = 0;
            }
            count = 0;
            
          }
          else{
            offset = 0;
            count = count + 1;
          }
      }
    }
    if(control == 4){
      setpoint = 0;
        if(millis() - timer > 100){
          timer = millis();
          if(count > 1){
            if(abs(input < 2)){
              offset = -2500;
            }
            else{
              offset = 0;
            }
            count = 0;
            
          }
          else{
            offset = 0;
            count = count + 1;
          }
      }
    }

    if(millis()>7500){
      errorD = setpointD - dist_travelled;
      errorD = constrain(errorD,-2/K,2/K);
      //setpoint = K * errorD;
  
      error = setpoint - controller_input;   //pitch angle PID Control block
      proportional = Kp * round(error*10)/10;
      integral += Ki * round(error*10)/10;
      integral = constrain(integral, -3000, 3000);
      derivative = Kd * (round(prevInput1*10)/10 - round(controller_input*10)/10);
      derivative = constrain(derivative,-20000,20000);

      output = proportional + integral + derivative;
      
      if(abs(controller_input) > 15){
         output = 0;
      }
  
  
      if(abs(controller_input) < 1){
         output = 0;
      }
  
      

//      Serial.print("Angle:");
//      Serial.print(controller_input);
//      Serial.print(",");
//      Serial.print("Output");
//      Serial.println(output);
  
      dist_travelled = -(dist_travelled + (output*0.22*0.003)) / stepsPerRevolution;
      
      
      prevInput10 = prevInput9;
      prevInput9 = prevInput8;
      prevInput8 = prevInput7;
      prevInput7 = prevInput6;
      prevInput6 = prevInput5;
      prevInput5 = prevInput4;
      prevInput4 = prevInput3;
      prevInput3 = prevInput2;
      prevInput2 = prevInput1;
      prevInput1 = controller_input;
    }

   vTaskDelay(1);
  }
}



void setup() {
  //Serial.begin(9600);
  
  stepperL.setMaxSpeed(32000);     // Maximum speed in steps per second
  stepperL.setAcceleration(16000);  // Acceleration in steps per second^2
  
  stepperR.setMaxSpeed(32000);     // Maximum speed in steps per second
  stepperR.setAcceleration(16000);  // Acceleration in steps per second^2





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

    stepperL.setSpeed(output+offset);
    stepperR.setSpeed(-output+offset);

    stepperL.runSpeed();
    stepperR.runSpeed();

  //Serial.println(output);
  


}
