// This program is used to control a heating system in which the temperature is measured by an infrared MLX90614 sensor.
// The program:
//1. reads the temperatures from the sensor
//2. computes a PID output by comparing the measure with the setpoint and send it to a pin as a PWM signal (0-255)
//3. prints the temperature in a format read by the read_HeatChips.m matlab script associated with the file and available at https://github.com/TAspert/HeatChips, and plot all the relevant informations
// Before uploading the program to the arduino board, you must download the PID_v1.h (https://playground.arduino.cc/Code/PIDLibrary/) and Adafruit_MLX90614.h (https://github.com/adafruit/Adafruit-MLX90614-Library) libraries from the menu of your Arduino software (Tools -> Library manager).
// More information at https://github.com/TAspert/HeatChips
// Contact: theo.aspert@gmail.com

#include <Wire.h>
#include <PID_v1.h>
#include <Adafruit_MLX90614.h>



//***** PID VALUES, CHANGE THE PID VALUES IN THE LINES BELOW *****//
int kp=50; // P coefficient. Increasing P decreases the time to equilibrium but increases the overshoot. A value too high will lead to oscillations.
int ki=10; // I coefficient. Increase if a static error exists (error at equilibrium). A value too high will increase overshooting and create oscillations
int kd=0; // D coefficient, keep =0 for non-experts.

//***** SET TEMPERATURE, CHANGE THE SETPOINT IN THE LINE BELOW *****//
double set_temperature=30; // Set temperature, in °C

//***** READING FREQUENCY, CHANGE IT IN THE LINES BELOW *****//
int readfreq=10; //The probe reads the temperature every second for readfreq seconds, averages the value, and send it to the PWN output computation. Decreasing this value would increase temporal resolution and modify the Integral response (I parameter would need to be adapted)

//
double maxtemp=40; //maximum object temperature allowed before considering it as aberrant measurment
//************************END OF CUSTOMISATION******************************//


int PWM_pin=3; // Pin to which the PWM output is sent. Connect this pin to the MOSFET gate
int Vref=5;
double timer=0;
double temperature_read;
double temperature_mean;



float PID_error=0;
double PID_value=0;
String PID_frozeStr="";
long PID_froze=150;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
PID myPID(&temperature_mean, &PID_value, &set_temperature, kp, ki, kd,DIRECT);


void setup() {
  Serial.begin(9600);
  myPID.SetMode(AUTOMATIC);
  mlx.begin();
}

void loop() {
  
  //=====if Matlab paused, use fixed PID_value=====
  timer=millis();
  while(!Serial.available() ){
    //Threshold timer necessary because serial is always unavailable for a few µs in normal conditions
    if (millis()-timer>5000){
      PID_value=PID_froze;
      analogWrite(PWM_pin,PID_value);
    }
  }
  PID_frozeStr=Serial.readStringUntil('\n');
  PID_froze=PID_frozeStr.toInt();

  Serial.print("PID_froze: ");
  Serial.println(PID_froze);
  //=====END Matlab paused=====

  //*******read temperature during readfreq seconds and averages********//
  temperature_read = mlx.readObjectTempC();
  for (int i=1; i<readfreq; i++){
    delay(1000);
    temperature_read=temperature_read+mlx.readObjectTempC();}
    
  temperature_mean=(temperature_read)/readfreq;

  //*******Compute PID********//
  if (temperature_mean>0 && temperature_mean<maxtemp){// && freeze==0){
    myPID.Compute();
    analogWrite(PWM_pin,PID_value);
  }

  //*******print object temperature********//
  Serial.print(temperature_mean);
  Serial.print(", ");
  //*******print ambient temperature value********//
  Serial.print(mlx.readAmbientTempC());
  //*******print PWM value********//
  Serial.print(", ");
  Serial.println(PID_value);
  //delay(500);
  
}
