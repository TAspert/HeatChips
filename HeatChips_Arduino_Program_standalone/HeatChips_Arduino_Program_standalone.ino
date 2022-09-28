// This program is used to control a heating system in which the temperature is measured by an infrared MLX90614 sensor.
// The program:
//1. reads the temperatures from the sensor
//2. computes a PID output by comparing the measure with the setpoint and send it to a pin as a PWM signal (0-255)
//3. prints the temperatures in a serial output. These values that can be read using the built-in Serial monitor of the Arduino Software (for example), accessible in the Tools menu.
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


int PWM_pin=11; // Pin to which the PWM output is sent. Connect this pin to the MOSFET gate
int Vref=5;
double temperature_read;
double temperature_mean;
double maxtemp=40; //max temperature allowed
double PID_value=0;


Adafruit_MLX90614 mlx = Adafruit_MLX90614();
PID myPID(&temperature_mean, &PID_value, &set_temperature, kp, ki, kd,DIRECT);


void setup() {
  Serial.begin(9600);
  myPID.SetMode(AUTOMATIC);
  mlx.begin();
}

void loop() {
  
  //*******read temperature during readfreq seconds and averages********//
  temperature_read = mlx.readObjectTempC();
  for (int i=1; i<readfreq; i++){
    delay(1000);
    temperature_read=temperature_read+mlx.readObjectTempC();}
    
  temperature_mean=(temperature_read)/readfreq;

  //*******Compute PID********//
  if (temperature_mean>0 && temperature_mean<maxtemp){
    myPID.Compute();
    analogWrite(PWM_pin,PID_value);
  }

  //*******print values********//
  Serial.print("Object temperature: ");
  Serial.print(temperature_mean);
  Serial.print(", Ambient temperature: ");
  Serial.print(mlx.readAmbientTempC());
  Serial.print(", PWM value: ");
  Serial.println(PID_value);
  //delay(500);
  
}
